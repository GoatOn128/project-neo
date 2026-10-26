// SPDX-License-Identifier: GPL-3.0-or-later
//
// The sound world of the id Tech 4 sound port: snd_world.cpp of the Doom 3
// (2004) system. One world holds the emitters and the listener and
// spatializes every active channel once per frame on the main thread, the
// same split the BFG engine uses (game state here, consuming in the
// hardware). Portal occlusion (pVS areas, the 2004 path through the render
// world) is not wired yet; it needs the renderer hooks and arrives with the
// idLib merge.

#include "snd_local.h"

idSoundWorldLocal::idSoundWorldLocal() {
	VectorClear( listenerOrigin );
	for ( int i = 0; i < 3; ++i ) {
		VectorClear( listenerAxis[i] );
	}
	// identity listener axis so a missing PlaceListener does not break
	listenerAxis[0][0] = listenerAxis[1][1] = listenerAxis[2][2] = 1.0f;
	listenerId = 0;
	gameTime = 0;
	paused = false;
	directEmitter.Init( -1 );
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		emitters[i].active = false;
		emitters[i].index = i + 1;
	}
	for ( int i = 0; i < SOUND_MAX_CLASSES; ++i ) {
		classFadeTo[i] = 1.0f;
		classFadeStart[i] = 1.0f;
		classFadeStartTime[i] = 0;
		classFadeDuration[i] = 0;
	}
}

void idSoundWorldLocal::ClearAllSoundEmitters() {
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		if ( emitters[i].active ) {
			emitters[i].StopSound( SCHANNEL_ANY );
			emitters[i].active = false;
		}
	}
	directEmitter.StopSound( SCHANNEL_ANY );
}

void idSoundWorldLocal::StopAllSounds() {
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		if ( emitters[i].active ) {
			emitters[i].StopSound( SCHANNEL_ANY );
		}
	}
	directEmitter.StopSound( SCHANNEL_ANY );
}

idSoundEmitter *idSoundWorldLocal::AllocSoundEmitter() {
	// emitters are pooled, never deleted, the 2004 rule; index 0 stays
	// invalid so save games can spot a hole
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		if ( !emitters[i].active ) {
			emitters[i].Init( i + 1 );
			return &emitters[i];
		}
	}
	Com_Printf( "idSoundWorld: out of emitters\n" );
	return nullptr;
}

idSoundEmitter *idSoundWorldLocal::EmitterForIndex( int index ) {
	if ( index <= 0 || index > SOUND_MAX_EMITTERS ) {
		return nullptr;
	}
	idSoundEmitterLocal &emitter = emitters[index - 1];
	return emitter.active ? &emitter : nullptr;
}

void idSoundWorldLocal::PlaceListener( const vec3_t origin, const vec3_t axis[3],
										int newListenerId, int newGameTime ) {
	VectorCopy( origin, listenerOrigin );
	for ( int i = 0; i < 3; ++i ) {
		VectorCopy( axis[i], listenerAxis[i] );
	}
	listenerId = newListenerId;
	gameTime = newGameTime;
}

void idSoundWorldLocal::FadeSoundClasses( int soundClass, float to, float over ) {
	if ( soundClass < 0 || soundClass >= SOUND_MAX_CLASSES ) {
		return;
	}
	classFadeTo[soundClass] = max( 0.0f, min( 1.0f, to ) );
	classFadeStart[soundClass] = 1.0f;
	classFadeStartTime[soundClass] = Sys_Milliseconds();
	classFadeDuration[soundClass] = static_cast<int>( max( 0.0f, over ) * 1000.0f );
}

void idSoundWorldLocal::PlayShaderDirectly( const char *name, int channel ) {
	// the 2004 music and dialog path; the engine background music still
	// runs through its own synthesizers, this is the shader side of it
	if ( !name || !*name ) {
		return;
	}
	idSoundShader *shader = soundSystemLocalPtr->GetShader( soundSystemLocalPtr->RegisterShader( name ) );
	if ( !shader ) {
		return;
	}
	if ( channel < 0 ) {
		channel = 0;
	}
	directEmitter.StartSound( shader, channel, 0, 0, true, nullptr );
}

void idSoundWorldLocal::Pause() {
	paused = true;
	// the BFG engine pauses the voices, the world state survives so
	// unpause resumes exactly where it left off
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		if ( emitters[i].active ) {
			for ( int j = 0; j < SOUND_MAX_CHANNELS; ++j ) {
				idSoundChannel &ch = emitters[i].channels[j];
				if ( ch.IsActive() && ch.voice ) {
					ch.voice->Pause();
				}
			}
		}
	}
}

void idSoundWorldLocal::UnPause() {
	paused = false;
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		if ( emitters[i].active ) {
			for ( int j = 0; j < SOUND_MAX_CHANNELS; ++j ) {
				idSoundChannel &ch = emitters[i].channels[j];
				if ( ch.IsActive() && ch.voice ) {
					ch.voice->UnPause();
				}
			}
		}
	}
}

void idSoundWorldLocal::ClearLoopingMarks() {
	// the frame boundary of the engine looping sound protocol: every
	// looping channel loses its mark and has to be re-added by the game
	directEmitter.channels[SOUND_LOOP_CHANNEL].marked = false;
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		if ( emitters[i].active ) {
			emitters[i].channels[SOUND_LOOP_CHANNEL].marked = false;
		}
	}
}

void idSoundWorldLocal::ReapLoopingSounds() {
	// stop the looping sounds the game did not re-add this frame, the old
	// voice.seen bookkeeping moved into the channels
	directEmitter.channels[SOUND_LOOP_CHANNEL].marked = false;
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		idSoundEmitterLocal &emitter = emitters[i];
		if ( !emitter.active ) {
			continue;
		}
		idSoundChannel &ch = emitter.channels[SOUND_LOOP_CHANNEL];
		if ( ch.IsActive() && ch.looping && !ch.marked ) {
			if ( ch.voice ) {
				soundSystemLocalPtr->hardware.FreeVoice( ch.voice );
				ch.voice = nullptr;
			}
			ch.playing = false;
		}
	}
}

idSoundVoice *idSoundWorldLocal::AllocateVoiceOrSteal() {
	idSoundHardware &hardware = soundSystemLocalPtr->hardware;
	idSoundVoice *voice = hardware.AllocateVoice();
	if ( voice ) {
		return voice;
	}
	// the pool is exhausted: stop the least audible playing channel and
	// reuse its voice, so a busy scene keeps its loudest sounds instead of
	// silently dropping every new one
	idSoundChannel *best = nullptr;
	float bestGain = 1.01f;
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		if ( !emitters[i].active ) {
			continue;
		}
		for ( int j = 0; j < SOUND_MAX_CHANNELS; ++j ) {
			idSoundChannel &ch = emitters[i].channels[j];
			if ( ch.IsActive() && ch.voice && ch.currentGain < bestGain ) {
				best = &ch;
				bestGain = ch.currentGain;
			}
		}
	}
	for ( int j = 0; j < SOUND_MAX_CHANNELS; ++j ) {
		idSoundChannel &ch = directEmitter.channels[j];
		if ( ch.IsActive() && ch.voice && ch.currentGain < bestGain ) {
			best = &ch;
			bestGain = ch.currentGain;
		}
	}
	if ( !best ) {
		return nullptr;
	}
	voice = best->voice;
	best->voice = nullptr;
	best->playing = false;
	hardware.FreeVoice( voice );
	return voice;
}

void idSoundWorldLocal::Update( int time, float masterVolume ) {
	// the sound class fades (cinematic ducking), the 2004 FadeSoundClasses
	for ( int i = 0; i < SOUND_MAX_CLASSES; ++i ) {
		if ( classFadeDuration[i] > 0 ) {
			const float elapsed = static_cast<float>( time - classFadeStartTime[i] );
			if ( elapsed >= classFadeDuration[i] ) {
				classFadeDuration[i] = 0;
			}
		}
	}
	directEmitter.Update( this, time, masterVolume );
	for ( int i = 0; i < SOUND_MAX_EMITTERS; ++i ) {
		if ( emitters[i].active ) {
			emitters[i].Update( this, time, masterVolume );
		}
	}
}
