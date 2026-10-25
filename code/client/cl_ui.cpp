/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company. 

This file is part of the Return to Castle Wolfenstein single player GPL Source Code (RTCW SP Source Code).  

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

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/


#include "client.h"

#include "../game/botlib.h"

extern botlib_export_t *botlib_export;

uiExport_t *uiExport = NULL;
static void *uiDllHandle = NULL;

extern char cl_cdkey[34];


/*
====================
GetClientState
====================
*/
static void GetClientState( uiClientState_t *state ) {
	state->connectPacketCount = clc.connectPacketCount;
	state->connState = cls.state;
	Q_strncpyz( state->servername, cls.servername, sizeof( state->servername ) );
	Q_strncpyz( state->updateInfoString, cls.updateInfoString, sizeof( state->updateInfoString ) );
	Q_strncpyz( state->messageString, clc.serverMessage, sizeof( state->messageString ) );
	state->clientNum = cl.snap.ps.clientNum;
}

/*
====================
LAN_LoadCachedServers
====================
*/
void LAN_LoadCachedServers() {
	// TTimo: stub, this is only relevant to MP, SP kills the servercache.dat (and favorites)
	// show_bug.cgi?id=445
	/*
	  int size;
	  fileHandle_t fileIn;
	  cls.numglobalservers = cls.nummplayerservers = cls.numfavoriteservers = 0;
	  cls.numGlobalServerAddresses = 0;
	  if (FS_SV_FOpenFileRead("servercache.dat", &fileIn)) {
		  FS_Read(&cls.numglobalservers, sizeof(int), fileIn);
		  FS_Read(&cls.nummplayerservers, sizeof(int), fileIn);
		  FS_Read(&cls.numfavoriteservers, sizeof(int), fileIn);
		  FS_Read(&size, sizeof(int), fileIn);
		  if (size == sizeof(cls.globalServers) + sizeof(cls.favoriteServers) + sizeof(cls.mplayerServers)) {
			  FS_Read(&cls.globalServers, sizeof(cls.globalServers), fileIn);
			  FS_Read(&cls.mplayerServers, sizeof(cls.mplayerServers), fileIn);
			  FS_Read(&cls.favoriteServers, sizeof(cls.favoriteServers), fileIn);
		  } else {
			  cls.numglobalservers = cls.nummplayerservers = cls.numfavoriteservers = 0;
			  cls.numGlobalServerAddresses = 0;
		  }
		  FS_FCloseFile(fileIn);
	  }
	*/
}

/*
====================
LAN_SaveServersToCache
====================
*/
void LAN_SaveServersToCache() {
	// TTimo: stub, this is only relevant to MP, SP kills the servercache.dat (and favorites)
	// show_bug.cgi?id=445
	/*
	  int size;
	  fileHandle_t fileOut;
  #ifdef __MACOS__	//DAJ MacOS file typing
	  {
		  extern _MSL_IMP_EXP_C long _fcreator, _ftype;
		  _ftype = 'WlfB';
		  _fcreator = 'WlfS';
	  }
  #endif
	  fileOut = FS_SV_FOpenFileWrite("servercache.dat");
	  FS_Write(&cls.numglobalservers, sizeof(int), fileOut);
	  FS_Write(&cls.nummplayerservers, sizeof(int), fileOut);
	  FS_Write(&cls.numfavoriteservers, sizeof(int), fileOut);
	  size = sizeof(cls.globalServers) + sizeof(cls.favoriteServers) + sizeof(cls.mplayerServers);
	  FS_Write(&size, sizeof(int), fileOut);
	  FS_Write(&cls.globalServers, sizeof(cls.globalServers), fileOut);
	  FS_Write(&cls.mplayerServers, sizeof(cls.mplayerServers), fileOut);
	  FS_Write(&cls.favoriteServers, sizeof(cls.favoriteServers), fileOut);
	  FS_FCloseFile(fileOut);
	*/
}


/*
====================
LAN_ResetPings
====================
*/
static void LAN_ResetPings( int source ) {
	int count,i;
	serverInfo_t *servers = NULL;
	count = 0;

	switch ( source ) {
	case AS_LOCAL:
		servers = &cls.localServers[0];
		count = MAX_OTHER_SERVERS;
		break;
	case AS_MPLAYER:
		servers = &cls.mplayerServers[0];
		count = MAX_OTHER_SERVERS;
		break;
	case AS_GLOBAL:
		servers = &cls.globalServers[0];
		count = MAX_GLOBAL_SERVERS;
		break;
	case AS_FAVORITES:
		servers = &cls.favoriteServers[0];
		count = MAX_OTHER_SERVERS;
		break;
	}
	if ( servers ) {
		for ( i = 0; i < count; i++ ) {
			servers[i].ping = -1;
		}
	}
}

/*
====================
LAN_AddServer
====================
*/
static int LAN_AddServer( int source, const char *name, const char *address ) {
	int max, *count, i;
	netadr_t adr;
	serverInfo_t *servers = NULL;
	max = MAX_OTHER_SERVERS;
	count = 0;

	switch ( source ) {
	case AS_LOCAL:
		count = &cls.numlocalservers;
		servers = &cls.localServers[0];
		break;
	case AS_MPLAYER:
		count = &cls.nummplayerservers;
		servers = &cls.mplayerServers[0];
		break;
	case AS_GLOBAL:
		max = MAX_GLOBAL_SERVERS;
		count = &cls.numglobalservers;
		servers = &cls.globalServers[0];
		break;
	case AS_FAVORITES:
		count = &cls.numfavoriteservers;
		servers = &cls.favoriteServers[0];
		break;
	}
	if ( servers && *count < max ) {
		NET_StringToAdr( address, &adr );
		for ( i = 0; i < *count; i++ ) {
			if ( NET_CompareAdr( servers[i].adr, adr ) ) {
				break;
			}
		}
		if ( i >= *count ) {
			servers[*count].adr = adr;
			Q_strncpyz( servers[*count].hostName, name, sizeof( servers[*count].hostName ) );
			servers[*count].visible = true;
			( *count )++;
			return 1;
		}
		return 0;
	}
	return -1;
}

/*
====================
LAN_RemoveServer
====================
*/
static void LAN_RemoveServer( int source, const char *addr ) {
	int *count, i;
	serverInfo_t *servers = NULL;
	count = 0;
	switch ( source ) {
	case AS_LOCAL:
		count = &cls.numlocalservers;
		servers = &cls.localServers[0];
		break;
	case AS_MPLAYER:
		count = &cls.nummplayerservers;
		servers = &cls.mplayerServers[0];
		break;
	case AS_GLOBAL:
		count = &cls.numglobalservers;
		servers = &cls.globalServers[0];
		break;
	case AS_FAVORITES:
		count = &cls.numfavoriteservers;
		servers = &cls.favoriteServers[0];
		break;
	}
	if ( servers ) {
		netadr_t comp;
		NET_StringToAdr( addr, &comp );
		for ( i = 0; i < *count; i++ ) {
			if ( NET_CompareAdr( comp, servers[i].adr ) ) {
				int j = i;
				while ( j < *count - 1 ) {
					Com_Memcpy( &servers[j], &servers[j + 1], sizeof( servers[j] ) );
					j++;
				}
				( *count )--;
				break;
			}
		}
	}
}


/*
====================
LAN_GetServerCount
====================
*/
static int LAN_GetServerCount( int source ) {
	switch ( source ) {
	case AS_LOCAL:
		return cls.numlocalservers;
		break;
	case AS_MPLAYER:
		return cls.nummplayerservers;
		break;
	case AS_GLOBAL:
		return cls.numglobalservers;
		break;
	case AS_FAVORITES:
		return cls.numfavoriteservers;
		break;
	}
	return 0;
}

/*
====================
LAN_GetLocalServerAddressString
====================
*/
static void LAN_GetServerAddressString( int source, int n, char *buf, int buflen ) {
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			Q_strncpyz( buf, NET_AdrToString( cls.localServers[n].adr ), buflen );
			return;
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			Q_strncpyz( buf, NET_AdrToString( cls.mplayerServers[n].adr ), buflen );
			return;
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			Q_strncpyz( buf, NET_AdrToString( cls.globalServers[n].adr ), buflen );
			return;
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			Q_strncpyz( buf, NET_AdrToString( cls.favoriteServers[n].adr ), buflen );
			return;
		}
		break;
	}
	buf[0] = '\0';
}

/*
====================
LAN_GetServerInfo
====================
*/
static void LAN_GetServerInfo( int source, int n, char *buf, int buflen ) {
	char info[MAX_STRING_CHARS];
	serverInfo_t *server = NULL;
	info[0] = '\0';
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.localServers[n];
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.mplayerServers[n];
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			server = &cls.globalServers[n];
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.favoriteServers[n];
		}
		break;
	}
	if ( server && buf ) {
		buf[0] = '\0';
		Info_SetValueForKey( info, "hostname", server->hostName );
		Info_SetValueForKey( info, "mapname", server->mapName );
		Info_SetValueForKey( info, "clients", va( "%i",server->clients ) );
		Info_SetValueForKey( info, "sv_maxclients", va( "%i",server->maxClients ) );
		Info_SetValueForKey( info, "ping", va( "%i",server->ping ) );
		Info_SetValueForKey( info, "minping", va( "%i",server->minPing ) );
		Info_SetValueForKey( info, "maxping", va( "%i",server->maxPing ) );
		Info_SetValueForKey( info, "game", server->game );
		Info_SetValueForKey( info, "gametype", va( "%i",server->gameType ) );
		Info_SetValueForKey( info, "nettype", va( "%i",server->netType ) );
		Info_SetValueForKey( info, "addr", NET_AdrToString( server->adr ) );
		Info_SetValueForKey( info, "sv_allowAnonymous", va( "%i", server->allowAnonymous ) );
		Q_strncpyz( buf, info, buflen );
	} else {
		if ( buf ) {
			buf[0] = '\0';
		}
	}
}

/*
====================
LAN_GetServerPing
====================
*/
static int LAN_GetServerPing( int source, int n ) {
	serverInfo_t *server = NULL;
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.localServers[n];
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.mplayerServers[n];
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			server = &cls.globalServers[n];
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			server = &cls.favoriteServers[n];
		}
		break;
	}
	if ( server ) {
		return server->ping;
	}
	return -1;
}

/*
====================
LAN_GetServerPtr
====================
*/
static serverInfo_t *LAN_GetServerPtr( int source, int n ) {
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return &cls.localServers[n];
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return &cls.mplayerServers[n];
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			return &cls.globalServers[n];
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return &cls.favoriteServers[n];
		}
		break;
	}
	return NULL;
}

/*
====================
LAN_CompareServers
====================
*/
static int LAN_CompareServers( int source, int sortKey, int sortDir, int s1, int s2 ) {
	int res;
	serverInfo_t *server1, *server2;

	server1 = LAN_GetServerPtr( source, s1 );
	server2 = LAN_GetServerPtr( source, s2 );
	if ( !server1 || !server2 ) {
		return 0;
	}

	res = 0;
	switch ( sortKey ) {
	case SORT_HOST:
		res = Q_stricmp( server1->hostName, server2->hostName );
		break;

	case SORT_MAP:
		res = Q_stricmp( server1->mapName, server2->mapName );
		break;
	case SORT_CLIENTS:
		if ( server1->clients < server2->clients ) {
			res = -1;
		} else if ( server1->clients > server2->clients )     {
			res = 1;
		} else {
			res = 0;
		}
		break;
	case SORT_GAME:
		if ( server1->gameType < server2->gameType ) {
			res = -1;
		} else if ( server1->gameType > server2->gameType )     {
			res = 1;
		} else {
			res = 0;
		}
		break;
	case SORT_PING:
		if ( server1->ping < server2->ping ) {
			res = -1;
		} else if ( server1->ping > server2->ping )     {
			res = 1;
		} else {
			res = 0;
		}
		break;
	}

	if ( sortDir ) {
		if ( res < 0 ) {
			return 1;
		}
		if ( res > 0 ) {
			return -1;
		}
		return 0;
	}
	return res;
}

/*
====================
LAN_GetPingQueueCount
====================
*/
static int LAN_GetPingQueueCount( void ) {
	return ( CL_GetPingQueueCount() );
}

/*
====================
LAN_ClearPing
====================
*/
static void LAN_ClearPing( int n ) {
	CL_ClearPing( n );
}

/*
====================
LAN_GetPing
====================
*/
static void LAN_GetPing( int n, char *buf, int buflen, int *pingtime ) {
	CL_GetPing( n, buf, buflen, pingtime );
}

/*
====================
LAN_GetPingInfo
====================
*/
static void LAN_GetPingInfo( int n, char *buf, int buflen ) {
	CL_GetPingInfo( n, buf, buflen );
}

/*
====================
LAN_MarkServerVisible
====================
*/
static void LAN_MarkServerVisible( int source, int n, bool visible ) {
	if ( n == -1 ) {
		int count = MAX_OTHER_SERVERS;
		serverInfo_t *server = NULL;
		switch ( source ) {
		case AS_LOCAL:
			server = &cls.localServers[0];
			break;
		case AS_MPLAYER:
			server = &cls.mplayerServers[0];
			break;
		case AS_GLOBAL:
			server = &cls.globalServers[0];
			count = MAX_GLOBAL_SERVERS;
			break;
		case AS_FAVORITES:
			server = &cls.favoriteServers[0];
			break;
		}
		if ( server ) {
			for ( n = 0; n < count; n++ ) {
				server[n].visible = visible;
			}
		}

	} else {
		switch ( source ) {
		case AS_LOCAL:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				cls.localServers[n].visible = visible;
			}
			break;
		case AS_MPLAYER:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				cls.mplayerServers[n].visible = visible;
			}
			break;
		case AS_GLOBAL:
			if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
				cls.globalServers[n].visible = visible;
			}
			break;
		case AS_FAVORITES:
			if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
				cls.favoriteServers[n].visible = visible;
			}
			break;
		}
	}
}


/*
=======================
LAN_ServerIsVisible
=======================
*/
static int LAN_ServerIsVisible( int source, int n ) {
	switch ( source ) {
	case AS_LOCAL:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return cls.localServers[n].visible;
		}
		break;
	case AS_MPLAYER:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return cls.mplayerServers[n].visible;
		}
		break;
	case AS_GLOBAL:
		if ( n >= 0 && n < MAX_GLOBAL_SERVERS ) {
			return cls.globalServers[n].visible;
		}
		break;
	case AS_FAVORITES:
		if ( n >= 0 && n < MAX_OTHER_SERVERS ) {
			return cls.favoriteServers[n].visible;
		}
		break;
	}
	return false;
}

/*
=======================
LAN_UpdateVisiblePings
=======================
*/
bool LAN_UpdateVisiblePings( int source ) {
	return CL_UpdateVisiblePings_f( source );
}

/*
====================
LAN_GetServerStatus
====================
*/
int LAN_GetServerStatus( char *serverAddress, char *serverStatus, int maxLen ) {
	return CL_ServerStatus( serverAddress, serverStatus, maxLen );
}

/*
====================
CL_GetGlConfig
====================
*/
static void CL_GetGlconfig( glconfig_t *config ) {
	*config = cls.glconfig;
}

/*
====================
GetClipboardData
====================
*/
static void GetClipboardData( char *buf, int buflen ) {
	char    *cbd;

	cbd = Sys_GetClipboardData();

	if ( !cbd ) {
		*buf = 0;
		return;
	}

	Q_strncpyz( buf, cbd, buflen );

	Z_Free( cbd );
}

/*
====================
Key_KeynumToStringBuf
====================
*/
static void Key_KeynumToStringBuf( int keynum, char *buf, int buflen ) {
	Q_strncpyz( buf, Key_KeynumToString( keynum, true ), buflen );
}

/*
====================
Key_GetBindingBuf
====================
*/
static void Key_GetBindingBuf( int keynum, char *buf, int buflen ) {
	char    *value;

	value = Key_GetBinding( keynum );
	if ( value ) {
		Q_strncpyz( buf, value, buflen );
	} else {
		*buf = 0;
	}
}

/*
====================
Key_GetCatcher
====================
*/
int Key_GetCatcher( void ) {
	return cls.keyCatchers;
}

/*
====================
Ket_SetCatcher
====================
*/
void Key_SetCatcher( int catcher ) {
	cls.keyCatchers = catcher;
}


/*
====================
CLUI_GetCDKey
====================
*/
static void CLUI_GetCDKey( char *buf, int buflen ) {
	cvar_t  *fs;
	fs = Cvar_Get( "fs_game", "", CVAR_INIT | CVAR_SYSTEMINFO );
	if ( UI_usesUniqueCDKey() && fs && fs->string[0] != 0 ) {
		memcpy( buf, &cl_cdkey[16], 16 );
		buf[16] = 0;
	} else {
		memcpy( buf, cl_cdkey, 16 );
		buf[16] = 0;
	}
}


/*
====================
CLUI_SetCDKey
====================
*/
static void CLUI_SetCDKey( char *buf ) {
	cvar_t  *fs;
	fs = Cvar_Get( "fs_game", "", CVAR_INIT | CVAR_SYSTEMINFO );
	if ( UI_usesUniqueCDKey() && fs && fs->string[0] != 0 ) {
		memcpy( &cl_cdkey[16], buf, 16 );
		cl_cdkey[32] = 0;
		// set the flag so the fle will be written at the next opportunity
		cvar_modifiedFlags |= CVAR_ARCHIVE;
	} else {
		memcpy( cl_cdkey, buf, 16 );
		// set the flag so the fle will be written at the next opportunity
		cvar_modifiedFlags |= CVAR_ARCHIVE;
	}
}


/*
====================
GetConfigString
====================
*/
static int GetConfigString( int index, char *buf, int size ) {
	int offset;

	if ( index < 0 || index >= MAX_CONFIGSTRINGS ) {
		return false;
	}

	offset = cl.gameState.stringOffsets[index];
	if ( !offset ) {
		if ( size ) {
			buf[0] = 0;
		}
		return false;
	}

	Q_strncpyz( buf, cl.gameState.stringData + offset, size );

	return true;
}

// The import table lists the engine functions directly. Inline lambdas only
// adapt the signatures that differ from what the ui module expects.
static const uiImport_t uiImportTable = {
	UI_API_VERSION,

	// Com_Printf and Com_Error are variadic, the table takes plain strings
	[]( const char *msg ) { Com_Printf( "%s", msg ); },
	[]( const char *msg ) { Com_Error( ERR_DROP, "%s", msg ); },
	Sys_Milliseconds,
	Cvar_Register,
	Cvar_Update,
	Cvar_Set,
	Cvar_VariableValue,
	Cvar_VariableStringBuffer,
	Cvar_SetValue,
	Cvar_Reset,
	// the engine creator returns the cvar, the module does not want it
	[]( const char *varName, const char *varValue, int flags ) { Cvar_Get( varName, varValue, flags ); },
	Cvar_InfoStringBuffer,
	Cmd_Argc,
	Cmd_ArgvBuffer,
	Cbuf_ExecuteText,
	FS_FOpenFileByMode,
	[]( void *buffer, int len, fileHandle_t f ) { FS_Read( buffer, len, f ); },
	[]( fileHandle_t f, long offset, int origin ) { FS_Seek( f, offset, origin ); },
	[]( const void *buffer, int len, fileHandle_t f ) { FS_Write( buffer, len, f ); },
	FS_FCloseFile,
	FS_GetFileList,
	[]( const char *filename ) { return FS_Delete( (char *)filename ); },
	[]( const char *name ) { return re.RegisterModel( name ); },
	[]( const char *name ) { return re.RegisterSkin( name ); },
	[]( const char *fontName, int pointSize, fontInfo_t *font ) { re.RegisterFont( fontName, pointSize, font ); },
	[]( const char *name ) { return re.RegisterShaderNoMip( name ); },
	[]() { re.ClearScene(); },
	[]( const refEntity_t *rentity ) { re.AddRefEntityToScene( rentity ); },
	[]( qhandle_t hShader, int numVerts, const polyVert_t *verts ) { re.AddPolyToScene( hShader, numVerts, verts ); },
	[]( const vec3_t org, float intensity, float r, float g, float b, int overdraw ) { re.AddLightToScene( org, intensity, r, g, b, overdraw ); },
	[]( const vec3_t org, float r, float g, float b, float scale, int id, int flags ) { re.AddCoronaToScene( org, r, g, b, scale, id, flags ); },
	[]( const refdef_t *fd ) { re.RenderScene( fd ); },
	[]( const float *rgba ) { re.SetColor( rgba ); },
	[]( float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader ) { re.DrawStretchPic( x, y, w, h, s1, t1, s2, t2, hShader ); },
	[]( clipHandle_t model, vec3_t mins, vec3_t maxs ) { re.ModelBounds( model, mins, maxs ); },
	SCR_UpdateScreen,
	[]( orientation_t *tag, const refEntity_t *refent, const char *tagName, int startIndex ) { return re.LerpTag( tag, refent, tagName, startIndex ); },
	S_StartLocalSound,
	// DOOMSOUND skips the loading flag the engine variant takes
	[]( const char *sample ) {
#ifdef DOOMSOUND
		return S_RegisterSound( sample );
#else
		return S_RegisterSound( sample, false );
#endif
	},
	[]( float targetvol, int time, int num ) { S_FadeStreamingSound( targetvol, time, num ); },
	[]( float targetvol, int time ) { S_FadeAllSounds( targetvol, time ); },
	Key_KeynumToStringBuf,
	Key_GetBindingBuf,
	Key_SetBinding,
	Key_IsDown,
	Key_GetOverstrikeMode,
	Key_SetOverstrikeMode,
	Key_ClearStates,
	Key_GetCatcher,
	Key_SetCatcher,
	GetClipboardData,
	GetClientState,
	CL_GetGlconfig,
	GetConfigString,
	LAN_GetPingQueueCount,
	LAN_ClearPing,
	LAN_GetPing,
	LAN_GetPingInfo,
	LAN_UpdateVisiblePings,
	LAN_GetServerCount,
	LAN_CompareServers,
	LAN_GetServerAddressString,
	LAN_GetServerInfo,
	LAN_AddServer,
	LAN_RemoveServer,
	LAN_GetServerPing,
	LAN_ServerIsVisible,
	[]( const char *serverAddress, char *serverStatus, int maxLen ) { return LAN_GetServerStatus( (char *)serverAddress, serverStatus, maxLen ); },
	[]() { LAN_SaveServersToCache(); },
	LAN_LoadCachedServers,
	LAN_MarkServerVisible,
	LAN_ResetPings,
	Hunk_MemoryRemaining,
	CLUI_GetCDKey,
	CLUI_SetCDKey,
	[]( char *define ) { return botlib_export->PC_AddGlobalDefine( define ); },
	[]( const char *filename ) { return botlib_export->PC_LoadSourceHandle( filename ); },
	[]( int handle ) { return botlib_export->PC_FreeSourceHandle( handle ); },
	[]( int handle, pc_token_t *token ) { return botlib_export->PC_ReadTokenHandle( handle, token ); },
	[]( int handle, char *filename, int *line ) { return botlib_export->PC_SourceFileAndLine( handle, filename, line ); },
	S_StopBackgroundTrack,
	S_StartBackgroundTrack,
	Com_RealTime,
	[]( const char *arg0, int xpos, int ypos, int width, int height, int bits ) {
		Com_DPrintf( "UI_CIN_PlayCinematic\n" );
		return CIN_PlayCinematic( arg0, xpos, ypos, width, height, bits );
	},
	CIN_StopCinematic,
	CIN_RunCinematic,
	CIN_DrawCinematic,
	CIN_SetExtents,
	[]( const char *oldShader, const char *newShader, const char *timeOffset ) { re.RemapShader( oldShader, newShader, timeOffset ); },
	CL_CDKeyValidate,
	CL_GetLimboString,
};

/*
====================
CL_ShutdownUI
====================
*/
void CL_ShutdownUI( void ) {
	cls.keyCatchers &= ~KEYCATCH_UI;
	cls.uiStarted = false;
	if ( !uiExport ) {
		return;
	}
	uiExport->Shutdown();
	Sys_UnloadDll( uiDllHandle );
	uiDllHandle = NULL;
	uiExport = NULL;
}

/*
====================
CL_InitUI
====================
*/

void CL_InitUI( void ) {
	// load the ui DLL
	uiExport = (uiExport_t *)Sys_LoadDll( "ui", &uiDllHandle, "GetUIAPI", (void *)&uiImportTable );
	if ( !uiExport ) {
		Com_Error( ERR_FATAL, "Sys_LoadDll on UI failed" );
	}

	// sanity check
	if ( uiExport->apiVersion != UI_API_VERSION ) {
		Com_Error( ERR_FATAL, "User Interface is version %d, expected %d", uiExport->apiVersion, UI_API_VERSION );
		cls.uiStarted = false;
	}

	// init for this gamestate
	uiExport->Init( ( cls.state >= CA_AUTHORIZING && cls.state < CA_ACTIVE ) );
}


bool UI_usesUniqueCDKey() {
	if ( uiExport ) {
		return ( uiExport->HasUniqueCDKey() == true );
	} else {
		return false;
	}
}

/*
====================
UI_GameCommand

See if the current console command is claimed by the ui
====================
*/
bool UI_GameCommand( void ) {
	if ( !uiExport ) {
		return false;
	}

	return uiExport->ConsoleCommand( cls.realtime );
}
