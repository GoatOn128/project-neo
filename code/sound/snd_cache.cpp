// SPDX-License-Identifier: GPL-3.0-or-later
//
// The sample cache of the id Tech 4 sound port: snd_cache.cpp of the Doom 3
// (2004) system, adapted to the engine filesystem (FS_ReadFile instead of
// idFileSystem; switched back when idLib is merged, TODO task 5).
// The wav decoder of the old sound code lives here as well: the idWaveFile
// stand-in below (DecodeWave) reads what idWaveFile did in Doom 3.

#include "snd_local.h"
#include <cstring>

namespace {
// one 44kHz tick in msec
const float SAMPLES_TO_MSEC = 1000.0f / 44100.0f;

// little endian readers for the RIFF chunks
unsigned int U32( const byte *p ) {
	return static_cast<unsigned int>( p[0] ) |
		   ( static_cast<unsigned int>( p[1] ) << 8 ) |
		   ( static_cast<unsigned int>( p[2] ) << 16 ) |
		   ( static_cast<unsigned int>( p[3] ) << 24 );
}
unsigned int U16( const byte *p ) {
	return static_cast<unsigned int>( p[0] ) | ( static_cast<unsigned int>( p[1] ) << 8 );
}
} // namespace

idSoundSample::idSoundSample() {
	name[0] = 0;
	buffer = 0;
	rate = 44100;
	channels = 1;
	numSamples = 0;
	loaded = false;
	isDefault = false;
	neverPurge = false;
	levelLoadReferenced = false;
}

int idSoundSample::LengthInMsec() const {
	return static_cast<int>( numSamples * SAMPLES_TO_MSEC );
}

bool idSoundSample::Load() {
	if ( loaded ) {
		return true;
	}
	if ( !name[0] ) {
		MakeDefault();
		return true;
	}
	void *data = nullptr;
	const int length = FS_ReadFile( name, &data );
	if ( length <= 0 || length > 64 * 1024 * 1024 ) {
		if ( data ) {
			FS_FreeFile( data );
		}
		// a missing or broken file becomes the placeholder beep, exactly
		// like Doom 3, so the game never plays silence for a bad name
		MakeDefault();
		return true;
	}
	sndWave_t wave;
	const bool decoded = DecodeWave( static_cast<const byte *>( data ), length, wave );
	FS_FreeFile( data );
	if ( !decoded ) {
		Com_Printf( "idSoundSample: unsupported or malformed WAV: %s\n", name );
		MakeDefault();
		return true;
	}
	ALuint alBuffer = 0;
	alGenBuffers( 1, &alBuffer );
	alBufferData( alBuffer, wave.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
				  wave.samples.data(), static_cast<ALsizei>( wave.samples.size() * 2 ), wave.rate );
	if ( alGetError() != AL_NO_ERROR ) {
		alDeleteBuffers( 1, &alBuffer );
		Com_Printf( "idSoundSample: cannot upload %s\n", name );
		MakeDefault();
		return true;
	}
	buffer = alBuffer;
	rate = wave.rate;
	channels = wave.channels;
	numSamples = static_cast<int>( wave.samples.size() );
	loaded = true;
	isDefault = false;
	return true;
}

void idSoundSample::MakeDefault() {
	// the Doom 3 default sound: a short beep so a missing sample is audible
	FreeData();
	const int beepSamples = 2205;  // 50 msec at 44100Hz
	std::vector<int16_t> samples( beepSamples );
	for ( int i = 0; i < beepSamples; ++i ) {
		const float t = static_cast<float>( i );
		const float envelope = 1.0f - t / beepSamples;
		samples[i] = static_cast<int16_t>( 8000.0f * envelope *
										   sinf( t * 0.03f ) );
	}
	ALuint alBuffer = 0;
	alGenBuffers( 1, &alBuffer );
	alBufferData( alBuffer, AL_FORMAT_MONO16, samples.data(),
				  static_cast<ALsizei>( beepSamples * 2 ), 44100 );
	if ( alGetError() != AL_NO_ERROR ) {
		alDeleteBuffers( 1, &alBuffer );
		return;
	}
	buffer = alBuffer;
	rate = 44100;
	channels = 1;
	numSamples = beepSamples;
	loaded = true;
	isDefault = true;
}

void idSoundSample::FreeData() {
	if ( buffer ) {
		alDeleteBuffers( 1, &buffer );
		buffer = 0;
	}
	loaded = false;
	isDefault = false;
}

idSoundCache::idSoundCache() {
	count = 0;
}

idSoundSample *idSoundCache::FindOrCreate( const char *sampleName ) {
	if ( !sampleName || !*sampleName ) {
		return nullptr;
	}
	for ( int i = 0; i < count; ++i ) {
		if ( !Q_stricmp( samples[i].name, sampleName ) ) {
			return &samples[i];
		}
	}
	if ( count >= SOUND_MAX_SAMPLES ) {
		Com_Printf( "Sound cache limit reached: %s\n", sampleName );
		return nullptr;
	}
	idSoundSample &sample = samples[count++];
	Q_strncpyz( sample.name, sampleName, sizeof( sample.name ) );
	sample.Load();
	return &sample;
}

void idSoundCache::BeginLevelLoad() {
	// free everything the previous level loaded and nothing re-referenced
	for ( int i = 0; i < count; ++i ) {
		if ( !samples[i].neverPurge && !samples[i].levelLoadReferenced ) {
			samples[i].FreeData();
		}
		samples[i].levelLoadReferenced = false;
	}
}

void idSoundCache::EndLevelLoad() {
	// the deferred purge point of the original; the purge above already
	// ran at begin, nothing to free here until the full idLib port
}

void idSoundCache::FreeAll() {
	for ( int i = 0; i < count; ++i ) {
		samples[i].FreeData();
	}
	count = 0;
}

int idSoundCache::NumLoaded() const {
	int loadedCount = 0;
	for ( int i = 0; i < count; ++i ) {
		if ( samples[i].loaded ) {
			++loadedCount;
		}
	}
	return loadedCount;
}


// ---------------------------------------------------------------------------
// DecodeWave: the wav decoder of the old sound code, kept as part of the
// port (the idWaveFile stand-in); the sample cache and the music module
// share it
// ---------------------------------------------------------------------------
bool DecodeWave( const byte *data, size_t length, sndWave_t &wave ) {
	if ( !data || length < 12 || memcmp( data, "RIFF", 4 ) || memcmp( data + 8, "WAVE", 4 ) ) {
		return false;
	}
	const size_t end = static_cast<size_t>( U32( data + 4 ) ) + 8;
	if ( end > length || end < 12 ) {
		return false;
	}
	int channels = 0, rate = 0, bits = 0;
	const byte *pcm = nullptr;
	size_t size = 0;
	for ( size_t offset = 12; offset + 8 <= end; ) {
		const byte *chunk = data + offset;
		const size_t bytes = U32( chunk + 4 );
		offset += 8;
		if ( bytes > end - offset ) {
			return false;
		}
		if ( !memcmp( chunk, "fmt ", 4 ) ) {
			if ( bytes < 16 || U16( data + offset ) != 1 ) {
				return false;
			}
			channels = U16( data + offset + 2 );
			rate = U32( data + offset + 4 );
			bits = U16( data + offset + 14 );
			if ( ( channels != 1 && channels != 2 ) || rate < 4000 || rate > 192000 ||
				 ( bits != 8 && bits != 16 ) || U16( data + offset + 12 ) != channels * bits / 8 ) {
				return false;
			}
		} else if ( !memcmp( chunk, "data", 4 ) ) {
			pcm = data + offset;
			size = bytes;
		}
		offset += bytes;
		// several Doom exporters omit the final odd-byte RIFF pad
		if ( ( bytes & 1 ) && offset < end ) {
			++offset;
		}
	}
	if ( !pcm || !size || !channels || size % ( channels * bits / 8 ) || size > 64 * 1024 * 1024 ) {
		return false;
	}
	sndWave_t decoded;
	decoded.rate = rate;
	decoded.channels = channels;
	decoded.samples.resize( size / ( bits / 8 ) );
	for ( size_t i = 0; i < decoded.samples.size(); ++i ) {
		decoded.samples[i] = bits == 8
			? static_cast<int16_t>( ( static_cast<int>( pcm[i] ) - 128 ) * 256 )
			: static_cast<int16_t>( U16( pcm + i * 2 ) );
	}
	wave = std::move( decoded );
	return true;
}
