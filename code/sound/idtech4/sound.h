// SPDX-License-Identifier: GPL-3.0-or-later
//
// The id Tech 4 sound system public interface, ported from the Doom 3
// (2004) sound system (references/doom3.gpl/neo/sound/sound.h) and mixed
// with the voice layer of the BFG edition audio engine
// (references/DOOM-3-BFG/neo/sound).
//
// The Doom 3 originals speak idLib types (idStr, idVec3, idDecl). idLib is
// vendored in code/idLib but not merged into the engine yet, so this port
// uses engine primitives (const char *, vec3_t) instead. Once idLib is
// truly part of the whole engine (TODO task 5), these signatures move to
// the idLib types and the temporary C shim layer
// (code/sound/snd_cshim.cpp) that bridges the old S_* C API onto this
// system is deleted.

#ifndef __SOUND_IDTECH4_PUBLIC_H__
#define __SOUND_IDTECH4_PUBLIC_H__

#include "../../game/game/q_shared.h"

// sound shader flags, values identical to the Doom 3 source
static const int SSF_PRIVATE_SOUND      = (1 << 0);  // only plays for the current listenerId
static const int SSF_ANTI_PRIVATE_SOUND = (1 << 1);  // plays for everyone but the listener
static const int SSF_NO_OCCLUSION       = (1 << 2);  // straight line only, portals come later
static const int SSF_GLOBAL             = (1 << 3);  // full volume, listener relative
static const int SSF_OMNIDIRECTIONAL    = (1 << 4);  // distance falloff, equal in all speakers
static const int SSF_LOOPING            = (1 << 5);  // repeat forever
static const int SSF_PLAY_ONCE          = (1 << 6);  // never restart while playing
static const int SSF_UNCLAMPED          = (1 << 7);  // do not clamp the volume at 1.0
static const int SSF_NO_FLICKER         = (1 << 8);  // always report 1.0 to amplitude queries
static const int SSF_NO_DUPS            = (1 << 9);  // avoid repeating the same entry

// the options that can be overridden per emitter and per channel
typedef struct {
	float minDistance;
	float maxDistance;
	float volume;            // linear 0-1; Doom 3 used dB, the engine is linear
	float shakes;
	int   soundShaderFlags;  // SSF_* bits
	int   soundClass;        // one of SOUND_MAX_CLASSES groups for global fades
} soundShaderParms_t;

static const int SOUND_MAX_LIST_WAVS = 32;
static const int SOUND_MAX_CLASSES  = 4;

class idSoundSample;

// sound channels: the game passes its own numbers, no casts required
typedef int s_channelType;
static const int SCHANNEL_ANY = 0;
static const int SCHANNEL_ONE = 1;

// The Doom 3 version derives from idDecl and is parsed by the decl manager
// from sound shader text. This game data has no sound shader decls, so every
// shader here is a default definition synthesized from one wav name, the
// same way idSoundShader::DefaultDefinition() builds one in Doom 3. The real
// parser (ParseShader with idLexer) plugs in when the idLib merge brings the
// decl manager.
class idSoundShader {
public:
	idSoundShader();

	void SetName( const char *n );
	const char *GetName() const { return name; }

	// builds the shader around a single wav file
	void SetDefaultDefinition( const char *sampleName );

	// parses a real Doom 3 shader text; unused until the decl manager lands
	bool Parse( const char *text, int textLength );

	const soundShaderParms_t *GetParms() const { return &parms; }
	int GetNumSounds() const { return numEntries; }
	const char *GetSound( int index ) const;

	// resolves and loads the entry sample through the sound cache
	idSoundSample *GetSample( int index );

private:
	void Init();

	char name[MAX_QPATH];
	soundShaderParms_t parms;
	char entries[SOUND_MAX_LIST_WAVS][MAX_QPATH];
	int numEntries;
	idSoundSample *samples[SOUND_MAX_LIST_WAVS];
};

class idSoundEmitter {
public:
	virtual ~idSoundEmitter() {}

	// a non-immediate free lets all currently playing sounds finish;
	// emitters are not deleted, they are just marked reusable
	virtual void Free( bool immediate ) = 0;

	// the parms are the defaults for every sound started on this emitter;
	// NULL is acceptable
	virtual void UpdateEmitter( const vec3_t origin, int listenerId,
							   const soundShaderParms_t *parms ) = 0;

	// returns the length of the started sound in msec
	// soundOrigin is an engine adapter: a one-shot position for sounds the
	// game starts at a fixed point instead of on the emitter; it goes away
	// with the idLib merge
	virtual int StartSound( const idSoundShader *shader, s_channelType channel,
							float diversity = 0, int shaderFlags = 0,
							bool allowSlow = true,
							const vec3_t soundOrigin = nullptr ) = 0;

	// pass SCHANNEL_ANY to affect every channel
	virtual void ModifySound( s_channelType channel, const soundShaderParms_t *parms ) = 0;
	virtual void StopSound( s_channelType channel ) = 0;
	// to is linear 0-1 here (dB in Doom 3), over is in seconds
	virtual void FadeSound( s_channelType channel, float to, float over ) = 0;

	virtual bool CurrentlyPlaying() const = 0;

	// 0.0 to 1.0 based on the current amplitude, for graphics effects
	virtual float CurrentAmplitude() const = 0;

	// marks a channel as wanted this frame, the looping sound bookkeeping
	// the engine's S_ClearLoopingSounds / S_AddLoopingSound pairs drive;
	// goes away with the idLib merge
	virtual void MarkChannel( s_channelType channel ) = 0;

	// for save games; index 0 is never a valid emitter
	virtual int Index() const = 0;
};

// There can be multiple independent sound worlds, just as there can be
// multiple render worlds (a live game and an editor preview at the same
// time, for example).
class idSoundWorld {
public:
	virtual ~idSoundWorld() {}

	virtual void ClearAllSoundEmitters() = 0;
	virtual void StopAllSounds() = 0;

	// get a new emitter that can play sounds in this world
	virtual idSoundEmitter *AllocSoundEmitter() = 0;

	// for load games; index 0 returns NULL
	virtual idSoundEmitter *EmitterForIndex( int index ) = 0;

	// where the camera/microphone is; gameTime is in msec and drives the
	// sound queries and removals, independent of the async update races
	// (areaName is dropped until portal occlusion comes with the idLib merge)
	virtual void PlaceListener( const vec3_t origin, const vec3_t axis[3],
								int listenerId, int gameTime ) = 0;

	// fade all sounds of one shader sound class
	virtual void FadeSoundClasses( int soundClass, float to, float over ) = 0;

	// background music and one shot dialogs
	virtual void PlayShaderDirectly( const char *name, int channel = -1 ) = 0;

	// pause and unpause this world
	virtual void Pause() = 0;
	virtual void UnPause() = 0;
	virtual bool IsPaused() const = 0;

	// clear the looping marks of every emitter, the engine frame calls
	// this before the game re-adds its looping sounds; goes away with the
	// idLib merge
	virtual void ClearLoopingMarks() = 0;

	// stop the looping sounds whose mark was not refreshed this frame
	virtual void ReapLoopingSounds() = 0;

	// main thread frame update: spatialize every active channel
	virtual void Update( int gameTime, float masterVolume ) = 0;
};

class idSoundSystem {
public:
	virtual ~idSoundSystem() {}

	// all non-hardware initialization
	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	// stop everything, the engine map change path
	virtual void StopEverything( bool streams, bool music ) = 0;

	virtual void SetMute( bool mute ) = 0;

	// AllocSoundWorld takes an idRenderWorld in Doom 3 for portal
	// occlusion; the port drops it until the renderer hooks exist
	virtual idSoundWorld *AllocSoundWorld() = 0;

	virtual void SetPlayingSoundWorld( idSoundWorld *soundWorld ) = 0;
	virtual idSoundWorld *GetPlayingSoundWorld() = 0;

	// sample cache bookkeeping at map load
	virtual void BeginLevelLoad() = 0;
	virtual void EndLevelLoad() = 0;

	// the 60Hz main thread update, like Doom 3's async loop
	virtual int AsyncUpdate( int time ) = 0;

	// 0: no EAX yet, the snd_efxfile reverb port comes later
	virtual int IsEAXAvailable() = 0;
};

extern idSoundSystem *soundSystem;

#endif
