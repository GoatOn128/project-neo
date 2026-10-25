// SPDX-License-Identifier: GPL-3.0-or-later
//
// Sound shaders of the id Tech 4 sound port: snd_shader.cpp of the Doom 3
// (2004) system. The decl manager and the idLexer based shader parser
// arrive with the idLib merge (TODO task 5); until then every shader is a
// default definition built around one wav name, which is what the engine
// game data needs.

#include "snd_local.h"

idSoundShader::idSoundShader() {
	Init();
}

void idSoundShader::Init() {
	name[0] = 0;
	// the engine defaults the old OpenAL code used; Doom 3 defaults are in
	// meters, the engine works in its own units
	parms.minDistance = 160.0f;
	parms.maxDistance = 1250.0f;
	parms.volume = 1.0f;
	parms.shakes = 0.0f;
	parms.soundShaderFlags = 0;
	parms.soundClass = 0;
	for ( int i = 0; i < SOUND_MAX_LIST_WAVS; ++i ) {
		entries[i][0] = 0;
		samples[i] = nullptr;
	}
	numEntries = 0;
}

void idSoundShader::SetName( const char *n ) {
	Q_strncpyz( name, n, sizeof( name ) );
}

void idSoundShader::SetDefaultDefinition( const char *sampleName ) {
	Init();
	Q_strncpyz( entries[0], sampleName, sizeof( entries[0] ) );
	numEntries = 1;
}

bool idSoundShader::Parse( const char *text, int textLength ) {
	// the real ParseShader (idLexer, minDistance/maxDistance/volume/loops/
	// leadins/altSound, the whole 2004 keyword set) plugs in here with the
	// decl manager when idLib is merged into the engine
	( void )text;
	( void )textLength;
	return false;
}

const char *idSoundShader::GetSound( int index ) const {
	if ( index < 0 || index >= numEntries ) {
		return "";
	}
	return entries[index];
}

idSoundSample *idSoundShader::GetSample( int index ) {
	if ( index < 0 || index >= numEntries ) {
		return nullptr;
	}
	if ( !samples[index] ) {
		samples[index] = soundSystemLocalPtr->sampleCache.FindOrCreate( entries[index] );
		if ( samples[index] ) {
			samples[index]->MarkLevelLoadReferenced();
		}
	}
	// on demand reload after a level load purge (the allowSlow path of
	// the original system)
	if ( samples[index] && !samples[index]->IsLoaded() ) {
		samples[index]->Load();
	}
	return samples[index];
}
