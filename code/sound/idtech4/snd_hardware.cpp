// SPDX-License-Identifier: GPL-3.0-or-later
//
// The hardware layer of the id Tech 4 sound port: the BFG edition
// SoundHardware interface (references/DOOM-3-BFG/neo/sound/SoundHardware.h)
// implemented over OpenAL. It owns the source pool the voices come from,
// the cinematic raw sample channels and the wav file streams. The streams
// are fed by the sound pump thread (UpdateStreams) and loaded by the main
// thread (ServiceStreamLoads), the same producer split the BFG engine has
// between its main thread and the audio device.

#include "snd_local.h"

idSoundHardware::idSoundHardware() {
	device = nullptr;
	context = nullptr;
	PauseDeviceSOFT = nullptr;
	ResumeDeviceSOFT = nullptr;
	for ( int i = 0; i < SOUND_MAX_RAW; ++i ) {
		rawSources[i] = 0;
	}
	for ( int i = 0; i < SOUND_MAX_STREAMS; ++i ) {
		streams[i].source = 0;
		streams[i].cursor = 0;
		streams[i].loop[0] = 0;
		streams[i].entity = -1;
		streams[i].channel = 0;
		streams[i].attenuated = false;
		streams[i].needsLoad = false;
		streams[i].active = false;
	}
	for ( int i = 0; i < MAX_GENTITIES; ++i ) {
		VectorClear( entityPositions[i] );
	}
}

bool idSoundHardware::Init() {
	device = alcOpenDevice( nullptr );
	if ( !device ) {
		Com_Printf( "OpenAL: could not open an audio device\n" );
		return false;
	}
	context = alcCreateContext( device, nullptr );
	if ( !context || !alcMakeContextCurrent( context ) ) {
		if ( context ) {
			alcDestroyContext( context );
		}
		context = nullptr;
		alcCloseDevice( device );
		device = nullptr;
		Com_Printf( "OpenAL: could not create audio context\n" );
		return false;
	}
	alDistanceModel( AL_LINEAR_DISTANCE_CLAMPED );
	alDopplerFactor( 0 );
	// the bundled import library does not export the pause/resume extension
	// entry points, so fetch them by name from the loaded OpenAL runtime
	PauseDeviceSOFT = reinterpret_cast<LPALCDEVICEPAUSESOFT>(
		alcGetProcAddress( device, "alcDevicePauseSOFT" ) );
	ResumeDeviceSOFT = reinterpret_cast<LPALCDEVICERESUMESOFT>(
		alcGetProcAddress( device, "alcDeviceResumeSOFT" ) );
	for ( int i = 0; i < SOUND_MAX_VOICES; ++i ) {
		ALuint source = 0;
		alGenSources( 1, &source );
		voices[i].Create( source );
	}
	return true;
}

void idSoundHardware::Shutdown() {
	for ( int i = 0; i < SOUND_MAX_VOICES; ++i ) {
		voices[i].Destroy();
	}
	for ( int i = 0; i < SOUND_MAX_RAW; ++i ) {
		if ( rawSources[i] ) {
			alDeleteSources( 1, &rawSources[i] );
			rawSources[i] = 0;
		}
	}
	for ( int i = 0; i < SOUND_MAX_STREAMS; ++i ) {
		if ( streams[i].source ) {
			alDeleteSources( 1, &streams[i].source );
			streams[i].source = 0;
		}
		streams[i].active = false;
		streams[i].wave = doomaudio::Wave();
	}
	alcMakeContextCurrent( nullptr );
	if ( context ) {
		alcDestroyContext( context );
		context = nullptr;
	}
	if ( device ) {
		alcCloseDevice( device );
		device = nullptr;
	}
	PauseDeviceSOFT = nullptr;
	ResumeDeviceSOFT = nullptr;
}

// ---------------------------------------------------------------------------
// game voices, main thread
// ---------------------------------------------------------------------------
idSoundVoice *idSoundHardware::AllocateVoice() {
	for ( int i = 0; i < SOUND_MAX_VOICES; ++i ) {
		if ( !voices[i].IsAssigned() ) {
			return &voices[i];
		}
	}
	return nullptr;
}

void idSoundHardware::FreeVoice( idSoundVoice *voice ) {
	if ( voice ) {
		voice->Stop();
	}
}

int idSoundHardware::NumFreeVoices() const {
	int freeVoices = 0;
	for ( int i = 0; i < SOUND_MAX_VOICES; ++i ) {
		if ( !voices[i].IsAssigned() ) {
			++freeVoices;
		}
	}
	return freeVoices;
}

idSoundVoice *idSoundHardware::VoiceFor( int emitterIndex, int channelIndex ) {
	for ( int i = 0; i < SOUND_MAX_VOICES; ++i ) {
		idSoundVoice &voice = voices[i];
		if ( voice.IsAssigned() && voice.EmitterIndex() == emitterIndex &&
			 voice.ChannelIndex() == channelIndex ) {
			return &voice;
		}
	}
	return nullptr;
}

// ---------------------------------------------------------------------------
// listener, main thread
// ---------------------------------------------------------------------------
void idSoundHardware::ApplyListener( const vec3_t origin, const vec3_t axis[3] ) {
	alListener3f( AL_POSITION, origin[0], origin[2], -origin[1] );
	const float orientation[] = {axis[0][0], axis[0][2], -axis[0][1],
								 axis[2][0], axis[2][2], -axis[2][1]};
	alListenerfv( AL_ORIENTATION, orientation );
}

// ---------------------------------------------------------------------------
// cinematic raw samples, main thread: OpenAL Soft serializes calls per
// source, and the pump never touches these channels
// ---------------------------------------------------------------------------
bool idSoundHardware::QueueRawSamples( int index, int samples, int rate, int width, int channels,
									   const byte *data, float left, float right ) {
	if ( index < 0 || index >= SOUND_MAX_RAW || samples <= 0 || samples > 262144 ||
		 rate < 4000 || rate > 192000 || ( width != 1 && width != 2 ) ||
		 ( channels != 1 && channels != 2 ) || !data ) {
		return false;
	}
	if ( !rawSources[index] ) {
		rawSources[index] = doomaudio::NewStreamSource();
	}
	alSourcef( rawSources[index], AL_GAIN, max( 0.0f, ( left + right ) * 0.5f ) );
	doomaudio::QueuePCM( rawSources[index], data, samples * width * channels, rate, channels, width );
	return true;
}

void idSoundHardware::SetRawStreamGain( int index, float gain ) {
	if ( index >= 0 && index < SOUND_MAX_RAW && rawSources[index] ) {
		alSourcef( rawSources[index], AL_GAIN, max( 0.0f, gain ) );
	}
}

void idSoundHardware::ClearRawStreams() {
	for ( int i = 0; i < SOUND_MAX_RAW; ++i ) {
		if ( rawSources[i] ) {
			doomaudio::ClearStream( rawSources[i] );
		}
	}
}

void idSoundHardware::UpdateEntityPosition( int entity, const vec3_t origin ) {
	// the attenuated streams follow their entity; shared with the pump
	std::lock_guard<std::mutex> guard( soundSystemLocalPtr->pumpMutex );
	if ( entity >= 0 && entity < MAX_GENTITIES ) {
		VectorCopy( origin, entityPositions[entity] );
	}
}

// ---------------------------------------------------------------------------
// wav file streams
// ---------------------------------------------------------------------------
bool idSoundHardware::ReadStream( soundStream_t &stream, const char *path ) {
	// main thread only: the filesystem is not thread safe
	void *data = nullptr;
	const int length = FS_ReadFile( path, &data );
	bool valid = false;
	if ( data && length > 0 && length <= 64 * 1024 * 1024 ) {
		valid = doomaudio::DecodeWave( static_cast<const byte *>( data ), length, stream.wave );
	}
	if ( data ) {
		FS_FreeFile( data );
	}
	stream.cursor = 0;
	if ( !valid ) {
		stream.wave = doomaudio::Wave();
		Com_Printf( "Sound stream: cannot stream %s\n", path );
	}
	return valid;
}

void idSoundHardware::StartStream( const char *intro, const char *loop, int entity, int channel,
								   int attenuation ) {
	if ( !intro || !*intro ) {
		return;
	}
	std::lock_guard<std::mutex> guard( soundSystemLocalPtr->pumpMutex );
	// the engine semantics: one streaming sound per entity
	StopEntityStreams( entity );
	for ( int i = 0; i < SOUND_MAX_STREAMS; ++i ) {
		soundStream_t &stream = streams[i];
		if ( stream.active ) {
			continue;
		}
		if ( !stream.source ) {
			stream.source = doomaudio::NewStreamSource();
		}
		doomaudio::ClearStream( stream.source );
		stream.entity = entity;
		stream.channel = channel;
		stream.attenuated = attenuation != 0;
		stream.needsLoad = false;
		Q_strncpyz( stream.loop, loop ? loop : "", sizeof( stream.loop ) );
		alSourcei( stream.source, AL_SOURCE_RELATIVE, stream.attenuated ? AL_FALSE : AL_TRUE );
		alSourcef( stream.source, AL_ROLLOFF_FACTOR, stream.attenuated ? 1.0f : 0.0f );
		alSourcef( stream.source, AL_REFERENCE_DISTANCE, 160.0f );
		alSourcef( stream.source, AL_MAX_DISTANCE, 1250.0f );
		stream.active = ReadStream( stream, intro );
		return;
	}
	Com_Printf( "Sound stream: no free streaming channel for %s\n", intro );
}

void idSoundHardware::StopStreamLocked( int index ) {
	if ( index < 0 || index >= SOUND_MAX_STREAMS ) {
		return;
	}
	soundStream_t &stream = streams[index];
	if ( !stream.active ) {
		return;
	}
	doomaudio::ClearStream( stream.source );
	stream.wave = doomaudio::Wave();
	stream.loop[0] = 0;
	stream.cursor = 0;
	stream.active = false;
}

void idSoundHardware::StopStream( int index ) {
	std::lock_guard<std::mutex> guard( soundSystemLocalPtr->pumpMutex );
	StopStreamLocked( index );
}

void idSoundHardware::StopEntityStreams( int entity ) {
	for ( int i = 0; i < SOUND_MAX_STREAMS; ++i ) {
		if ( streams[i].active && streams[i].entity == entity ) {
			StopStreamLocked( i );
		}
	}
}

void idSoundHardware::StopAllStreams() {
	std::lock_guard<std::mutex> guard( soundSystemLocalPtr->pumpMutex );
	for ( int i = 0; i < SOUND_MAX_STREAMS; ++i ) {
		StopStreamLocked( i );
	}
}

void idSoundHardware::ServiceStreamLoads() {
	// main thread, pumpMutex already held by the caller: the loop file
	// reloads the pump thread posted
	for ( int i = 0; i < SOUND_MAX_STREAMS; ++i ) {
		soundStream_t &stream = streams[i];
		if ( !stream.active || !stream.needsLoad || !stream.loop[0] ) {
			continue;
		}
		if ( ReadStream( stream, stream.loop ) ) {
			stream.needsLoad = false;
		} else {
			StopStreamLocked( i );
		}
	}
}

void idSoundHardware::UpdateStreams( float gain ) {
	// pump thread, pumpMutex held by the caller
	for ( int i = 0; i < SOUND_MAX_STREAMS; ++i ) {
		soundStream_t &stream = streams[i];
		if ( !stream.active || stream.wave.samples.empty() ) {
			continue;
		}
		alSourcef( stream.source, AL_GAIN, gain );
		ALint queued = 0, processed = 0;
		alGetSourcei( stream.source, AL_BUFFERS_QUEUED, &queued );
		alGetSourcei( stream.source, AL_BUFFERS_PROCESSED, &processed );
		// keep the BFG queue depth on the source
		for ( int count = queued - processed; count < MAX_QUEUED_BUFFERS; ++count ) {
			if ( stream.cursor == stream.wave.samples.size() ) {
				if ( !stream.loop[0] ) {
					break;
				}
				// the loop file read belongs to the main thread
				stream.needsLoad = true;
				break;
			}
			const size_t samples = min( size_t( 8192 ),
										 stream.wave.samples.size() - stream.cursor );
			doomaudio::QueuePCM( stream.source, stream.wave.samples.data() + stream.cursor,
								static_cast<int>( samples * 2 ), stream.wave.rate,
								stream.wave.channels, 2 );
			stream.cursor += samples;
		}
		if ( stream.attenuated && stream.entity >= 0 && stream.entity < MAX_GENTITIES ) {
			const vec3_t &position = entityPositions[stream.entity];
			alSource3f( stream.source, AL_POSITION, position[0], position[2], -position[1] );
		}
		// a finished non looping stream frees its slot for the next one
		if ( !stream.loop[0] && stream.cursor == stream.wave.samples.size() && queued == 0 ) {
			StopStreamLocked( i );
		}
	}
}

// ---------------------------------------------------------------------------
// device pause for the editor, main thread
// ---------------------------------------------------------------------------
void idSoundHardware::PauseDevice() {
	if ( !context ) {
		return;
	}
	alListenerf( AL_GAIN, 0 );
	if ( PauseDeviceSOFT ) {
		PauseDeviceSOFT( device );
	}
}

void idSoundHardware::ResumeDevice() {
	if ( !context ) {
		return;
	}
	if ( ResumeDeviceSOFT ) {
		ResumeDeviceSOFT( device );
	}
}
