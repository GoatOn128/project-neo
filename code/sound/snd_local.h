// SPDX-License-Identifier: GPL-3.0-or-later
//
// Internal declarations of the id Tech 4 sound port.
//
// TEMPORARY ENGINE ADAPTERS: everything in this port that talks to engine
// facilities goes through the old C functions (Cvar_Get, FS_ReadFile,
// Com_Printf, Sys_Milliseconds) instead of the idLib classes (idCVar,
// idFileSystem, idCommon, idSys). Those adapters switch to the idLib
// versions when idLib is merged into the whole engine (TODO task 5), and
// together with that merge the C shim layer (code/sound/snd_cshim.cpp)
// goes away.

#ifndef __SOUND_LOCAL_H__
#define __SOUND_LOCAL_H__

#include "../client/client.h"
#include "../openal/include/AL/al.h"
#include "../openal/include/AL/alc.h"
#include "../openal/include/AL/alext.h"
#include <SDL3/SDL.h>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <mutex>
#include <vector>
#include "sound.h"

// ---------------------------------------------------------------------------
// sizes, kept close to both reference engines
// ---------------------------------------------------------------------------
static const int SOUND_MAX_CHANNELS = 16;  // engine channel ids mask into this range
static const int SOUND_LOOP_CHANNEL = (SOUND_MAX_CHANNELS - 1);  // looping sounds live here
static const int SOUND_MAX_EMITTERS = 1024;
static const int SOUND_MAX_VOICES   = 128;  // the BFG style source pool
static const int SOUND_MAX_SAMPLES  = 4096;
static const int SOUND_MAX_SHADERS  = 4096;
static const int SOUND_MAX_STREAMS  = 16;   // wav file streams
static const int SOUND_MAX_RAW      = 32;   // cinematic raw sample channels
// the BFG voice layer keeps this many buffers queued per streaming source
static const int MAX_QUEUED_BUFFERS = 3;

class idSoundSystemLocal;

// ---------------------------------------------------------------------------
// idSoundSample: one loaded wav, the Doom 3 2004 sample with the BFG
// SoundSample flags (neverPurge, levelLoadReferenced)
// ---------------------------------------------------------------------------
class idSoundSample {
public:
	idSoundSample();

	const char *GetName() const { return name; }
	bool IsLoaded() const { return loaded; }
	bool IsDefault() const { return isDefault; }
	int LengthInMsec() const;
	int SampleRate() const { return rate; }
	int NumChannels() const { return channels; }
	int NumSamples() const { return numSamples; }
	ALuint GetBuffer() const { return buffer; }  // 0 when not loaded

	// reads the wav through the engine filesystem and uploads it
	bool Load();
	// the placeholder beep, what missing files fall back to
	void MakeDefault();
	// drops the OpenAL buffer, keeps the name so it can reload
	void FreeData();

	void SetNeverPurge( bool never ) { neverPurge = never; }
	void MarkLevelLoadReferenced() { levelLoadReferenced = true; }
	bool GetLevelLoadReferenced() const { return levelLoadReferenced; }

private:
	friend class idSoundCache;
	char name[MAX_QPATH];
	ALuint buffer;
	int rate, channels, numSamples;
	bool loaded, isDefault, neverPurge, levelLoadReferenced;
};

// ---------------------------------------------------------------------------
// idSoundCache: the sample table, Doom 3's snd_cache
// ---------------------------------------------------------------------------
class idSoundCache {
public:
	idSoundCache();
	// finds or creates the sample and loads it on first use
	idSoundSample *FindOrCreate( const char *name );
	void BeginLevelLoad();
	void EndLevelLoad();
	void FreeAll();
	int NumLoaded() const;

private:
	idSoundSample samples[SOUND_MAX_SAMPLES];
	int count;
};

// ---------------------------------------------------------------------------
// idSoundVoice: the BFG edition voice (SoundVoice.h / SoundVoiceBase.h),
// backed by an OpenAL source. The BFG engine hands these to the hardware
// (XAudio2 there, OpenAL here) and lets its own audio pump consume the
// queued buffers, which is what the sound pump thread below does.
// ---------------------------------------------------------------------------
class idSoundVoice {
public:
	idSoundVoice();

	void Create( ALuint source );
	void Destroy();
	bool Start( idSoundSample *leadinSample, idSoundSample *loopingSample, bool looping );
	void Stop();
	void Pause();
	void UnPause();
	// sends position/gain/pitch to the hardware and handles the
	// leadin -> loop transition of two-sample shaders
	void Update();
	bool IsPlaying() const;
	bool CompatibleFormat( const idSoundSample *sample ) const;

	void SetPosition( const vec3_t p, bool relative );
	void SetGain( float g ) { gain = g; }
	void SetPitch( float p ) { pitch = p; }
	void SetDistance( float minDistance, float maxDistance );
	void SetOwner( int emitterIndex, int channelIndex );

	bool IsAssigned() const { return assigned; }
	int EmitterIndex() const { return emitterIndex; }
	int ChannelIndex() const { return channelIndex; }

private:
	void ApplyParams();

	ALuint source;
	idSoundSample *leadinSample, *loopingSample;
	bool looping, paused, assigned, relative;
	bool switchedToLoop;
	float gain, pitch, minDistance, maxDistance;
	vec3_t position;
	int emitterIndex, channelIndex;
};

// ---------------------------------------------------------------------------
// idSoundChannel: one playing shader on one emitter channel, from the 2004
// system
// ---------------------------------------------------------------------------
class idSoundChannel {
public:
	idSoundChannel();

	void Reset();
	bool IsActive() const { return playing; }

	const idSoundShader *shader;
	soundShaderParms_t parms;
	int startTime;
	bool looping, playing, relative, marked;
	vec3_t position;     // fixed position when the sound started at a world point
	bool hasPosition;
	idSoundVoice *voice;
	int diversity;
	float currentGain;
	// per channel fade (FadeSound)
	float fadeTargetGain, fadeStartGain, fadeStartTime, fadeDuration;
};

// ---------------------------------------------------------------------------
// idSoundEmitterLocal
// ---------------------------------------------------------------------------
class idSoundWorldLocal;

class idSoundEmitterLocal : public idSoundEmitter {
public:
	idSoundEmitterLocal();

	void Init( int emitterIndex );

	// idSoundEmitter interface
	void Free( bool immediate ) override;
	void UpdateEmitter( const vec3_t origin, int listenerId,
						const soundShaderParms_t *parms ) override;
	int StartSound( const idSoundShader *shader, s_channelType channel,
					float diversity = 0, int shaderFlags = 0,
					bool allowSlow = true,
					const vec3_t soundOrigin = nullptr ) override;
	void ModifySound( s_channelType channel, const soundShaderParms_t *parms ) override;
	void StopSound( s_channelType channel ) override;
	void FadeSound( s_channelType channel, float to, float over ) override;
	bool CurrentlyPlaying() const override;
	float CurrentAmplitude() const override;
	void MarkChannel( s_channelType channel ) override;
	int Index() const override { return index; }

	// main thread spatialization, called by the world every frame
	void Update( const idSoundWorldLocal *world, int gameTime, float masterVolume );

	idSoundChannel *FindChannel( s_channelType channel );

	idSoundChannel channels[SOUND_MAX_CHANNELS];
	bool active;
	vec3_t origin;
	int listenerId;
	soundShaderParms_t parms;
	int index;
};

// ---------------------------------------------------------------------------
// idSoundWorldLocal
// ---------------------------------------------------------------------------
class idSoundWorldLocal : public idSoundWorld {
public:
	idSoundWorldLocal();

	// idSoundWorld interface
	void ClearAllSoundEmitters() override;
	void StopAllSounds() override;
	idSoundEmitter *AllocSoundEmitter() override;
	idSoundEmitter *EmitterForIndex( int index ) override;
	void PlaceListener( const vec3_t origin, const vec3_t axis[3],
						int listenerId, int gameTime ) override;
	void FadeSoundClasses( int soundClass, float to, float over ) override;
	void PlayShaderDirectly( const char *name, int channel = -1 ) override;
	void Pause() override;
	void UnPause() override;
	bool IsPaused() const override { return paused; }
	void ClearLoopingMarks() override;
	void ReapLoopingSounds() override;
	void Update( int gameTime, float masterVolume ) override;

	const vec3_t &ListenerOrigin() const { return listenerOrigin; }
	const vec3_t *ListenerAxis() const { return listenerAxis; }
	int ListenerId() const { return listenerId; }

	idSoundEmitterLocal emitters[SOUND_MAX_EMITTERS];

private:
	idSoundEmitterLocal directEmitter;  // PlayShaderDirectly target, outside the pool
	vec3_t listenerOrigin;
	vec3_t listenerAxis[3];
	int listenerId, gameTime;
	bool paused;
	// sound class fades, Doom 3's FadeSoundClasses
	float classFadeTo[SOUND_MAX_CLASSES];
	float classFadeStart[SOUND_MAX_CLASSES];
	int classFadeStartTime[SOUND_MAX_CLASSES];
	int classFadeDuration[SOUND_MAX_CLASSES];
};

// ---------------------------------------------------------------------------
// the wav decoder and the OpenAL PCM queue helpers of the old sound code,
// now part of this port: the shared plumbing of the music module and the
// cinematic streams
// ---------------------------------------------------------------------------

struct sndWave_t {
	int rate;
	int channels;
	std::vector<int16_t> samples;
};

// decodes a wav in memory, false on malformed input; snd_cache.cpp
bool DecodeWave( const byte *data, size_t length, sndWave_t &wave );

// OpenAL helpers for the music and cinematic streams; snd_hardware.cpp
unsigned int NewStreamSource();
void ClearStream( unsigned int source );
void QueuePCM( unsigned int source, const void *data, int bytes,
			  int rate, int channels, int width );

// ---------------------------------------------------------------------------
// idSoundHardware: the BFG SoundHardware interface over OpenAL, plus the
// engine's cinematic raw sample channels and wav file streams
// ---------------------------------------------------------------------------
struct soundStream_t {
	ALuint source;
	sndWave_t wave;
	size_t cursor;
	char loop[MAX_QPATH];
	int entity, channel;
	bool attenuated, needsLoad, active;
};

class idSoundHardware {
public:
	idSoundHardware();

	bool Init();
	void Shutdown();
	bool IsReady() const { return context != nullptr; }

	// game voices, main thread only (the BFG engine updates its voices from
	// the main thread and leaves the consuming to the audio pump)
	idSoundVoice *AllocateVoice();
	void FreeVoice( idSoundVoice *voice );
	int NumFreeVoices() const;
	idSoundVoice *VoiceFor( int emitterIndex, int channelIndex );

	// listener, main thread
	void ApplyListener( const vec3_t origin, const vec3_t axis[3] );

	// cinematic raw samples, main thread (S_RawSamples)
	bool QueueRawSamples( int index, int samples, int rate, int width, int channels,
						  const byte *data, float left, float right );
	void SetRawStreamGain( int index, float gain );
	void ClearRawStreams();

	// entity positions for the attenuated streams, main thread
	void UpdateEntityPosition( int entity, const vec3_t origin );

	// wav file streams: the main thread sets them up (it owns the
	// filesystem), the sound pump thread feeds their buffers
	void StartStream( const char *intro, const char *loop, int entity, int channel,
					  int attenuation );
	void StopStream( int index );
	void StopEntityStreams( int entity );
	void StopAllStreams();
	void ServiceStreamLoads();   // main thread: the loop file reads the pump asked for
	void UpdateStreams( float gain );  // pump thread: queue the next buffers

	// device pause for the editor, main thread
	void PauseDevice();
	void ResumeDevice();

private:
	bool ReadStream( soundStream_t &stream, const char *path );  // main thread only
	void StopStreamLocked( int index );  // pumpMutex already held

	ALCdevice *device;
	ALCcontext *context;
	// ALC_SOFT_pause_device entry points, resolved at runtime like the old
	// sound code did, because the bundled import library does not export them
	LPALCDEVICEPAUSESOFT PauseDeviceSOFT;
	LPALCDEVICERESUMESOFT ResumeDeviceSOFT;
	idSoundVoice voices[SOUND_MAX_VOICES];
	ALuint rawSources[SOUND_MAX_RAW];
	soundStream_t streams[SOUND_MAX_STREAMS];
	vec3_t entityPositions[MAX_GENTITIES];
};

// ---------------------------------------------------------------------------
// idSoundSystemLocal
// ---------------------------------------------------------------------------
class idSoundSystemLocal : public idSoundSystem {
public:
	idSoundSystemLocal();

	// idSoundSystem interface
	void Init() override;
	void Shutdown() override;
	void StopEverything( bool streams, bool music ) override;
	void SetMute( bool mute ) override;
	idSoundWorld *AllocSoundWorld() override;
	void SetPlayingSoundWorld( idSoundWorld *soundWorld ) override;
	idSoundWorld *GetPlayingSoundWorld() override;
	void BeginLevelLoad() override;
	void EndLevelLoad() override;
	int AsyncUpdate( int time ) override;
	int IsEAXAvailable() override;

	// engine side extras, deleted with the idLib merge
	void SetEditorPaused( bool paused );
	void FadeAll( float target, int time );
	// registers a sample name as a default sound shader and preloads it,
	// the S_RegisterSound translation
	int RegisterShader( const char *name );
	idSoundShader *GetShader( int handle );
	void PrintInfo();

	idSoundHardware hardware;
	idSoundCache sampleCache;
	idSoundWorldLocal *World() { return currentWorld; }

	// the sound pump thread (the BFG audio engine's background work)
	SDL_Thread *pumpThread;
	std::mutex pumpMutex;          // guards the stream slots and the entity positions
	std::atomic<bool> pumpRunning;
	std::atomic<bool> editorPaused;
	std::atomic<bool> muted;
	std::atomic<float> pumpStreamGain;  // s_volume copy the pump thread reads
	std::atomic<float> pumpMusicGain;   // s_musicvolume copy the pump thread reads
	int editorPauseStarted;

	// global fade (the old S_FadeAllSounds behavior)
	float globalGain, fadeStartGain, fadeTarget;
	int fadeStart, fadeEnd;

private:
	idSoundWorldLocal worlds[2];
	int worldCount;
	idSoundWorldLocal *currentWorld;
	idSoundShader shaders[SOUND_MAX_SHADERS];
	int shaderCount;
};

// the concrete system, one instance, like the original engine
extern idSoundSystemLocal *soundSystemLocalPtr;

// ---------------------------------------------------------------------------
// the music module (snd_music.cpp): the port's own background music,
// FluidSynth MIDI scores and wav PCM on one OpenAL source, rendered by
// the sound pump thread
// ---------------------------------------------------------------------------
void MusicInit();
void MusicShutdown();
void MusicStop();
void MusicStart( const char *intro, const char *loop );
void MusicInfo();
void MusicUpdatePump( float volume );  // pump thread: renders, never reads files
bool MusicLoadPending();               // a loop reload the pump thread posted
void MusicLoadQueued( float volume );  // main thread: performs the posted load

// console commands, the engine C API until idCmdArgs exists with idLib
void PlaySoundCommand();
void MusicCommand();
void InfoCommand();
void StopSoundCommand();

#endif
