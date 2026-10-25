// SPDX-License-Identifier: GPL-3.0-or-later
//
// ---------------------------------------------------------------------------
// TEMPORARY C SHIM LAYER -- THIS FILE GOES AWAY
//
// The engine and the game modules still speak the id Tech 3 C API (the S_*
// functions of snd_public.h), but the sound system underneath is the id
// Tech 4 port in idtech4/ (Doom 3 2004 system with the BFG voice pump).
// Every function below is a thin translation between the two.
//
// Once idLib is truly merged into the entire engine (TODO task 5), the
// game code talks to the idSoundSystem / idSoundWorld / idSoundEmitter
// interface directly, the way id Tech 4 does it, and this shim layer is
// deleted. Nothing else in the engine should call S_* from here on.
// ---------------------------------------------------------------------------

#include "../client/client.h"
#include "idtech4/snd_local.h"

// the old engine's cinematic timing globals, kept in the 22050Hz sample
// units the client code expects (cl_cin.cpp reads them)
int s_soundtime, s_paintedtime, s_rawend[32];

namespace {

// one emitter per entity, the entity keyed translation the engine needs;
// the id Tech 4 side keeps no entity table of its own
idSoundEmitter *entityEmitters[MAX_GENTITIES];
idSoundEmitter *ambientEmitter = nullptr;  // entity -1 and out of range sounds

idSoundEmitter *EmitterForEntity( int entityNum ) {
	idSoundWorld *world = soundSystemLocalPtr->World();
	if ( !world ) {
		return nullptr;
	}
	if ( entityNum < 0 || entityNum >= MAX_GENTITIES ) {
		if ( !ambientEmitter ) {
			ambientEmitter = world->AllocSoundEmitter();
		}
		return ambientEmitter;
	}
	if ( !entityEmitters[entityNum] ) {
		entityEmitters[entityNum] = world->AllocSoundEmitter();
	}
	return entityEmitters[entityNum];
}

bool Ready() {
	return soundSystemLocalPtr->hardware.IsReady();
}

soundShaderParms_t LoopParms( int volume ) {
	soundShaderParms_t parms;
	parms.minDistance = 0.0f;      // no override, the shader keeps its value
	parms.maxDistance = 0.0f;
	parms.volume = min( 255, max( 0, volume & 255 ) ) / 255.0f;
	parms.shakes = 0.0f;
	parms.soundShaderFlags = 0;
	parms.soundClass = 0;
	return parms;
}

} // namespace

// TEMPORARY C SHIM: S_Init -> idSoundSystem::Init
void S_Init() {
	soundSystem->Init();
}

// TEMPORARY C SHIM: S_Shutdown -> idSoundSystem::Shutdown
void S_Shutdown() {
	soundSystem->Shutdown();
}

// TEMPORARY C SHIM: the pump thread owns this work now
void S_UpdateThread() {
}

// TEMPORARY C SHIM: S_RegisterSound -> a default sound shader around the wav
sfxHandle_t S_RegisterSound( const char *name, bool compressed ) {
	( void )compressed;
	return static_cast<sfxHandle_t>( soundSystemLocalPtr->RegisterShader( name ) );
}

// TEMPORARY C SHIM: S_StartSound -> emitter->StartSound
void S_StartSound( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx ) {
	idSoundShader *shader = soundSystemLocalPtr->GetShader( sfx );
	if ( !shader ) {
		return;
	}
	idSoundEmitter *emitter = EmitterForEntity( entnum );
	if ( !emitter ) {
		return;
	}
	emitter->StartSound( shader, entchannel, 0, 0, true, origin );
}

// TEMPORARY C SHIM: the engine flags have no id Tech 4 meaning yet
void S_StartSoundEx( vec3_t origin, int entnum, int entchannel, sfxHandle_t sfx, int flags ) {
	( void )flags;
	S_StartSound( origin, entnum, entchannel, sfx );
}

// TEMPORARY C SHIM: a local sound is a global (listener relative) sound
void S_StartLocalSound( sfxHandle_t sfx, int channelNum ) {
	idSoundShader *shader = soundSystemLocalPtr->GetShader( sfx );
	if ( !shader || !Ready() ) {
		return;
	}
	idSoundEmitter *emitter = EmitterForEntity( -1 );
	if ( !emitter ) {
		return;
	}
	emitter->StartSound( shader, channelNum, 0, SSF_GLOBAL, true, nullptr );
}

// TEMPORARY C SHIM: background music goes to the engine synthesizers,
// which the pump thread feeds now
void S_StartBackgroundTrack( const char *intro, const char *loop, int fadeupTime ) {
	( void )fadeupTime;
	if ( Ready() ) {
		doomaudio::MusicStart( intro, loop );
	}
}

// TEMPORARY C SHIM
void S_StopBackgroundTrack() {
	if ( Ready() ) {
		doomaudio::MusicStop();
	}
}

// TEMPORARY C SHIM
void S_QueueBackgroundTrack( const char *loop ) {
	S_StartBackgroundTrack( loop, loop, 0 );
}

// TEMPORARY C SHIM
void S_FadeStreamingSound( float targetvol, int time, int ssNum ) {
	if ( ssNum < 0 ) {
		soundSystemLocalPtr->FadeAll( targetvol, time );
	} else {
		soundSystemLocalPtr->hardware.SetRawStreamGain( ssNum, targetvol );
	}
}

// TEMPORARY C SHIM: the global fade lives in the system now
void S_FadeAllSounds( float targetvol, int time ) {
	soundSystemLocalPtr->FadeAll( targetvol, time );
}

// TEMPORARY C SHIM
void S_StartStreamingSound( const char *intro, const char *loop, int entnum, int channel,
							int attenuation ) {
	if ( !Ready() ) {
		return;
	}
	soundSystemLocalPtr->hardware.StartStream( intro, loop, entnum, channel, attenuation );
}

// TEMPORARY C SHIM
void S_StopStreamingSound( int index ) {
	if ( Ready() ) {
		soundSystemLocalPtr->hardware.StopStream( index );
	}
}

// TEMPORARY C SHIM
void S_StopEntStreamingSound( int entNum ) {
	if ( Ready() ) {
		soundSystemLocalPtr->hardware.StopEntityStreams( entNum );
	}
}

// TEMPORARY C SHIM: raw samples go to the cinematic channels of the
// hardware layer, the pump never touches them
void S_RawSamples( int samples, int rate, int width, int s_channels,
				   const byte *data, float lvol, float rvol, int streamingIndex ) {
	if ( !Ready() ) {
		return;
	}
	if ( soundSystemLocalPtr->hardware.QueueRawSamples( streamingIndex, samples, rate, width,
													   s_channels, data, lvol, rvol ) ) {
		s_rawend[streamingIndex] = max( s_soundtime, s_rawend[streamingIndex] ) +
								   samples * 22050 / rate;
	}
}

// TEMPORARY C SHIM: everything stops at once
void S_StopAllSounds() {
	soundSystemLocalPtr->StopEverything( true, true );
}

// TEMPORARY C SHIM: the frame boundary of the looping sound protocol
void S_ClearLoopingSounds() {
	idSoundWorld *world = soundSystemLocalPtr->World();
	if ( world ) {
		world->ClearLoopingMarks();
	}
}

// TEMPORARY C SHIM: S_ClearSounds -> the selective stop of the system
void S_ClearSounds( bool clearStreaming, bool clearMusic ) {
	soundSystemLocalPtr->StopEverything( clearStreaming, clearMusic );
}

// TEMPORARY C SHIM: S_AddLoopingSound -> the emitter loop channel
void S_AddLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, const int range,
						sfxHandle_t sfxHandle, int volume ) {
	( void )velocity;
	idSoundShader *shader = soundSystemLocalPtr->GetShader( sfxHandle );
	if ( !shader || !Ready() ) {
		return;
	}
	idSoundEmitter *emitter = EmitterForEntity( entityNum );
	if ( !emitter ) {
		return;
	}
	emitter->StartSound( shader, SOUND_LOOP_CHANNEL, 0, SSF_LOOPING, true, origin );
	soundShaderParms_t parms = LoopParms( volume );
	parms.maxDistance = range > 0 ? static_cast<float>( range ) : 1250.0f;
	emitter->ModifySound( SOUND_LOOP_CHANNEL, &parms );
}

// TEMPORARY C SHIM
void S_AddRealLoopingSound( int entityNum, const vec3_t origin, const vec3_t velocity, const int range,
							sfxHandle_t sfxHandle ) {
	S_AddLoopingSound( entityNum, origin, velocity, range, sfxHandle, 255 );
}

// TEMPORARY C SHIM
void S_StopLoopingSound( int entityNum ) {
	idSoundEmitter *emitter = EmitterForEntity( entityNum );
	if ( emitter ) {
		emitter->StopSound( SOUND_LOOP_CHANNEL );
	}
}

// TEMPORARY C SHIM: S_ClearSoundBuffer -> the full stop
void S_ClearSoundBuffer( bool killStreaming ) {
	soundSystemLocalPtr->StopEverything( killStreaming, killStreaming );
}

// TEMPORARY C SHIM: S_Respatialize -> PlaceListener
void S_Respatialize( int entityNum, const vec3_t origin, vec3_t axis[3], int inwater ) {
	( void )inwater;
	idSoundWorld *world = soundSystemLocalPtr->World();
	if ( !world || !Ready() ) {
		return;
	}
	world->PlaceListener( origin, axis, entityNum, Sys_Milliseconds() );
}

// TEMPORARY C SHIM: S_UpdateEntityPosition -> emitter->UpdateEmitter
void S_UpdateEntityPosition( int entityNum, const vec3_t origin ) {
	if ( entityNum < 0 || entityNum >= MAX_GENTITIES ) {
		return;
	}
	idSoundEmitter *emitter = EmitterForEntity( entityNum );
	if ( emitter ) {
		emitter->UpdateEmitter( origin, entityNum, nullptr );
	}
	if ( Ready() ) {
		soundSystemLocalPtr->hardware.UpdateEntityPosition( entityNum, origin );
	}
}

// TEMPORARY C SHIM: S_Update -> idSoundSystem::AsyncUpdate, the 60Hz
// main thread half of the split
void S_Update() {
	soundSystem->AsyncUpdate( Sys_Milliseconds() );
	const int now = Sys_Milliseconds();
	s_soundtime = s_paintedtime = static_cast<int>( static_cast<int64_t>( now ) * 22050 / 1000 );
}

// TEMPORARY C SHIM
void S_SetEditorPaused( bool paused ) {
	soundSystemLocalPtr->SetEditorPaused( paused );
}

// TEMPORARY C SHIM
void S_DisableSounds() {
	S_StopAllSounds();
}

// TEMPORARY C SHIM: the level load purge point
void S_BeginRegistration() {
	soundSystemLocalPtr->BeginLevelLoad();
	soundSystemLocalPtr->FadeAll( 1.0f, 0 );
	// the world pooled fresh emitters, the entity table follows
	for ( int i = 0; i < MAX_GENTITIES; ++i ) {
		entityEmitters[i] = nullptr;
	}
	ambientEmitter = nullptr;
}

// TEMPORARY C SHIM
void S_DisplayFreeMemory() {
	soundSystemLocalPtr->PrintInfo();
}

// TEMPORARY C SHIM: S_GetVoiceAmplitude -> emitter->CurrentAmplitude
int S_GetVoiceAmplitude( int entityNum ) {
	if ( entityNum < 0 || entityNum >= MAX_GENTITIES || !entityEmitters[entityNum] ) {
		return 0;
	}
	return static_cast<int>( entityEmitters[entityNum]->CurrentAmplitude() * 255.0f );
}

// TEMPORARY C SHIM: OpenAL retains the device across focus changes
void SNDDMA_Activate() {
}
