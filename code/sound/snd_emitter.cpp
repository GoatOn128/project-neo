// SPDX-License-Identifier: GPL-3.0-or-later
//
// Sound emitters of the id Tech 4 sound port: snd_emitter.cpp of the Doom 3
// (2004) system. Emitters own their channels, channels own their playing
// voice, the 2004 architecture.

#include "snd_local.h"

idSoundEmitterLocal::idSoundEmitterLocal() {
	active = false;
	VectorClear( origin );
	listenerId = 0;
	parms = soundShaderParms_t();
	index = 0;
	for ( int i = 0; i < SOUND_MAX_CHANNELS; ++i ) {
		channels[i].Reset();
	}
}

void idSoundEmitterLocal::Init( int emitterIndex ) {
	active = true;
	listenerId = 0;
	parms = soundShaderParms_t();
	index = emitterIndex;
	VectorClear( origin );
	for ( int i = 0; i < SOUND_MAX_CHANNELS; ++i ) {
		channels[i].Reset();
	}
}

void idSoundEmitterLocal::Free( bool immediate ) {
	// the world pools emitters, freeing just makes it reusable; a
	// non-immediate free would let running sounds finish in the original,
	// the engine only frees through full stops today
	( void )immediate;
	StopSound( SCHANNEL_ANY );
	active = false;
}

void idSoundEmitterLocal::UpdateEmitter( const vec3_t newOrigin, int newListenerId,
										 const soundShaderParms_t *newParms ) {
	VectorCopy( newOrigin, origin );
	listenerId = newListenerId;
	if ( newParms ) {
		parms = *newParms;
	}
}

idSoundChannel *idSoundEmitterLocal::FindChannel( s_channelType channelNum ) {
	if ( channelNum == SCHANNEL_ANY ) {
		// any free channel, the engine CHAN_AUTO behavior
		for ( int i = 0; i < SOUND_MAX_CHANNELS; ++i ) {
			if ( !channels[i].IsActive() ) {
				return &channels[i];
			}
		}
		return nullptr;
	}
	int idx = channelNum % SOUND_MAX_CHANNELS;
	return &channels[idx];
}

int idSoundEmitterLocal::StartSound( const idSoundShader *shader, s_channelType channelNum,
									  float diversity, int shaderFlags, bool allowSlow,
									  const vec3_t soundOrigin ) {
	if ( !shader ) {
		return 0;
	}
	if ( channelNum < 0 ) {
		channelNum = SCHANNEL_ANY;
	}
	idSoundChannel *ch = FindChannel( channelNum );
	if ( !ch ) {
		// every channel busy, the engine channel space is masked to 16 so
		// this is rare; the old code just dropped the sound too
		return 0;
	}
	// the looping channel reuses a running voice instead of restarting it,
	// the engine adds its looping sounds every frame
	if ( ch->IsActive() && ch->shader == shader && ch->looping ) {
		ch->marked = true;
		if ( soundOrigin ) {
			VectorCopy( soundOrigin, ch->position );
			ch->hasPosition = true;
		}
		return 0;
	}
	ch->Reset();
	ch->shader = shader;
	ch->parms = *shader->GetParms();
	ch->parms.soundShaderFlags |= shaderFlags;
	ch->looping = ( ch->parms.soundShaderFlags & SSF_LOOPING ) != 0;
	ch->relative = ( ch->parms.soundShaderFlags & SSF_GLOBAL ) != 0;
	ch->marked = true;
	ch->startTime = Sys_Milliseconds();
	ch->diversity = static_cast<int>( diversity * shader->GetNumSounds() ) %
					 ( shader->GetNumSounds() > 0 ? shader->GetNumSounds() : 1 );
	// GetSample demand-loads the wav so it cannot be const on the shader;
	// the shader itself stays const on the channel, the 2004 interface shape
	idSoundSample *sample = const_cast<idSoundShader *>( shader )->GetSample( ch->diversity );
	if ( !sample ) {
		return 0;
	}
	if ( !sample->IsLoaded() ) {
		if ( !allowSlow ) {
			return 0;
		}
		sample->Load();
	}
	if ( soundOrigin ) {
		VectorCopy( soundOrigin, ch->position );
		ch->hasPosition = true;
	} else {
		ch->hasPosition = false;
	}
	idSoundHardware &hardware = soundSystemLocalPtr->hardware;
	// the engine semantics: a new sound on the same channel replaces the
	// old one, so reuse the voice the (emitter, channel) pair already has
	idSoundVoice *voice = hardware.VoiceFor( index, static_cast<int>( channelNum ) % SOUND_MAX_CHANNELS );
	if ( !voice ) {
		// the world steals the least audible playing voice when the pool runs
		// out, instead of dropping the new sound
		idSoundWorldLocal *world = soundSystemLocalPtr->World();
		voice = world ? world->AllocateVoiceOrSteal() : hardware.AllocateVoice();
	}
	if ( !voice ) {
		// nothing is playing anywhere, so there is no voice to take either
		return 0;
	}
	voice->SetOwner( index, static_cast<int>( channelNum ) % SOUND_MAX_CHANNELS );
	voice->SetDistance( ch->parms.minDistance, ch->parms.maxDistance );
	voice->SetPosition( ch->hasPosition ? ch->position : origin, ch->relative );
	// this port has one sample per shader so far, leadin and loop are the
	// same; the two sample leadin->loop switch stays wired in the voice
	voice->Start( sample, sample, ch->looping );
	ch->voice = voice;
	ch->playing = true;
	ch->currentGain = ch->parms.volume;
	return sample->LengthInMsec();
}

void idSoundEmitterLocal::ModifySound( s_channelType channelNum, const soundShaderParms_t *newParms ) {
	if ( !newParms ) {
		return;
	}
	for ( int i = 0; i < SOUND_MAX_CHANNELS; ++i ) {
		idSoundChannel &ch = channels[i];
		if ( !ch.IsActive() ) {
			continue;
		}
		if ( channelNum != SCHANNEL_ANY && i != channelNum % SOUND_MAX_CHANNELS ) {
			continue;
		}
		// the original semantics: non-zero fields override
		if ( newParms->minDistance > 0.0f ) {
			ch.parms.minDistance = newParms->minDistance;
		}
		if ( newParms->maxDistance > 0.0f ) {
			ch.parms.maxDistance = newParms->maxDistance;
		}
		if ( newParms->volume >= 0.0f ) {
			ch.parms.volume = newParms->volume;
		}
		ch.parms.soundShaderFlags |= newParms->soundShaderFlags;
		ch.parms.soundClass = newParms->soundClass;
	}
}

void idSoundEmitterLocal::StopSound( s_channelType channelNum ) {
	for ( int i = 0; i < SOUND_MAX_CHANNELS; ++i ) {
		if ( channelNum != SCHANNEL_ANY && i != channelNum % SOUND_MAX_CHANNELS ) {
			continue;
		}
		idSoundChannel &ch = channels[i];
		if ( !ch.IsActive() ) {
			continue;
		}
		if ( ch.voice ) {
			soundSystemLocalPtr->hardware.FreeVoice( ch.voice );
			ch.voice = nullptr;
		}
		ch.playing = false;
	}
}

void idSoundEmitterLocal::FadeSound( s_channelType channelNum, float to, float over ) {
	for ( int i = 0; i < SOUND_MAX_CHANNELS; ++i ) {
		idSoundChannel &ch = channels[i];
		if ( !ch.IsActive() ) {
			continue;
		}
		if ( channelNum != SCHANNEL_ANY && i != channelNum % SOUND_MAX_CHANNELS ) {
			continue;
		}
		ch.fadeTargetGain = max( 0.0f, min( 1.0f, to ) );
		ch.fadeStartGain = ch.currentGain;
		ch.fadeStartTime = static_cast<float>( Sys_Milliseconds() );
		ch.fadeDuration = max( 0.0f, over ) * 1000.0f;
	}
}

bool idSoundEmitterLocal::CurrentlyPlaying() const {
	for ( int i = 0; i < SOUND_MAX_CHANNELS; ++i ) {
		if ( channels[i].IsActive() ) {
			return true;
		}
	}
	return false;
}

float idSoundEmitterLocal::CurrentAmplitude() const {
	// the conservative slop of the original: report the loudest channel
	// gain of anything this emitter plays
	float amplitude = 0.0f;
	for ( int i = 0; i < SOUND_MAX_CHANNELS; ++i ) {
		const idSoundChannel &ch = channels[i];
		if ( !ch.IsActive() ) {
			continue;
		}
		if ( ch.parms.soundShaderFlags & SSF_NO_FLICKER ) {
			return 1.0f;
		}
		if ( ch.currentGain > amplitude ) {
			amplitude = ch.currentGain;
		}
	}
	return min( 1.0f, amplitude );
}

void idSoundEmitterLocal::MarkChannel( s_channelType channelNum ) {
	if ( channelNum == SCHANNEL_ANY ) {
		return;
	}
	idSoundChannel *ch = &channels[channelNum % SOUND_MAX_CHANNELS];
	if ( ch->IsActive() ) {
		ch->marked = true;
	}
}

idSoundChannel::idSoundChannel() {
	Reset();
}

void idSoundChannel::Reset() {
	shader = nullptr;
	parms = soundShaderParms_t();
	startTime = 0;
	looping = false;
	playing = false;
	relative = false;
	marked = false;
	VectorClear( position );
	hasPosition = false;
	voice = nullptr;
	diversity = 0;
	currentGain = 0.0f;
	fadeTargetGain = -1.0f;
	fadeStartGain = 1.0f;
	fadeStartTime = 0.0f;
	fadeDuration = 0.0f;
}

void idSoundEmitterLocal::Update( const idSoundWorldLocal *world, int time, float masterVolume ) {
	( void )time;
	const vec3_t &listener = world->ListenerOrigin();
	for ( int i = 0; i < SOUND_MAX_CHANNELS; ++i ) {
		idSoundChannel &ch = channels[i];
		if ( !ch.IsActive() ) {
			continue;
		}
		if ( !ch.voice ) {
			ch.playing = false;
			continue;
		}
		// the 2004 distance model: full inside minDistance, linear to
		// zero at maxDistance (the model the old OpenAL code had through
		// AL_LINEAR_DISTANCE_CLAMPED)
		const vec3_t &position = ch.hasPosition ? ch.position : origin;
		float gain = ch.parms.volume;
		if ( !ch.relative ) {
			const float dx = position[0] - listener[0];
			const float dy = position[1] - listener[1];
			const float dz = position[2] - listener[2];
			const float distance = sqrtf( dx * dx + dy * dy + dz * dz );
			if ( distance <= ch.parms.minDistance ) {
				// full volume
			} else if ( distance >= ch.parms.maxDistance ) {
				gain = 0.0f;
			} else {
				gain *= ( ch.parms.maxDistance - distance ) /
						( ch.parms.maxDistance - ch.parms.minDistance );
			}
		}
		// the per channel fade (FadeSound)
		if ( ch.fadeTargetGain >= 0.0f && ch.fadeDuration > 0.0f ) {
			const float elapsed = static_cast<float>( Sys_Milliseconds() ) - ch.fadeStartTime;
			if ( elapsed >= ch.fadeDuration ) {
				gain *= ch.fadeTargetGain;
			} else {
				const float fraction = elapsed / ch.fadeDuration;
				gain *= ch.fadeStartGain + ( ch.fadeTargetGain - ch.fadeStartGain ) * fraction;
			}
		}
		ch.currentGain = gain;
		ch.voice->SetGain( max( 0.0f, gain * masterVolume ) );
		ch.voice->SetPosition( position, ch.relative );
		ch.voice->Update();
		if ( !ch.looping && !ch.voice->IsPlaying() ) {
			soundSystemLocalPtr->hardware.FreeVoice( ch.voice );
			ch.voice = nullptr;
			ch.playing = false;
		}
	}
}
