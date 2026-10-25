// SPDX-License-Identifier: GPL-3.0-or-later
//
// The music module of the id Tech 4 sound port. FluidSynth MIDI scores and
// wav PCM tracks play on one OpenAL source; the sound pump thread renders
// the chunks, and the loop file reads it cannot do are posted to the main
// thread, the same split the wav file streams use.

#include "snd_local.h"
#include "snd_fluidsynth.h"
#include <cstring>

namespace {

fluid_settings_t *settings;
fluid_synth_t *synth;
fluid_player_t *player;
ALuint source;
bool synthesizer;
char introName[MAX_QPATH], loopName[MAX_QPATH];
std::vector<byte> midi;
sndWave_t wave;
size_t wavePosition;
int loops, chunks;
// The music state is shared between the main thread (starts, stops, file
// loads) and the sound pump thread (rendering), so every entry point
// locks this recursive mutex (recursive because MusicLoadQueued re-enters
// it while it runs a full update on the main thread).
std::recursive_mutex musicMutex;
bool loadPending;

bool Load( const char *name ) {
	if ( player ) {
		fluidsynth.delete_player( player );
		player = nullptr;
	}
	wave = sndWave_t();
	wavePosition = 0;
	midi.clear();
	if ( !name || !*name ) {
		return false;
	}
	void *bytes = nullptr;
	const int length = FS_ReadFile( name, &bytes );
	if ( length <= 0 || length > 32 * 1024 * 1024 ) {
		if ( bytes ) {
			FS_FreeFile( bytes );
		}
		Com_Printf( "Music: cannot read %s\n", name );
		return false;
	}
	const byte *data = static_cast<const byte *>( bytes );
	if ( length >= 4 && !memcmp( data, "MThd", 4 ) ) {
		if ( !synthesizer ) {
			FS_FreeFile( bytes );
			Com_Printf( "Music: MIDI track needs FluidSynth, which is unavailable: %s\n", name );
			return false;
		}
		midi.assign( data, data + length );
		FS_FreeFile( bytes );
		player = fluidsynth.new_player( synth );
		if ( !player || fluidsynth.player_add_mem( player, midi.data(), midi.size() ) != FLUID_OK ) {
			if ( player ) {
				fluidsynth.delete_player( player );
				player = nullptr;
			}
			Com_Printf( "FluidSynth: cannot decode %s\n", name );
			return false;
		}
		fluidsynth.player_play( player );
		Com_Printf( "FluidSynth music: %s (%d ticks)\n", name,
					fluidsynth.player_get_total_ticks( player ) );
		return true;
	}
	const bool decoded = DecodeWave( data, length, wave );
	FS_FreeFile( bytes );
	if ( !decoded ) {
		Com_Printf( "Music: unsupported or malformed track %s\n", name );
	}
	return decoded;
}

void MusicUpdateInternal( float volume, bool allowLoad ) {
	std::lock_guard<std::recursive_mutex> guard( musicMutex );
	if ( !source ) {
		return;
	}
	alSourcef( source, AL_GAIN, volume );
	ALint queued = 0, processed = 0;
	alGetSourcei( source, AL_BUFFERS_QUEUED, &queued );
	alGetSourcei( source, AL_BUFFERS_PROCESSED, &processed );
	int emptyLoops = 0;
	for ( int count = queued - processed;
		  count < 4 && ( player || !wave.samples.empty() ); ++count ) {
		int16_t pcm[8192];
		size_t size = 0;
		int rate = 44100, channels = 2;
		if ( player ) {
			if ( fluidsynth.player_get_status( player ) != FLUID_PLAYER_PLAYING ) {
				size = 0;
			} else {
				// render interleaved stereo straight into the buffer
				fluidsynth.write_s16( synth, sizeof( pcm ) / ( sizeof( pcm[0] ) * 2 ), pcm, 0, 2,
									  pcm, 1, 2 );
				size = sizeof( pcm );
			}
		} else {
			rate = wave.rate;
			channels = wave.channels;
			const size_t samples = min( size_t( 8192 ), wave.samples.size() - wavePosition );
			memcpy( pcm, wave.samples.data() + wavePosition, samples * 2 );
			wavePosition += samples;
			size = samples * 2;
		}
		if ( !size ) {
			if ( !loopName[0] ) {
				break;
			}
			// the pump thread never reads files: it posts the loop reload
			// to the main thread and keeps the last chunk queued
			if ( !allowLoad ) {
				loadPending = true;
				break;
			}
			char next[MAX_QPATH];
			Q_strncpyz( next, loopName, sizeof( next ) );
			if ( !Load( next ) ) {
				loopName[0] = 0;
				break;
			}
			++loops;
			if ( ++emptyLoops > 1 ) {
				Com_Printf( "Music: empty loop rejected: %s\n", next );
				MusicStop();
				break;
			}
			--count;
			continue;
		}
		QueuePCM( source, pcm, static_cast<int>( size ), rate, channels, 2 );
		++chunks;
	}
	if ( ( player || !wave.samples.empty() ) && queued > 0 ) {
		ALint state;
		alGetSourcei( source, AL_SOURCE_STATE, &state );
		if ( state != AL_PLAYING ) {
			alSourcePlay( source );
		}
	}
}

} // namespace

void MusicInit() {
	std::lock_guard<std::recursive_mutex> guard( musicMutex );
	source = NewStreamSource();
	if ( !fluidsynth.Load() ) {
		return;
	}
	settings = fluidsynth.new_settings();
	if ( settings ) {
		fluidsynth.settings_setnum( settings, "synth.sample-rate", 44100.0 );
		// the sample clock drives the MIDI player, so rendering pulls the
		// timeline along with the audio instead of wall time
		fluidsynth.settings_setstr( settings, "player.timing-source", "sample" );
	}
	synth = settings ? fluidsynth.new_synth( settings ) : nullptr;
	if ( synth ) {
		char path[MAX_OSPATH];
		Com_sprintf( path, sizeof( path ), "%s/base/sound/timidity/TimGM6mb.sf2",
					 Cvar_VariableString( "fs_basepath" ) );
		if ( fluidsynth.sfload( synth, path, 1 ) == FLUID_FAILED ) {
			Com_Printf( "FluidSynth: cannot read SoundFont %s\n", path );
		} else {
			synthesizer = true;
		}
	}
	if ( !synthesizer ) {
		// no FluidSynth DLL or no SoundFont: WAV music still works
		if ( synth ) {
			fluidsynth.delete_synth( synth );
			synth = nullptr;
		}
		if ( settings ) {
			fluidsynth.delete_settings( settings );
			settings = nullptr;
		}
		fluidsynth.Unload();
	}
}

void MusicStop() {
	std::lock_guard<std::recursive_mutex> guard( musicMutex );
	ClearStream( source );
	if ( player ) {
		fluidsynth.delete_player( player );
		player = nullptr;
	}
	wave = sndWave_t();
	midi.clear();
	wavePosition = 0;
	loopName[0] = 0;
	introName[0] = 0;
	loadPending = false;
}

void MusicShutdown() {
	std::lock_guard<std::recursive_mutex> guard( musicMutex );
	MusicStop();
	if ( source ) {
		alDeleteSources( 1, &source );
	}
	source = 0;
	if ( synth ) {
		fluidsynth.delete_synth( synth );
		synth = nullptr;
	}
	if ( settings ) {
		fluidsynth.delete_settings( settings );
		settings = nullptr;
	}
	fluidsynth.Unload();
	synthesizer = false;
}

void MusicStart( const char *intro, const char *loop ) {
	std::lock_guard<std::recursive_mutex> guard( musicMutex );
	const char *repeat = loop && *loop ? loop : intro;
	// the UI may ask for the same track repeatedly; while it is still
	// loaded the timeline just keeps advancing
	if ( intro && repeat && ( player || !wave.samples.empty() ) &&
		 !Q_stricmp( introName, intro ) && !Q_stricmp( loopName, repeat ) ) {
		return;
	}
	MusicStop();
	loops = chunks = 0;
	if ( !intro || !*intro ) {
		return;
	}
	Q_strncpyz( loopName, repeat, sizeof( loopName ) );
	Q_strncpyz( introName, intro, sizeof( introName ) );
	Load( intro );
}

void MusicInfo() {
	std::lock_guard<std::recursive_mutex> guard( musicMutex );
	ALint queued = 0, state = AL_STOPPED;
	if ( source ) {
		alGetSourcei( source, AL_BUFFERS_QUEUED, &queued );
		alGetSourcei( source, AL_SOURCE_STATE, &state );
	}
	Com_Printf( "Music: track=%s playing=%d queued=%d position=%u loops=%d backend=%s\n",
				introName, state == AL_PLAYING, queued,
				player ? static_cast<unsigned>( fluidsynth.player_get_current_tick( player ) )
					   : static_cast<unsigned>( wavePosition / ( wave.channels ? wave.channels : 1 ) ),
				loops, player ? "fluidsynth" : "pcm" );
}

void MusicUpdatePump( float volume ) {
	// the sound pump thread half: renders chunks, never reads files
	MusicUpdateInternal( volume, false );
}

bool MusicLoadPending() {
	std::lock_guard<std::recursive_mutex> guard( musicMutex );
	return loadPending;
}

void MusicLoadQueued( float volume ) {
	{
		std::lock_guard<std::recursive_mutex> guard( musicMutex );
		if ( !loadPending ) {
			return;
		}
		loadPending = false;
	}
	// the main thread performs the posted file load and keeps rendering
	// until the next chunk boundary
	MusicUpdateInternal( volume, true );
}
