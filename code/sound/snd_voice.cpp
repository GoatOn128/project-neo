// SPDX-License-Identifier: GPL-3.0-or-later
//
// The voice layer of the id Tech 4 sound port: the BFG edition idSoundVoice
// (references/DOOM-3-BFG/neo/sound/SoundVoice.h), backed by an OpenAL
// source. The BFG engine drives its voices from the main thread and lets the
// audio hardware consume on its own, which is the mixing model here too:
// OpenAL consumes, this class only submits parameters.

#include "snd_local.h"

idSoundVoice::idSoundVoice() {
	source = 0;
	leadinSample = nullptr;
	loopingSample = nullptr;
	looping = false;
	paused = false;
	assigned = false;
	relative = false;
	switchedToLoop = false;
	gain = 1.0f;
	pitch = 1.0f;
	minDistance = 160.0f;
	maxDistance = 1250.0f;
	VectorClear( position );
	emitterIndex = -1;
	channelIndex = -1;
}

void idSoundVoice::Create( ALuint newSource ) {
	source = newSource;
	assigned = false;
	alSourcef( source, AL_GAIN, 1.0f );
	alSourcef( source, AL_PITCH, 1.0f );
}

void idSoundVoice::Destroy() {
	if ( source ) {
		alSourceStop( source );
		alDeleteSources( 1, &source );
		source = 0;
	}
	assigned = false;
}

void idSoundVoice::SetOwner( int emitter, int channel ) {
	emitterIndex = emitter;
	channelIndex = channel;
	assigned = true;
}

void idSoundVoice::SetDistance( float newMinDistance, float newMaxDistance ) {
	minDistance = newMinDistance > 0.0f ? newMinDistance : 160.0f;
	maxDistance = newMaxDistance > 0.0f ? newMaxDistance : 1250.0f;
}

void idSoundVoice::SetPosition( const vec3_t p, bool isRelative ) {
	VectorCopy( p, position );
	relative = isRelative;
}

bool idSoundVoice::CompatibleFormat( const idSoundSample *sample ) const {
	// the BFG voice reuse check: a voice keeps its hardware format, only a
	// matching sample can reuse it; OpenAL sources are format agnostic, so
	// this is the bookkeeping half only
	return sample != nullptr;
}

void idSoundVoice::ApplyParams() {
	alSourcei( source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE );
	alSourcef( source, AL_GAIN, max( 0.0f, gain ) );
	alSourcef( source, AL_PITCH, max( 0.05f, pitch ) );
	alSourcef( source, AL_REFERENCE_DISTANCE, minDistance );
	alSourcef( source, AL_MAX_DISTANCE, maxDistance );
	alSourcef( source, AL_ROLLOFF_FACTOR, relative ? 0.0f : 1.0f );
	// the engine is Y up, OpenAL is Z up, the same mapping the old code used
	alSource3f( source, AL_POSITION, position[0], position[2], -position[1] );
}

bool idSoundVoice::Start( idSoundSample *leadin, idSoundSample *loop, bool isLooping ) {
	if ( !source || !leadin || !leadin->IsLoaded() ) {
		return false;
	}
	Stop();
	leadinSample = leadin;
	loopingSample = loop;
	looping = isLooping;
	switchedToLoop = ( leadinSample == loopingSample );
	// a looping shader that plays one sample loops the buffer directly; a
	// two sample shader plays the leadin first and switches in Update(),
	// the 2004 leadin/loop model the BFG voices kept
	alSourcei( source, AL_LOOPING, ( looping && leadinSample == loopingSample ) ? AL_TRUE : AL_FALSE );
	alSourcei( source, AL_BUFFER, leadinSample->GetBuffer() );
	ApplyParams();
	alSourcePlay( source );
	paused = false;
	return true;
}

void idSoundVoice::Update() {
	if ( !source || !assigned ) {
		return;
	}
	// the leadin -> loop switch of a two sample shader
	if ( !switchedToLoop && loopingSample && loopingSample != leadinSample && !IsPlaying() ) {
		switchedToLoop = true;
		alSourcei( source, AL_BUFFER, loopingSample->GetBuffer() );
		alSourcei( source, AL_LOOPING, AL_TRUE );
		alSourcePlay( source );
		return;
	}
	// sends the new position/gain/pitch to the hardware, the BFG
	// idSoundVoice::Update job
	alSourcei( source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE );
	alSourcef( source, AL_GAIN, max( 0.0f, gain ) );
	alSourcef( source, AL_PITCH, max( 0.05f, pitch ) );
	alSourcef( source, AL_REFERENCE_DISTANCE, minDistance );
	alSourcef( source, AL_MAX_DISTANCE, maxDistance );
	alSourcef( source, AL_ROLLOFF_FACTOR, relative ? 0.0f : 1.0f );
	alSource3f( source, AL_POSITION, position[0], position[2], -position[1] );
}

void idSoundVoice::Stop() {
	if ( !source ) {
		return;
	}
	alSourceStop( source );
	alSourcei( source, AL_BUFFER, 0 );
	alSourcei( source, AL_LOOPING, AL_FALSE );
	assigned = false;
	emitterIndex = -1;
	channelIndex = -1;
	leadinSample = nullptr;
	loopingSample = nullptr;
	looping = false;
	paused = false;
}

void idSoundVoice::Pause() {
	if ( source && !paused ) {
		alSourcePause( source );
		paused = true;
	}
}

void idSoundVoice::UnPause() {
	if ( source && paused ) {
		alSourcePlay( source );
		paused = false;
	}
}

bool idSoundVoice::IsPlaying() const {
	if ( !source ) {
		return false;
	}
	ALint state = AL_STOPPED;
	alGetSourcei( source, AL_SOURCE_STATE, &state );
	return state == AL_PLAYING;
}
