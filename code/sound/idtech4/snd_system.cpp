// SPDX-License-Identifier: GPL-3.0-or-later
//
// The sound system of the id Tech 4 sound port: snd_system.cpp of the Doom 3
// (2004) system, with the background work of the BFG edition audio engine
// mixed in: a dedicated sound pump thread feeds the streaming sources (the
// wav file streams and the music synthesizers) while the main thread runs
// the 60Hz async update that spatializes the game voices, exactly the split
// the BFG engine has between its main thread voice updates and the audio
// hardware consuming buffers.
//
// TEMPORARY ENGINE ADAPTERS: cvars, commands, printing and timing go
// through the engine C API (Cvar_Get, Cmd_AddCommand, Com_Printf,
// Sys_Milliseconds). They switch to idCVar, idCmdArgs and idCommon when
// idLib is merged into the whole engine (TODO task 5), and the C shim layer
// (code/sound/snd_cshim.cpp) is deleted with it.

#include "snd_local.h"

// the single system instance, like the original engine
static idSoundSystemLocal soundSystemLocalImpl;
idSoundSystemLocal *soundSystemLocalPtr = &soundSystemLocalImpl;
idSoundSystem *soundSystem = &soundSystemLocalImpl;

idSoundSystemLocal::idSoundSystemLocal() {
	pumpThread = nullptr;
	pumpRunning = false;
	editorPaused = false;
	muted = false;
	pumpStreamGain = 0.0f;
	pumpMusicGain = 0.0f;
	globalGain = 1.0f;
	fadeStartGain = 1.0f;
	fadeTarget = 1.0f;
	fadeStart = 0;
	fadeEnd = 0;
	worldCount = 0;
	currentWorld = nullptr;
	shaderCount = 0;
}

// ---------------------------------------------------------------------------
// the sound pump thread: the BFG audio engine's background half. It owns
// the streaming sources (wav file streams, music), queues their buffers
// with the BFG MAX_QUEUED_BUFFERS depth and never touches the filesystem:
// loop file reads are posted to the main thread instead.
// ---------------------------------------------------------------------------
static int SDLCALL SoundPumpThread( void *data ) {
	idSoundSystemLocal *system = static_cast<idSoundSystemLocal *>( data );
	while ( system->pumpRunning.load() ) {
		if ( !system->editorPaused.load() ) {
			const float streamGain = system->pumpStreamGain.load();
			const float musicGain = system->pumpMusicGain.load();
			{
				// the stream slots and the entity positions are shared with
				// the main thread, the music carries its own lock
				std::lock_guard<std::mutex> guard( system->pumpMutex );
				system->hardware.UpdateStreams( streamGain );
			}
			doomaudio::MusicUpdatePump( musicGain );
		}
		SDL_Delay( 15 );  // ~60Hz, the async update rate of the 2004 system
	}
	return 0;
}

void idSoundSystemLocal::Init() {
	if ( hardware.IsReady() ) {
		return;
	}
	// the engine cvars, read every frame until idCVar exists
	Cvar_Get( "s_volume", "0.8", CVAR_ARCHIVE );
	Cvar_Get( "s_musicvolume", "0.5", CVAR_ARCHIVE );
	Cvar_Get( "s_muteWhenUnfocused", "0", CVAR_ARCHIVE );
	if ( !Cvar_Get( "s_initsound", "1", 0 )->integer ) {
		return;
	}
	if ( !hardware.Init() ) {
		return;
	}
	// the world the game plays in
	if ( !currentWorld ) {
		currentWorld = &worlds[worldCount++];
	}
	doomaudio::MusicInit();
	pumpRunning = true;
	pumpThread = SDL_CreateThread( SoundPumpThread, "idSoundPump", this );
	if ( !pumpThread ) {
		Com_Printf( "idSoundSystem: cannot start the pump thread, streaming stays "
					"on the main thread\n" );
	}
	Cvar_Set( "s_backend", "id Tech 4 (Doom 3 port, OpenAL Soft, BFG voice pump)" );
	Cmd_AddCommand( "play", PlaySoundCommand );
	Cmd_AddCommand( "music", MusicCommand );
	Cmd_AddCommand( "s_info", InfoCommand );
	Cmd_AddCommand( "stopsound", StopSoundCommand );
	Cmd_AddCommand( "s_musicinfo", doomaudio::MusicInfo );
	PrintInfo();
}

void idSoundSystemLocal::Shutdown() {
	if ( !hardware.IsReady() ) {
		return;
	}
	// stop the pump first so nothing feeds the device while it closes
	if ( pumpThread ) {
		pumpRunning = false;
		int status = 0;
		SDL_WaitThread( pumpThread, &status );
		pumpThread = nullptr;
	}
	StopEverything( true, true );
	doomaudio::MusicShutdown();
	for ( int i = 0; i < worldCount; ++i ) {
		worlds[i].ClearAllSoundEmitters();
	}
	currentWorld = nullptr;
	worldCount = 0;
	sampleCache.FreeAll();
	hardware.Shutdown();
	for ( const char *command : {"play", "music", "s_info", "stopsound", "s_musicinfo"} ) {
		Cmd_RemoveCommand( command );
	}
}

void idSoundSystemLocal::StopEverything( bool streams, bool music ) {
	if ( currentWorld ) {
		currentWorld->StopAllSounds();
	}
	if ( streams ) {
		hardware.ClearRawStreams();
		hardware.StopAllStreams();
	}
	if ( music ) {
		doomaudio::MusicStop();
	}
}

void idSoundSystemLocal::SetMute( bool mute ) {
	muted = mute;
}

idSoundWorld *idSoundSystemLocal::AllocSoundWorld() {
	if ( worldCount >= 2 ) {
		return nullptr;
	}
	idSoundWorldLocal *world = &worlds[worldCount++];
	return world;
}

void idSoundSystemLocal::SetPlayingSoundWorld( idSoundWorld *soundWorld ) {
	currentWorld = static_cast<idSoundWorldLocal *>( soundWorld );
}

idSoundWorld *idSoundSystemLocal::GetPlayingSoundWorld() {
	return currentWorld;
}

void idSoundSystemLocal::BeginLevelLoad() {
	// every sample not re-referenced this level is freed, the 2004 rule
	sampleCache.BeginLevelLoad();
	if ( currentWorld ) {
		currentWorld->ClearAllSoundEmitters();
	}
}

void idSoundSystemLocal::EndLevelLoad() {
	sampleCache.EndLevelLoad();
}

int idSoundSystemLocal::AsyncUpdate( int time ) {
	( void )time;
	if ( !hardware.IsReady() || !currentWorld ) {
		return 0;
	}
	// the global fade of the old S_FadeAllSounds
	if ( fadeEnd > fadeStart ) {
		const float fraction = min( 1.0f, max( 0.0f,
			static_cast<float>( Sys_Milliseconds() - fadeStart ) / ( fadeEnd - fadeStart ) ) );
		globalGain = fadeStartGain + ( fadeTarget - fadeStartGain ) * fraction;
	} else {
		globalGain = fadeTarget;
	}
	cvar_t *volume = Cvar_Get( "s_volume", "0.8", 0 );
	cvar_t *musicVolume = Cvar_Get( "s_musicvolume", "0.5", 0 );
	cvar_t *mutedCvar = Cvar_Get( "s_muteWhenUnfocused", "0", 0 );
	const float masterGain = max( 0.0f, globalGain ) *
							 ( ( mutedCvar->integer && !g_sv.activeApp ) ? 0.0f : 1.0f );
	// hand the stream and music gains to the pump thread
	pumpStreamGain = max( 0.0f, volume->value );
	pumpMusicGain = max( 0.0f, musicVolume->value );
	// the listener, the engine Y-up to OpenAL Z-up mapping
	const vec3_t &origin = currentWorld->ListenerOrigin();
	const vec3_t *axis = currentWorld->ListenerAxis();
	hardware.ApplyListener( origin, axis );
	alListenerf( AL_GAIN, max( 0.0f, masterGain ) );
	// the game voices update on the main thread like the BFG voices do
	currentWorld->Update( Sys_Milliseconds(), max( 0.0f, volume->value ) );
	currentWorld->ReapLoopingSounds();
	// main thread file service: the loop reloads the pump thread asked for
	{
		std::lock_guard<std::mutex> guard( pumpMutex );
		hardware.ServiceStreamLoads();
	}
	if ( doomaudio::MusicLoadPending() ) {
		doomaudio::MusicLoadQueued( pumpMusicGain.load() );
	}
	return time;
}

int idSoundSystemLocal::IsEAXAvailable() {
	// the snd_efxfile reverb/EAX port comes after the merger of the rest
	return 0;
}

void idSoundSystemLocal::SetEditorPaused( bool paused ) {
	const bool requested = paused != false;
	if ( requested == editorPaused.load() ) {
		return;
	}
	const int now = Sys_Milliseconds();
	editorPaused = requested;
	if ( requested ) {
		if ( fadeEnd > fadeStart ) {
			// the fade clock stops with the device, shift the window
			editorPauseStarted = now;
		}
		hardware.PauseDevice();
	} else {
		if ( fadeEnd > fadeStart ) {
			fadeStart += now - editorPauseStarted;
			fadeEnd += now - editorPauseStarted;
		}
		hardware.ResumeDevice();
	}
}

void idSoundSystemLocal::FadeAll( float target, int time ) {
	fadeStartGain = globalGain;
	fadeTarget = max( 0.0f, min( 1.0f, target ) );
	fadeStart = Sys_Milliseconds();
	fadeEnd = fadeStart + max( 0, time );
	if ( !time ) {
		globalGain = fadeTarget;
	}
}

int idSoundSystemLocal::RegisterShader( const char *name ) {
	if ( !hardware.IsReady() || !name || !*name || strlen( name ) >= MAX_QPATH ) {
		return 0;
	}
	for ( int i = 0; i < shaderCount; ++i ) {
		if ( !Q_stricmp( shaders[i].GetName(), name ) ) {
			return i + 1;
		}
	}
	if ( shaderCount >= SOUND_MAX_SHADERS ) {
		Com_Printf( "Sound shader limit reached: %s\n", name );
		return 0;
	}
	idSoundShader &shader = shaders[shaderCount++];
	shader.SetName( name );
	shader.SetDefaultDefinition( name );
	// preload the sample at registration, the engine level load style;
	// the 2004 on demand path (allowSlow) reloads it after a purge
	idSoundSample *sample = shader.GetSample( 0 );
	if ( sample ) {
		sample->MarkLevelLoadReferenced();
	}
	return shaderCount;
}

idSoundShader *idSoundSystemLocal::GetShader( int handle ) {
	if ( handle <= 0 || handle > shaderCount ) {
		return nullptr;
	}
	return &shaders[handle - 1];
}

void idSoundSystemLocal::PrintInfo() {
	if ( !hardware.IsReady() ) {
		Com_Printf( "Doom audio: no device\n" );
		return;
	}
	Com_Printf( "Doom audio: id Tech 4 sound system (Doom 3 2004 port, BFG voice "
				"pump), %d voices, %d samples, %d shaders\n",
				hardware.NumFreeVoices(), sampleCache.NumLoaded(), shaderCount );
}

// ---------------------------------------------------------------------------
// console commands, the engine C API until idCmdArgs exists
// ---------------------------------------------------------------------------
void PlaySoundCommand() {
	for ( int i = 1; i < Cmd_Argc(); ++i ) {
		S_StartLocalSound( S_RegisterSound( Cmd_Argv( i ), false ), CHAN_LOCAL_SOUND );
	}
}

void MusicCommand() {
	S_StartBackgroundTrack( Cmd_Argv( 1 ), Cmd_Argc() > 2 ? Cmd_Argv( 2 ) : Cmd_Argv( 1 ), 0 );
}

void InfoCommand() {
	soundSystemLocalPtr->PrintInfo();
	Com_Printf( "Doom audio state: editorPaused=%d fade=%d\n",
				soundSystemLocalPtr->editorPaused.load() ? 1 : 0,
				soundSystemLocalPtr->fadeEnd > soundSystemLocalPtr->fadeStart ? 1 : 0 );
}

void StopSoundCommand() {
	S_StopAllSounds();
}
