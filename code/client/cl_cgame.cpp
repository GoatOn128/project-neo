/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein single player GPL Source Code (“RTCW SP Source Code”).  

RTCW SP Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RTCW SP Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RTCW SP Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the RTCW SP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the RTCW SP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning the license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

// cl_cgame.cpp  -- client system interaction with client game

#include "client.h"

#include "../game/botlib.h"

extern botlib_export_t *botlib_export;

#include "../splines/camera.h"

// RF, this is only used when running a local server
extern void SV_SendMoveSpeedsToGame( int entnum, char *text );
extern bool SV_GetModelInfo( int clientNum, char *modelName, animModelInfo_t **modelInfo );


/*
====================
CL_GetGameState
====================
*/
void CL_GetGameState( gameState_t *gs ) {
	*gs = cl.gameState;
}

/*
====================
CL_GetGlconfig
====================
*/
void CL_GetGlconfig( glconfig_t *glconfig ) {
	*glconfig = cls.glconfig;
}


/*
====================
CL_GetUserCmd
====================
*/
bool CL_GetUserCmd( int cmdNumber, usercmd_t *ucmd ) {
	// cmds[cmdNumber] is the last properly generated command

	// can't return anything that we haven't created yet
	if ( cmdNumber > cl.cmdNumber ) {
		Com_Error( ERR_DROP, "CL_GetUserCmd: %i >= %i", cmdNumber, cl.cmdNumber );
	}

	// the usercmd has been overwritten in the wrapping
	// buffer because it is too far out of date
	if ( cmdNumber <= cl.cmdNumber - CMD_BACKUP ) {
		return false;
	}

	*ucmd = cl.cmds[ cmdNumber & CMD_MASK ];

	return true;
}

int CL_GetCurrentCmdNumber( void ) {
	return cl.cmdNumber;
}


/*
====================
CL_GetParseEntityState
====================
*/
bool    CL_GetParseEntityState( int parseEntityNumber, entityState_t *state ) {
	// can't return anything that hasn't been parsed yet
	if ( parseEntityNumber >= cl.parseEntitiesNum ) {
		Com_Error( ERR_DROP, "CL_GetParseEntityState: %i >= %i",
				   parseEntityNumber, cl.parseEntitiesNum );
	}

	// can't return anything that has been overwritten in the circular buffer
	if ( parseEntityNumber <= cl.parseEntitiesNum - MAX_PARSE_ENTITIES ) {
		return false;
	}

	*state = cl.parseEntities[ parseEntityNumber & ( MAX_PARSE_ENTITIES - 1 ) ];
	return true;
}

/*
====================
CL_GetCurrentSnapshotNumber
====================
*/
void    CL_GetCurrentSnapshotNumber( int *snapshotNumber, int *serverTime ) {
	*snapshotNumber = cl.snap.messageNum;
	*serverTime = cl.snap.serverTime;
}

/*
====================
CL_GetSnapshot
====================
*/
bool    CL_GetSnapshot( int snapshotNumber, snapshot_t *snapshot ) {
	clSnapshot_t    *clSnap;
	int i, count;

	if ( snapshotNumber > cl.snap.messageNum ) {
		Com_Error( ERR_DROP, "CL_GetSnapshot: snapshotNumber > cl.snapshot.messageNum" );
	}

	// if the frame has fallen out of the circular buffer, we can't return it
	if ( cl.snap.messageNum - snapshotNumber >= PACKET_BACKUP ) {
		return false;
	}

	// if the frame is not valid, we can't return it
	clSnap = &cl.snapshots[snapshotNumber & PACKET_MASK];
	if ( !clSnap->valid ) {
		return false;
	}

	// if the entities in the frame have fallen out of their
	// circular buffer, we can't return it
	if ( cl.parseEntitiesNum - clSnap->parseEntitiesNum >= MAX_PARSE_ENTITIES ) {
		return false;
	}

	// write the snapshot
	snapshot->snapFlags = clSnap->snapFlags;
	snapshot->serverCommandSequence = clSnap->serverCommandNum;
	snapshot->ping = clSnap->ping;
	snapshot->serverTime = clSnap->serverTime;
	memcpy( snapshot->areamask, clSnap->areamask, sizeof( snapshot->areamask ) );
	snapshot->ps = clSnap->ps;
	count = clSnap->numEntities;
	if ( count > MAX_ENTITIES_IN_SNAPSHOT ) {
		Com_DPrintf( "CL_GetSnapshot: truncated %i entities to %i\n", count, MAX_ENTITIES_IN_SNAPSHOT );
		count = MAX_ENTITIES_IN_SNAPSHOT;
	}
	snapshot->numEntities = count;
	for ( i = 0 ; i < count ; i++ ) {
		snapshot->entities[i] =
			cl.parseEntities[ ( clSnap->parseEntitiesNum + i ) & ( MAX_PARSE_ENTITIES - 1 ) ];
	}

	// FIXME: configstring changes and server commands!!!

	return true;
}

/*
==============
CL_SetUserCmdValue
==============
*/
void CL_SetUserCmdValue( int userCmdValue, int holdableValue, float sensitivityScale, int cld ) {
	cl.cgameUserCmdValue        = userCmdValue;
	cl.cgameUserHoldableValue   = holdableValue;
	cl.cgameSensitivity         = sensitivityScale;
	cl.cgameCld                 = cld;
}

/*
==============
CL_AddCgameCommand
==============
*/
void CL_AddCgameCommand( const char *cmdName ) {
	Cmd_AddCommand( cmdName, NULL );
}

/*
==============
CL_CgameError
==============
*/
void CL_CgameError( const char *string ) {
	Com_Error( ERR_DROP, "%s", string );
}


/*
=====================
CL_ConfigstringModified
=====================
*/
void CL_ConfigstringModified( void ) {
	char        *old, *s;
	int i, index;
	char        *dup;
	gameState_t oldGs;
	int len;

	index = atoi( Cmd_Argv( 1 ) );
	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		Com_Error( ERR_DROP, "configstring > MAX_CONFIGSTRINGS" );
	}
//	s = Cmd_Argv(2);
	// get everything after the "cs <num>"
	s = Cmd_ArgsFrom( 2 );

	old = cl.gameState.stringData + cl.gameState.stringOffsets[ index ];
	if ( !strcmp( old, s ) ) {
		return;     // unchanged
	}

	// build the new gameState_t
	oldGs = cl.gameState;

	memset( &cl.gameState, 0, sizeof( cl.gameState ) );

	// leave the first 0 for uninitialized strings
	cl.gameState.dataCount = 1;

	for ( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ ) {
		if ( i == index ) {
			dup = s;
		} else {
			dup = oldGs.stringData + oldGs.stringOffsets[ i ];
		}
		if ( !dup[0] ) {
			continue;       // leave with the default empty string
		}

		len = strlen( dup );

		if ( len + 1 + cl.gameState.dataCount > MAX_GAMESTATE_CHARS ) {
			Com_Error( ERR_DROP, "MAX_GAMESTATE_CHARS exceeded" );
		}

		// append it to the gameState string buffer
		cl.gameState.stringOffsets[ i ] = cl.gameState.dataCount;
		memcpy( cl.gameState.stringData + cl.gameState.dataCount, dup, len + 1 );
		cl.gameState.dataCount += len + 1;
	}

	if ( index == CS_SYSTEMINFO ) {
		// parse serverId and other cvars
		CL_SystemInfoChanged();
	}

}


/*
===================
CL_GetServerCommand

Set up argc/argv for the given command
===================
*/
bool CL_GetServerCommand( int serverCommandNumber ) {
	char    *s;
	char    *cmd;
	static char bigConfigString[BIG_INFO_STRING];

	// if we have irretrievably lost a reliable command, drop the connection
	if ( serverCommandNumber <= clc.serverCommandSequence - MAX_RELIABLE_COMMANDS ) {
		// when a demo record was started after the client got a whole bunch of
		// reliable commands then the client never got those first reliable commands
		if ( clc.demoplaying ) {
			return false;
		}
		Com_Error( ERR_DROP, "CL_GetServerCommand: a reliable command was cycled out" );
		return false;
	}

	if ( serverCommandNumber > clc.serverCommandSequence ) {
		Com_Error( ERR_DROP, "CL_GetServerCommand: requested a command not received" );
		return false;
	}

	s = clc.serverCommands[ serverCommandNumber & ( MAX_RELIABLE_COMMANDS - 1 ) ];
	clc.lastExecutedServerCommand = serverCommandNumber;

	Com_DPrintf( "serverCommand: %i : %s\n", serverCommandNumber, s );

rescan:
	Cmd_TokenizeString( s );
	cmd = Cmd_Argv( 0 );

	if ( !strcmp( cmd, "disconnect" ) ) {
		Com_Error( ERR_SERVERDISCONNECT,"Server disconnected\n" );
	}

	if ( !strcmp( cmd, "bcs0" ) ) {
		Com_sprintf( bigConfigString, BIG_INFO_STRING, "cs %s \"%s", Cmd_Argv( 1 ), Cmd_Argv( 2 ) );
		return false;
	}

	if ( !strcmp( cmd, "bcs1" ) ) {
		s = Cmd_Argv( 2 );
		if ( strlen( bigConfigString ) + strlen( s ) >= BIG_INFO_STRING ) {
			Com_Error( ERR_DROP, "bcs exceeded BIG_INFO_STRING" );
		}
		strcat( bigConfigString, s );
		return false;
	}

	if ( !strcmp( cmd, "bcs2" ) ) {
		s = Cmd_Argv( 2 );
		if ( strlen( bigConfigString ) + strlen( s ) + 1 >= BIG_INFO_STRING ) {
			Com_Error( ERR_DROP, "bcs exceeded BIG_INFO_STRING" );
		}
		strcat( bigConfigString, s );
		strcat( bigConfigString, "\"" );
		s = bigConfigString;
		goto rescan;
	}

	if ( !strcmp( cmd, "cs" ) ) {
		CL_ConfigstringModified();
		// reparse the string, because CL_ConfigstringModified may have done another Cmd_TokenizeString()
		Cmd_TokenizeString( s );
		return true;
	}

	if ( !strcmp( cmd, "map_restart" ) ) {
		// clear notify lines and outgoing commands before passing
		// the restart to the cgame
		Con_ClearNotify();
		memset( cl.cmds, 0, sizeof( cl.cmds ) );
		return true;
	}

	if ( !strcmp( cmd, "popup" ) ) { // direct server to client popup request, bypassing cgame
//		trap_UI_Popup(Cmd_Argv(1));
//		if ( cls.state == CA_ACTIVE && !clc.demoplaying ) {
//			VM_Call( uivm, UI_SET_ACTIVE_MENU, UIMENU_CLIPBOARD);
//			Menus_OpenByName(Cmd_Argv(1));
//		}
		return false;
	}


	// the clientLevelShot command is used during development
	// to generate 128*128 screenshots from the intermission
	// point of levels for the menu system to use
	// we pass it along to the cgame to make apropriate adjustments,
	// but we also clear the console and notify lines here
	if ( !strcmp( cmd, "clientLevelShot" ) ) {
		// don't do it if we aren't running the server locally,
		// otherwise malicious remote servers could overwrite
		// the existing thumbnails
		if ( !com_sv_running->integer ) {
			return false;
		}
		// close the console
		Con_Close();
		// take a special screenshot next frame
		Cbuf_AddText( "wait ; wait ; wait ; wait ; screenshot levelshot\n" );
		return true;
	}

	// we may want to put a "connect to other server" command here

	// cgame can now act on the command
	return true;
}


/*
====================
CL_CM_LoadMap

Just adds default parameters that cgame doesn't need to know about
====================
*/
void CL_CM_LoadMap( const char *mapname ) {
	int checksum;

	CM_LoadMap( mapname, true, &checksum );
}

/*
====================
CL_ShutdonwCGame

====================
*/

// The loaded cgame DLL and the exports it provides.
static void *cgameDllHandle = NULL;
cgameExport_t *cgameExport = NULL;

void CL_ShutdownCGame( void ) {
	cls.keyCatchers &= ~KEYCATCH_CGAME;
	cls.cgameStarted = false;
	if ( !cgameExport ) {
		return;
	}
	cgameExport->Shutdown();
	Sys_UnloadDll( cgameDllHandle );
	cgameDllHandle = NULL;
	cgameExport = NULL;
}

// The import table lists the engine functions directly. Inline lambdas only
// adapt the signatures that differ from what the cgame module expects.
static const cgameImport_t cgameImportTable = {
	CGAME_IMPORT_API_VERSION,

	// Com_Printf and Com_Error are variadic, the table takes plain strings
	[]( const char *msg ) { Com_Printf( "%s", msg ); },
	[]( const char *msg ) { Com_Error( ERR_DROP, "%s", msg ); },
	Sys_Milliseconds,
	Cvar_Register,
	Cvar_Update,
	Cvar_Set,
	Cvar_VariableStringBuffer,
	Cmd_Argc,
	Cmd_ArgvBuffer,
	Cmd_ArgsBuffer,
	FS_FOpenFileByMode,
	[]( void *buffer, int len, fileHandle_t f ) { FS_Read( buffer, len, f ); },
	[]( const void *buffer, int len, fileHandle_t f ) { FS_Write( buffer, len, f ); },
	FS_FCloseFile,
	Cbuf_AddText,
	CL_AddCgameCommand,
	CL_AddReliableCommand,
	SCR_UpdateScreen,
	CL_CM_LoadMap,
	CM_NumInlineModels,
	CM_InlineModel,
	[]( const vec3_t mins, const vec3_t maxs ) { return CM_TempBoxModel( mins, maxs, false ); },
	[]( const vec3_t mins, const vec3_t maxs ) { return CM_TempBoxModel( mins, maxs, true ); },
	CM_PointContents,
	CM_TransformedPointContents,
	[]( trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask ) { CM_BoxTrace( results, start, end, mins, maxs, model, brushmask, false ); },
	[]( trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles ) { CM_TransformedBoxTrace( results, start, end, mins, maxs, model, brushmask, origin, angles, false ); },
	[]( trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask ) { CM_BoxTrace( results, start, end, mins, maxs, model, brushmask, true ); },
	[]( trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles ) { CM_TransformedBoxTrace( results, start, end, mins, maxs, model, brushmask, origin, angles, true ); },
	[]( int numPoints, const vec3_t *points, const vec3_t projection, int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t *fragmentBuffer ) { return re.MarkFragments( numPoints, points, projection, maxPoints, pointBuffer, maxFragments, fragmentBuffer ); },
	S_StartSound,
	S_StartSoundEx,
	S_StartLocalSound,
	[]( int killall ) {
		S_ClearLoopingSounds(); // (SA) so no_pvs sounds can function
		// RF, if killall, then stop all sounds
		if ( killall == 1 ) {
			S_ClearSounds( true, false );
		} else if ( killall == 2 ) {
			S_ClearSounds( true, true );
		}
	},
	[]( int entityNum, const vec3_t origin, const vec3_t velocity, int range, sfxHandle_t sfx, int volume ) { S_AddLoopingSound( entityNum, origin, velocity, range, sfx, volume ); },
	[]( int ) { /* not functional anymore, the old looping code was reverted */ },
	[]( int entityNum ) { S_StopEntStreamingSound( entityNum ); },
	S_UpdateEntityPosition,
	S_GetVoiceAmplitude,
	S_Respatialize,
	// DOOMSOUND skips the loading flag the engine variant takes
	[]( const char *sample ) {
#ifdef DOOMSOUND
		return S_RegisterSound( sample );
#else
		return S_RegisterSound( sample, false );
#endif
	},
	S_StartBackgroundTrack,
	[]( float targetvol, int time, int num ) { S_FadeStreamingSound( targetvol, time, num ); },
	[]( float targetvol, int time ) { S_FadeAllSounds( targetvol, time ); },
	S_StartStreamingSound,
	[]( const char *mapname ) { re.LoadWorld( mapname ); },
	[]( const char *name ) { return re.RegisterModel( name ); },
	[]( qhandle_t skinid, const char *type, char *name ) { return re.GetSkinModel( skinid, type, name ); },
	[]( qhandle_t modelid, int surfnum, int withlightmap ) { return re.GetShaderFromModel( modelid, surfnum, withlightmap ); },
	[]( const char *name ) { return re.RegisterSkin( name ); },
	[]( const char *name ) { return re.RegisterShader( name ); },
	[]( const char *name ) { return re.RegisterShaderNoMip( name ); },
	[]( const char *fontName, int pointSize, fontInfo_t *font ) { re.RegisterFont( fontName, pointSize, font ); },
	[]() { re.ClearScene(); },
	[]( const refEntity_t *rentity ) { re.AddRefEntityToScene( rentity ); },
	[]( qhandle_t hShader, int numVerts, const polyVert_t *verts ) { re.AddPolyToScene( hShader, numVerts, verts ); },
	[]( qhandle_t hShader, int numVerts, const polyVert_t *verts, int numPolys ) { re.AddPolysToScene( hShader, numVerts, verts, numPolys ); },
	[]( int entityNum, const vec3_t hitPos, const vec3_t hitDir ) { re.ZombieFXAddNewHit( entityNum, hitPos, hitDir ); },
	[]( const vec3_t org, float intensity, float r, float g, float b, int overdraw ) { re.AddLightToScene( org, intensity, r, g, b, overdraw ); },
	[]( const vec3_t org, float r, float g, float b, float scale, int id, int flags ) { re.AddCoronaToScene( org, r, g, b, scale, id, flags ); },
	[]( int fogvar, int var1, int var2, float r, float g, float b, float density ) { re.SetFog( fogvar, var1, var2, r, g, b, density ); },
	[]( const refdef_t *fd ) { re.RenderScene( fd ); },
	[]( const float *rgba ) { re.SetColor( rgba ); },
	[]( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader ) { re.DrawStretchPic( x, y, w, h, s1, t1, s2, t2, hShader ); },
	[]( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader, const float *gradientColor, int gradientType ) { re.DrawStretchPicGradient( x, y, w, h, s1, t1, s2, t2, hShader, gradientColor, gradientType ); },
	[]( clipHandle_t model, vec3_t mins, vec3_t maxs ) { re.ModelBounds( model, mins, maxs ); },
	[]( orientation_t *tag, const refEntity_t *refent, const char *tagName, int startIndex ) { return re.LerpTag( tag, refent, tagName, startIndex ); },
	[]( const char *oldShader, const char *newShader, const char *timeOffset ) { re.RemapShader( oldShader, newShader, timeOffset ); },
	CL_GetGlconfig,
	CL_GetGameState,
	CL_GetCurrentSnapshotNumber,
	CL_GetSnapshot,
	CL_GetServerCommand,
	CL_GetCurrentCmdNumber,
	CL_GetUserCmd,
	CL_SetUserCmdValue,
	Hunk_MemoryRemaining,
	loadCamera,
	[]( int camNum, int time ) {
		if ( camNum == 0 ) { // CAM_PRIMARY
			cl.cameraMode = true;
		}
		startCamera( camNum, time );
	},
	[]( int camNum ) {
		if ( camNum == 0 ) { // CAM_PRIMARY
			cl.cameraMode = false;
		}
	},
	[]( int camNum, int time, vec3_t *origin, vec3_t *angles, float *fov ) { return (bool)getCameraInfo( camNum, time, *origin, *angles, fov ); },
	Key_IsDown,
	Key_GetCatcher,
	Key_SetCatcher,
	Key_GetKey,
	[]( char *define ) { return botlib_export->PC_AddGlobalDefine( define ); },
	[]( const char *filename ) { return botlib_export->PC_LoadSourceHandle( filename ); },
	[]( int handle ) { return botlib_export->PC_FreeSourceHandle( handle ); },
	[]( int handle, pc_token_t *token ) { return botlib_export->PC_ReadTokenHandle( handle, token ); },
	[]( int handle, char *filename, int *line ) { return botlib_export->PC_SourceFileAndLine( handle, filename, line ); },
	S_StopBackgroundTrack,
	Com_RealTime,
	[]( float *v ) { Sys_SnapVector( v ); },
	SV_SendMoveSpeedsToGame,
	CIN_PlayCinematic,
	CIN_StopCinematic,
	CIN_RunCinematic,
	CIN_DrawCinematic,
	CIN_SetExtents,
	[]( char *buffer, int bufferSize ) { return re.GetEntityToken( buffer, bufferSize ); },
	// popup names map onto the fixed engine menus
	[]( const char *name ) {
		if ( name && !Q_stricmp( name, "briefing" ) ) {
			uiExport->SetActiveMenu( UIMENU_BRIEFING );
			return;
		}
		if ( cls.state == CA_ACTIVE && !clc.demoplaying ) {
			if ( name && !Q_stricmp( name, "UIMENU_WM_PICKTEAM" ) ) {
				uiExport->SetActiveMenu( UIMENU_WM_PICKTEAM );
			} else if ( name && !Q_stricmp( name, "UIMENU_WM_PICKPLAYER" ) ) {
				uiExport->SetActiveMenu( UIMENU_WM_PICKPLAYER );
			} else if ( name && !Q_stricmp( name, "UIMENU_WM_QUICKMESSAGE" ) ) {
				uiExport->SetActiveMenu( UIMENU_WM_QUICKMESSAGE );
			} else if ( name && !Q_stricmp( name, "UIMENU_WM_LIMBO" ) ) {
				uiExport->SetActiveMenu( UIMENU_WM_LIMBO );
			} else if ( name && !Q_stricmp( name, "pregame" ) ) {
				uiExport->SetActiveMenu( UIMENU_PREGAME );
			} else {
				uiExport->SetActiveMenu( UIMENU_CLIPBOARD );
			}
		}
	},
	[]( const char *text ) { if ( text ) { CL_AddToLimboChat( text ); } },
	[]( const char * ) { uiExport->KeyEvent( K_ESCAPE, true ); },
	[]( int clientNum, char *modelName, void **modelInfo ) { return SV_GetModelInfo( clientNum, modelName, (animModelInfo_t **)modelInfo ); },
	[]( int sector, int light ) { re.SetDoomSectorLight( sector, light ); },
	[]( const char *state ) { re.SetDoomSwitches( state ); },
	[]( vec3_t point, vec3_t ambient, vec3_t directed, vec3_t lightDirection ) { return re.LightForPoint( point, ambient, directed, lightDirection ); },
	[]( int x, int y, int width, int height, const byte *pixels ) {
		if ( width <= 0 || height <= 0 || width > 16384 || height > 16384 || !pixels ) {
			Com_Error( ERR_DROP, "Invalid Doom automap image" );
		}
		re.DrawStretchRaw( x, y, width, height, 256, 256, pixels, 31, true );
	},
};

/*
====================
CL_UpdateLevelHunkUsage

  This updates the "hunkusage.dat" file with the current map and it's hunk usage count

  This is used for level loading, so we can show a percentage bar dependant on the amount
  of hunk memory allocated so far

  This will be slightly inaccurate if some settings like sound quality are changed, but these
  things should only account for a small variation (hopefully)
====================
*/
void CL_UpdateLevelHunkUsage( void ) {
	int handle;
	const char *memlistfile = "hunkusage.dat";
	char *buf, *outbuf;
	char *buftrav, *outbuftrav;
	char *token;
	char outstr[256];
	int len, memusage;

	memusage = Cvar_VariableIntegerValue( "com_hunkused" ) + Cvar_VariableIntegerValue( "hunk_soundadjust" );

	len = FS_FOpenFileByMode( memlistfile, &handle, FS_READ );
	if ( len >= 0 ) { // the file exists, so read it in, strip out the current entry for this map, and save it out, so we can append the new value

		buf = (char *)Z_Malloc( len + 1 );
		memset( buf, 0, len + 1 );
		outbuf = (char *)Z_Malloc( len + 1 );
		memset( outbuf, 0, len + 1 );

		FS_Read( (void *)buf, len, handle );
		FS_FCloseFile( handle );

		// now parse the file, filtering out the current map
		buftrav = buf;
		outbuftrav = outbuf;
		outbuftrav[0] = '\0';
		while ( ( token = COM_Parse( &buftrav ) ) && token[0] ) {
			if ( !Q_strcasecmp( token, cl.mapname ) ) {
				// found a match
				token = COM_Parse( &buftrav );  // read the size
				if ( token && token[0] ) {
					if ( atoi( token ) == memusage ) {  // if it is the same, abort this process
						Z_Free( buf );
						Z_Free( outbuf );
						return;
					}
				}
			} else {    // send it to the outbuf
				Q_strcat( outbuftrav, len + 1, token );
				Q_strcat( outbuftrav, len + 1, " " );
				token = COM_Parse( &buftrav );  // read the size
				if ( token && token[0] ) {
					Q_strcat( outbuftrav, len + 1, token );
					Q_strcat( outbuftrav, len + 1, "\n" );
				} else {
					Com_Error( ERR_DROP, "hunkusage.dat file is corrupt\n" );
				}
			}
		}

#ifdef __MACOS__    //DAJ MacOS file typing
		{
			extern _MSL_IMP_EXP_C long _fcreator, _ftype;
			_ftype = 'WlfB';
			_fcreator = 'WlfS';
		}
#endif
		handle = FS_FOpenFileWrite( memlistfile );
		if ( handle < 0 ) {
			Com_Error( ERR_DROP, "cannot create %s\n", memlistfile );
		}
		// input file is parsed, now output to the new file
		len = strlen( outbuf );
		if ( FS_Write( (void *)outbuf, len, handle ) != len ) {
			Com_Error( ERR_DROP, "cannot write to %s\n", memlistfile );
		}
		FS_FCloseFile( handle );

		Z_Free( buf );
		Z_Free( outbuf );
	}
	// now append the current map to the current file
	FS_FOpenFileByMode( memlistfile, &handle, FS_APPEND );
	if ( handle < 0 ) {
		Com_Error( ERR_DROP, "cannot write to hunkusage.dat, check disk full\n" );
	}
	Com_sprintf( outstr, sizeof( outstr ), "%s %i\n", cl.mapname, memusage );
	FS_Write( outstr, strlen( outstr ), handle );
	FS_FCloseFile( handle );

	// now just open it and close it, so it gets copied to the pak dir
	len = FS_FOpenFileByMode( memlistfile, &handle, FS_READ );
	if ( len >= 0 ) {
		FS_FCloseFile( handle );
	}
}

/*
====================
CL_InitCGame

Should only by called by CL_StartHunkUsers
====================
*/
void CL_InitCGame( void ) {
	const char          *info;
	const char          *mapname;
	int t1, t2;

	t1 = Sys_Milliseconds();

	// put away the console
	Con_Close();

	// find the current mapname
	info = cl.gameState.stringData + cl.gameState.stringOffsets[ CS_SERVERINFO ];
	mapname = Info_ValueForKey( info, "mapname" );
	Com_sprintf( cl.mapname, sizeof( cl.mapname ), "maps/%s.bsp", mapname );

	// load the cgame DLL
	cgameExport = (cgameExport_t *)Sys_LoadDll( "cgame", &cgameDllHandle, "GetCGameAPI", (void *)&cgameImportTable );
	if ( !cgameExport ) {
		Com_Error( ERR_DROP, "Sys_LoadDll on cgame failed" );
	}
	cls.state = CA_LOADING;

	// init for this gamestate
	// use the lastExecutedServerCommand instead of the serverCommandSequence
	// otherwise server commands sent just before a gamestate are dropped
	cgameExport->Init( clc.serverMessageSequence, clc.lastExecutedServerCommand );

	// we will send a usercmd this frame, which
	// will cause the server to send us the first snapshot
	cls.state = CA_PRIMED;

	t2 = Sys_Milliseconds();

	Com_Printf( "CL_InitCGame: %5.2f seconds\n", ( t2 - t1 ) / 1000.0 );

	// have the renderer touch all its images, so they are present
	// on the card even if the driver does deferred loading
	re.EndRegistration();

	// make sure everything is paged in
	if ( !Sys_LowPhysicalMemory() ) {
		Com_TouchMemory();
	}

	// clear anything that got printed
	Con_ClearNotify();

	// Ridah, update the memory usage file
	CL_UpdateLevelHunkUsage();
}


/*
====================
CL_GameCommand

See if the current console command is claimed by the cgame
====================
*/
bool CL_GameCommand( void ) {
	if ( !cgameExport ) {
		return false;
	}

	return cgameExport->ConsoleCommand();
}



/*
=====================
CL_CGameRendering
=====================
*/
void CL_CGameRendering( stereoFrame_t stereo ) {
	cgameExport->DrawActiveFrame( cl.serverTime, stereo, clc.demoplaying );

}


/*
=================
CL_AdjustTimeDelta

Adjust the clients view of server time.

We attempt to have cl.serverTime exactly equal to the server's view
of time plus the timeNudge, but with variable latencies over the
internet it will often need to drift a bit to match conditions.

Our ideal time would be to have the adjusted time approach, but not pass,
the very latest snapshot.

Adjustments are only made when a new snapshot arrives with a rational
latency, which keeps the adjustment process framerate independent and
prevents massive overadjustment during times of significant packet loss
or bursted delayed packets.
=================
*/

#define RESET_TIME  500

void CL_AdjustTimeDelta( void ) {
	int newDelta;
	int deltaDelta;

	cl.newSnapshots = false;

	// the delta never drifts when replaying a demo
	if ( clc.demoplaying ) {
		return;
	}

	newDelta = cl.snap.serverTime - cls.realtime;
	deltaDelta = abs( newDelta - cl.serverTimeDelta );

	if ( deltaDelta > RESET_TIME ) {
		cl.serverTimeDelta = newDelta;
		cl.oldServerTime = cl.snap.serverTime;  // FIXME: is this a problem for cgame?
		cl.serverTime = cl.snap.serverTime;
		if ( cl_showTimeDelta->integer ) {
			Com_Printf( "<RESET> " );
		}
	} else if ( deltaDelta > 100 ) {
		// fast adjust, cut the difference in half
		if ( cl_showTimeDelta->integer ) {
			Com_Printf( "<FAST> " );
		}
		cl.serverTimeDelta = ( cl.serverTimeDelta + newDelta ) >> 1;
	} else {
		// slow drift adjust, only move 1 or 2 msec

		// if any of the frames between this and the previous snapshot
		// had to be extrapolated, nudge our sense of time back a little
		// the granularity of +1 / -2 is too high for timescale modified frametimes
		if ( com_timescale->value == 0 || com_timescale->value == 1 ) {
			if ( cl.extrapolatedSnapshot ) {
				cl.extrapolatedSnapshot = false;
				cl.serverTimeDelta -= 2;
			} else {
				// otherwise, move our sense of time forward to minimize total latency
				cl.serverTimeDelta++;
			}
		}
	}

	if ( cl_showTimeDelta->integer ) {
		Com_Printf( "%i ", cl.serverTimeDelta );
	}
}


/*
==================
CL_FirstSnapshot
==================
*/
void CL_FirstSnapshot( void ) {
	// ignore snapshots that don't have entities
	if ( cl.snap.snapFlags & SNAPFLAG_NOT_ACTIVE ) {
		return;
	}
	cls.state = CA_ACTIVE;

	// set the timedelta so we are exactly on this first frame
	cl.serverTimeDelta = cl.snap.serverTime - cls.realtime;
	cl.oldServerTime = cl.snap.serverTime;

	clc.timeDemoBaseTime = cl.snap.serverTime;

	// if this is the first frame of active play,
	// execute the contents of activeAction now
	// this is to allow scripting a timedemo to start right
	// after loading
	if ( cl_activeAction->string[0] ) {
		Cbuf_AddText( cl_activeAction->string );
		Cvar_Set( "activeAction", "" );
	}

	Sys_BeginProfiling();
}

/*
==================
CL_SetCGameTime
==================
*/
void CL_SetCGameTime( void ) {
	// getting a valid frame message ends the connection process
	if ( cls.state != CA_ACTIVE ) {
		if ( cls.state != CA_PRIMED ) {
			return;
		}
		if ( clc.demoplaying ) {
			// we shouldn't get the first snapshot on the same frame
			// as the gamestate, because it causes a bad time skip
			if ( !clc.firstDemoFrameSkipped ) {
				clc.firstDemoFrameSkipped = true;
				return;
			}
			CL_ReadDemoMessage();
		}
		if ( cl.newSnapshots ) {
			cl.newSnapshots = false;
			CL_FirstSnapshot();
		}
		if ( cls.state != CA_ACTIVE ) {
			return;
		}
	}

	// if we have gotten to this point, cl.snap is guaranteed to be valid
	if ( !cl.snap.valid ) {
		Com_Error( ERR_DROP, "CL_SetCGameTime: !cl.snap.valid" );
	}

	// allow pause in single player
	if ( sv_paused->integer && cl_paused->integer && com_sv_running->integer ) {
		// paused
		return;
	}

	if ( cl.snap.serverTime < cl.oldFrameServerTime ) {
		// Ridah, if this is a localhost, then we are probably loading a savegame
		if ( !Q_stricmp( cls.servername, "localhost" ) ) {
			// do nothing?
			CL_FirstSnapshot();
		} else {
			Com_Error( ERR_DROP, "cl.snap.serverTime < cl.oldFrameServerTime" );
		}
	}
	cl.oldFrameServerTime = cl.snap.serverTime;


	// get our current view of time

	if ( clc.demoplaying && cl_freezeDemo->integer ) {
		// cl_freezeDemo is used to lock a demo in place for single frame advances

	} else {
		// cl_timeNudge is a user adjustable cvar that allows more
		// or less latency to be added in the interest of better
		// smoothness or better responsiveness.
		int tn;

		tn = cl_timeNudge->integer;
		if ( tn < -30 ) {
			tn = -30;
		} else if ( tn > 30 ) {
			tn = 30;
		}

		cl.serverTime = cls.realtime + cl.serverTimeDelta - tn;

		// guarantee that time will never flow backwards, even if
		// serverTimeDelta made an adjustment or cl_timeNudge was changed
		if ( cl.serverTime < cl.oldServerTime ) {
			cl.serverTime = cl.oldServerTime;
		}
		cl.oldServerTime = cl.serverTime;

		// note if we are almost past the latest frame (without timeNudge),
		// so we will try to adjust back a bit when the next snapshot arrives
		if ( cls.realtime + cl.serverTimeDelta >= cl.snap.serverTime - 5 ) {
			cl.extrapolatedSnapshot = true;
		}
	}

	// if we have gotten new snapshots, drift serverTimeDelta
	// don't do this every frame, or a period of packet loss would
	// make a huge adjustment
	if ( cl.newSnapshots ) {
		CL_AdjustTimeDelta();
	}

	if ( !clc.demoplaying ) {
		return;
	}

	// if we are playing a demo back, we can just keep reading
	// messages from the demo file until the cgame definately
	// has valid snapshots to interpolate between

	// a timedemo will always use a deterministic set of time samples
	// no matter what speed machine it is run on,
	// while a normal demo may have different time samples
	// each time it is played back
	if ( cl_timedemo->integer ) {
		if ( !clc.timeDemoStart ) {
			clc.timeDemoStart = Sys_Milliseconds();
		}
		clc.timeDemoFrames++;
		cl.serverTime = clc.timeDemoBaseTime + clc.timeDemoFrames * 50;
	}

	while ( cl.serverTime >= cl.snap.serverTime ) {
		// feed another messag, which should change
		// the contents of cl.snap
		CL_ReadDemoMessage();
		if ( cls.state != CA_ACTIVE ) {
			return;     // end of demo
		}
	}

}

/*
====================
CL_GetTag
====================
*/
bool CL_GetTag( int clientNum, char *tagname, orientation_t *orient ) {
	if ( !cgameExport ) {
		return false;
	}

	return cgameExport->GetTag( clientNum, tagname, orient );
}



