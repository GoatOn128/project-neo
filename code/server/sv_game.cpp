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

// sv_game.cpp -- interface to the game dll

#include "server.h"

#include "../game/botlib.h"

botlib_export_t *botlib_export;

void SV_GameError( const char *string ) {
	Com_Error( ERR_DROP, "%s", string );
}

void SV_GamePrint( const char *string ) {
	Com_Printf( "%s", string );
}

// these functions must be used instead of pointer arithmetic, because
// the game allocates gentities with private information after the server shared part
int SV_NumForGentity( sharedEntity_t *ent ) {
	int num;

	num = ( (byte *)ent - (byte *)sv.gentities ) / sv.gentitySize;

	return num;
}

sharedEntity_t *SV_GentityNum( int num ) {
	sharedEntity_t *ent;

	ent = ( sharedEntity_t * )( (byte *)sv.gentities + sv.gentitySize * ( num ) );

	return ent;
}

playerState_t *SV_GameClientNum( int num ) {
	playerState_t   *ps;

	ps = ( playerState_t * )( (byte *)sv.gameClients + sv.gameClientSize * ( num ) );

	return ps;
}

svEntity_t  *SV_SvEntityForGentity( sharedEntity_t *gEnt ) {
	if ( !gEnt || gEnt->s.number < 0 || gEnt->s.number >= MAX_GENTITIES ) {
		Com_Error( ERR_DROP, "SV_SvEntityForGentity: bad gEnt" );
	}
	return &sv.svEntities[ gEnt->s.number ];
}

sharedEntity_t *SV_GEntityForSvEntity( svEntity_t *svEnt ) {
	int num;

	num = svEnt - sv.svEntities;
	return SV_GentityNum( num );
}

/*
===============
SV_GameSendServerCommand

Sends a command string to a client
===============
*/
void SV_GameSendServerCommand( int clientNum, const char *text ) {
	if ( clientNum == -1 ) {
		SV_SendServerCommand( NULL, "%s", text );
	} else {
		if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
			return;
		}
		SV_SendServerCommand( svs.clients + clientNum, "%s", text );
	}
}


/*
===============
SV_GameDropClient

Disconnects the client with a message
===============
*/
void SV_GameDropClient( int clientNum, const char *reason ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		return;
	}
	SV_DropClient( svs.clients + clientNum, reason );
}


/*
=================
SV_SetBrushModel

sets mins and maxs for inline bmodels
=================
*/
void SV_SetBrushModel( sharedEntity_t *ent, const char *name ) {
	clipHandle_t h;
	vec3_t mins, maxs;

	if ( !name ) {
		Com_Error( ERR_DROP, "SV_SetBrushModel: NULL" );
	}

	if ( name[0] != '*' ) {
		Com_Error( ERR_DROP, "SV_SetBrushModel: %s isn't a brush model", name );
	}


	ent->s.modelindex = atoi( name + 1 );

	h = CM_InlineModel( ent->s.modelindex );
	CM_ModelBounds( h, mins, maxs );
	VectorCopy( mins, ent->r.mins );
	VectorCopy( maxs, ent->r.maxs );
	ent->r.bmodel = qtrue;

	ent->r.contents = -1;       // we don't know exactly what is in the brushes

	SV_LinkEntity( ent );       // FIXME: remove
}



/*
=================
SV_inPVS

Also checks portalareas so that doors block sight
=================
*/
qboolean SV_inPVS( const vec3_t p1, const vec3_t p2 ) {
	int leafnum;
	int cluster;
	int area1, area2;
	byte    *mask;

	leafnum = CM_PointLeafnum( p1 );
	cluster = CM_LeafCluster( leafnum );
	area1 = CM_LeafArea( leafnum );
	mask = CM_ClusterPVS( cluster );

	leafnum = CM_PointLeafnum( p2 );
	cluster = CM_LeafCluster( leafnum );
	area2 = CM_LeafArea( leafnum );
	if ( mask && ( !( mask[cluster >> 3] & ( 1 << ( cluster & 7 ) ) ) ) ) {
		return qfalse;
	}
	if ( !CM_AreasConnected( area1, area2 ) ) {
		return qfalse;      // a door blocks sight
	}
	return qtrue;
}


/*
=================
SV_inPVSIgnorePortals

Does NOT check portalareas
=================
*/
qboolean SV_inPVSIgnorePortals( const vec3_t p1, const vec3_t p2 ) {
	int leafnum;
	int cluster;
	byte    *mask;

	leafnum = CM_PointLeafnum( p1 );
	cluster = CM_LeafCluster( leafnum );
	mask = CM_ClusterPVS( cluster );

	leafnum = CM_PointLeafnum( p2 );
	cluster = CM_LeafCluster( leafnum );

	if ( mask && ( !( mask[cluster >> 3] & ( 1 << ( cluster & 7 ) ) ) ) ) {
		return qfalse;
	}

	return qtrue;
}


/*
========================
SV_AdjustAreaPortalState
========================
*/
void SV_AdjustAreaPortalState( sharedEntity_t *ent, qboolean open ) {
	svEntity_t  *svEnt;

	svEnt = SV_SvEntityForGentity( ent );
	if ( svEnt->areanum2 == -1 ) {
		return;
	}
	CM_AdjustAreaPortalState( svEnt->areanum, svEnt->areanum2, open );
}


/*
==================
SV_GameAreaEntities
==================
*/
qboolean    SV_EntityContact( const vec3_t mins, const vec3_t maxs, const sharedEntity_t *gEnt, const int capsule ) {
	const float *origin, *angles;
	clipHandle_t ch;
	trace_t trace;

	// check for exact collision
	origin = gEnt->r.currentOrigin;
	angles = gEnt->r.currentAngles;

	ch = SV_ClipHandleForEntity( gEnt );
	CM_TransformedBoxTrace( &trace, vec3_origin, vec3_origin, mins, maxs,
							ch, -1, origin, angles, capsule );

	return trace.startsolid;
}


/*
===============
SV_GetServerinfo

===============
*/
void SV_GetServerinfo( char *buffer, int bufferSize ) {
	if ( bufferSize < 1 ) {
		Com_Error( ERR_DROP, "SV_GetServerinfo: bufferSize == %i", bufferSize );
	}
	Q_strncpyz( buffer, Cvar_InfoString( CVAR_SERVERINFO ), bufferSize );
}

/*
===============
SV_LocateGameData

===============
*/
void SV_LocateGameData( sharedEntity_t *gEnts, int numGEntities, int sizeofGEntity_t,
						playerState_t *clients, int sizeofGameClient ) {
	sv.gentities = gEnts;
	sv.gentitySize = sizeofGEntity_t;
	sv.num_entities = numGEntities;

	sv.gameClients = clients;
	sv.gameClientSize = sizeofGameClient;
}


/*
===============
SV_GetUsercmd

===============
*/
void SV_GetUsercmd( int clientNum, usercmd_t *cmd ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		Com_Error( ERR_DROP, "SV_GetUsercmd: bad clientNum:%i", clientNum );
	}
	*cmd = svs.clients[clientNum].lastUsercmd;
}


// The import table lists the engine functions directly. Inline lambdas only
// adapt the signatures that differ from what the game module expects.
static const gameImport_t gameImportTable = {
	GAME_API_VERSION,

	// Com_Printf and Com_Error are variadic, the table takes plain strings
	[]( const char *msg ) { Com_Printf( "%s", msg ); },
	[]( const char *msg ) { Com_Error( ERR_DROP, "%s", msg ); },
	[]() { Com_Error( ERR_ENDGAME, "endgame" ); }, // no message, no error print
	Sys_Milliseconds,
	Cmd_Argc,
	Cmd_ArgvBuffer,
	FS_FOpenFileByMode,
	[]( void *buffer, int len, fileHandle_t f ) { FS_Read( buffer, len, f ); },
	FS_Write,
	[]( const char *from, const char *to ) { FS_Rename( from, to ); return 0; },
	FS_FCloseFile,
	FS_CopyFileOS,
	FS_GetFileList,
	Cbuf_ExecuteText,
	Cvar_Register,
	Cvar_Update,
	Cvar_Set,
	Cvar_VariableIntegerValue,
	Cvar_VariableStringBuffer,
	[]( void *gEnts, int numGEntities, int sizeofGEntity_t, playerState_t *clients, int sizeofGameClient ) { SV_LocateGameData( (sharedEntity_t *)gEnts, numGEntities, sizeofGEntity_t, clients, sizeofGameClient ); },
	SV_GameDropClient,
	SV_GameSendServerCommand,
	SV_SetConfigstring,
	SV_GetConfigstring,
	SV_GetUserinfo,
	SV_SetUserinfo,
	SV_GetServerinfo,
	[]( void *ent, const char *name ) { SV_SetBrushModel( (sharedEntity_t *)ent, name ); },
	[]( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) { SV_Trace( results, start, mins, maxs, end, passEntityNum, contentmask, qfalse ); },
	[]( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) { SV_Trace( results, start, mins, maxs, end, passEntityNum, contentmask, qtrue ); },
	SV_PointContents,
	SV_inPVS,
	SV_inPVSIgnorePortals,
	[]( void *ent, qboolean open ) { SV_AdjustAreaPortalState( (sharedEntity_t *)ent, open ); },
	CM_AreasConnected,
	[]( void *ent ) { SV_LinkEntity( (sharedEntity_t *)ent ); },
	[]( void *ent ) { SV_UnlinkEntity( (sharedEntity_t *)ent ); },
	SV_AreaEntities,
	[]( const vec3_t mins, const vec3_t maxs, const void *ent ) { return SV_EntityContact( mins, maxs, (const sharedEntity_t *)ent, qfalse ); },
	[]( const vec3_t mins, const vec3_t maxs, const void *ent ) { return SV_EntityContact( mins, maxs, (const sharedEntity_t *)ent, qtrue ); },
	SV_BotAllocateClient,
	SV_BotFreeClient,
	SV_GetUsercmd,
	// the enum literals need casts so the lambda deduces qboolean, not the enum type
	[]( char *buffer, int bufferSize ) {
		const char *s = COM_Parse( &sv.entityParsePoint );
		Q_strncpyz( buffer, s, bufferSize );
		if ( !sv.entityParsePoint && !s[0] ) {
			return (qboolean)qfalse;
		}
		return (qboolean)qtrue;
	},
	BotImport_DebugPolygonCreate,
	BotImport_DebugPolygonDelete,
	Com_RealTime,
	[]( float *v ) { Sys_SnapVector( v ); },
	SV_GetTag,
	SV_BotLibSetup,
	SV_BotLibShutdown,
	[]( char *var_name, char *value ) { return botlib_export->BotLibVarSet( var_name, value ); },
	[]( char *var_name, char *value, int size ) { return botlib_export->BotLibVarGet( var_name, value, size ); },
	[]( char *string ) { return botlib_export->PC_AddGlobalDefine( string ); },
	[]( float time ) { return botlib_export->BotLibStartFrame( time ); },
	[]( const char *mapname ) { return botlib_export->BotLibLoadMap( mapname ); },
	[]( int ent, void *bue ) { return botlib_export->BotLibUpdateEntity( ent, (bot_entitystate_t *)bue ); },
	[]( int parm0, char *parm1, vec3_t parm2, vec3_t parm3 ) { return botlib_export->Test( parm0, parm1, (vec_t *)parm2, (vec_t *)parm3 ); },
	SV_BotGetSnapshotEntity,
	SV_BotGetConsoleMessage,
	[]( int clientNum, usercmd_t *ucmd ) { SV_ClientThink( &svs.clients[clientNum], ucmd ); },
	[]( int entnum, void *info ) { botlib_export->aas.AAS_EntityInfo( entnum, (struct aas_entityinfo_s *)info ); },
	[]() { return botlib_export->aas.AAS_Initialized(); },
	[]( int presencetype, vec3_t mins, vec3_t maxs ) { botlib_export->aas.AAS_PresenceTypeBoundingBox( presencetype, (vec_t *)mins, (vec_t *)maxs ); },
	[]() { return botlib_export->aas.AAS_Time(); },
	[]( int index ) { botlib_export->aas.AAS_SetCurrentWorld( index ); },
	[]( vec3_t point ) { return botlib_export->aas.AAS_PointAreaNum( (vec_t *)point ); },
	[]( vec3_t start, vec3_t end, int *areas, vec3_t *points, int maxareas ) { return botlib_export->aas.AAS_TraceAreas( (vec_t *)start, (vec_t *)end, areas, points, maxareas ); },
	[]( vec3_t point ) { return botlib_export->aas.AAS_PointContents( (vec_t *)point ); },
	[]( int ent ) { return botlib_export->aas.AAS_NextBSPEntity( ent ); },
	[]( int ent, char *key, char *value, int size ) { return botlib_export->aas.AAS_ValueForBSPEpairKey( ent, key, value, size ); },
	[]( int ent, char *key, vec3_t v ) { return botlib_export->aas.AAS_VectorForBSPEpairKey( ent, key, (vec_t *)v ); },
	[]( int ent, char *key, float *value ) { return botlib_export->aas.AAS_FloatForBSPEpairKey( ent, key, value ); },
	[]( int ent, char *key, int *value ) { return botlib_export->aas.AAS_IntForBSPEpairKey( ent, key, value ); },
	[]( int areanum ) { return botlib_export->aas.AAS_AreaReachability( areanum ); },
	[]( int areanum, vec3_t origin, int goalareanum, int travelflags ) { return botlib_export->aas.AAS_AreaTravelTimeToGoalArea( areanum, (vec_t *)origin, goalareanum, travelflags ); },
	[]( vec3_t origin ) { return botlib_export->aas.AAS_Swimming( (vec_t *)origin ); },
	[]( void *move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize ) {
		return botlib_export->aas.AAS_PredictClientMovement( (struct aas_clientmove_s *)move, entnum, (vec_t *)origin, presencetype, onground,
															(vec_t *)velocity, (vec_t *)cmdmove, cmdframes, maxframes, frametime, stopevent, stopareanum, visualize );
	},
	[]( vec3_t srcpos, int srcnum, int destnum ) { botlib_export->aas.AAS_RT_ShowRoute( (vec_t *)srcpos, srcnum, destnum ); },
	[]( vec3_t srcpos, int srcnum, int srcarea, vec3_t destpos, int destnum, int destarea, vec3_t returnPos ) { return botlib_export->aas.AAS_RT_GetHidePos( (vec_t *)srcpos, srcnum, srcarea, (vec_t *)destpos, destnum, destarea, (vec_t *)returnPos ); },
	[]( int srcnum, int rangenum, int enemynum, float rangedist, int travelflags, float *outpos ) { return botlib_export->aas.AAS_FindAttackSpotWithinRange( srcnum, rangenum, enemynum, rangedist, travelflags, outpos ); },
	[]( vec3_t srcpos, vec3_t destpos, int travelflags, vec3_t retpos ) { return botlib_export->aas.AAS_GetRouteFirstVisPos( (vec_t *)srcpos, (vec_t *)destpos, travelflags, (vec_t *)retpos ); },
	[]( vec3_t absmin, vec3_t absmax, qboolean blocking ) { botlib_export->aas.AAS_SetAASBlockingEntity( (vec_t *)absmin, (vec_t *)absmax, blocking ); },
	[]( int client, char *str ) { botlib_export->ea.EA_Say( client, str ); },
	[]( int client, char *str ) { botlib_export->ea.EA_SayTeam( client, str ); },
	[]( int client, char *it ) { botlib_export->ea.EA_UseItem( client, it ); },
	[]( int client, char *it ) { botlib_export->ea.EA_DropItem( client, it ); },
	[]( int client, char *inv ) { botlib_export->ea.EA_UseInv( client, inv ); },
	[]( int client, char *inv ) { botlib_export->ea.EA_DropInv( client, inv ); },
	[]( int client ) { botlib_export->ea.EA_Gesture( client ); },
	[]( int client, char *command ) { botlib_export->ea.EA_Command( client, command ); },
	[]( int client, int weapon ) { botlib_export->ea.EA_SelectWeapon( client, weapon ); },
	[]( int client ) { botlib_export->ea.EA_Talk( client ); },
	[]( int client ) { botlib_export->ea.EA_Attack( client ); },
	[]( int client ) { botlib_export->ea.EA_Reload( client ); },
	[]( int client ) { botlib_export->ea.EA_Use( client ); },
	[]( int client ) { botlib_export->ea.EA_Respawn( client ); },
	[]( int client ) { botlib_export->ea.EA_Jump( client ); },
	[]( int client ) { botlib_export->ea.EA_DelayedJump( client ); },
	[]( int client ) { botlib_export->ea.EA_Crouch( client ); },
	[]( int client ) { botlib_export->ea.EA_MoveUp( client ); },
	[]( int client ) { botlib_export->ea.EA_MoveDown( client ); },
	[]( int client ) { botlib_export->ea.EA_MoveForward( client ); },
	[]( int client ) { botlib_export->ea.EA_MoveBack( client ); },
	[]( int client ) { botlib_export->ea.EA_MoveLeft( client ); },
	[]( int client ) { botlib_export->ea.EA_MoveRight( client ); },
	[]( int client, vec3_t dir, float speed ) { botlib_export->ea.EA_Move( client, (vec_t *)dir, speed ); },
	[]( int client, vec3_t viewangles ) { botlib_export->ea.EA_View( client, (vec_t *)viewangles ); },
	[]( int client, float thinktime ) { botlib_export->ea.EA_EndRegular( client, thinktime ); },
	[]( int client, float thinktime, void *input ) { botlib_export->ea.EA_GetInput( client, thinktime, (bot_input_t *)input ); },
	[]( int client, void *init ) { botlib_export->ea.EA_ResetInput( client, (bot_input_t *)init ); },
	[]( char *charfile, int skill ) { return botlib_export->ai.BotLoadCharacter( charfile, skill ); },
	[]( int character ) { botlib_export->ai.BotFreeCharacter( character ); },
	[]( int character, int index ) { return botlib_export->ai.Characteristic_Float( character, index ); },
	[]( int character, int index, float min, float max ) { return botlib_export->ai.Characteristic_BFloat( character, index, min, max ); },
	[]( int character, int index ) { return botlib_export->ai.Characteristic_Integer( character, index ); },
	[]( int character, int index, int min, int max ) { return botlib_export->ai.Characteristic_BInteger( character, index, min, max ); },
	[]( int character, int index, char *buf, int size ) { botlib_export->ai.Characteristic_String( character, index, buf, size ); },
	[]() { return botlib_export->ai.BotAllocChatState(); },
	[]( int handle ) { botlib_export->ai.BotFreeChatState( handle ); },
	[]( int chatstate, int type, char *message ) { botlib_export->ai.BotQueueConsoleMessage( chatstate, type, message ); },
	[]( int chatstate, int handle ) { botlib_export->ai.BotRemoveConsoleMessage( chatstate, handle ); },
	[]( int chatstate, void *cm ) { return botlib_export->ai.BotNextConsoleMessage( chatstate, (struct bot_consolemessage_s *)cm ); },
	[]( int chatstate ) { return botlib_export->ai.BotNumConsoleMessages( chatstate ); },
	[]( int chatstate, char *type, int mcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 ) { botlib_export->ai.BotInitialChat( chatstate, type, mcontext, var0, var1, var2, var3, var4, var5, var6, var7 ); },
	[]( int chatstate, char *type ) { return botlib_export->ai.BotNumInitialChats( chatstate, type ); },
	[]( int chatstate, char *message, int mcontext, int vcontext, char *var0, char *var1, char *var2, char *var3, char *var4, char *var5, char *var6, char *var7 ) { return botlib_export->ai.BotReplyChat( chatstate, message, mcontext, vcontext, var0, var1, var2, var3, var4, var5, var6, var7 ); },
	[]( int chatstate ) { return botlib_export->ai.BotChatLength( chatstate ); },
	[]( int chatstate, int client, int sendto ) { botlib_export->ai.BotEnterChat( chatstate, client, sendto ); },
	[]( int chatstate, char *buf, int size ) { botlib_export->ai.BotGetChatMessage( chatstate, buf, size ); },
	[]( char *str1, char *str2, int casesensitive ) { return botlib_export->ai.StringContains( str1, str2, casesensitive ); },
	[]( char *str, void *match, unsigned long int context ) { return botlib_export->ai.BotFindMatch( str, (struct bot_match_s *)match, context ); },
	[]( void *match, int variable, char *buf, int size ) { botlib_export->ai.BotMatchVariable( (struct bot_match_s *)match, variable, buf, size ); },
	[]( char *string ) { botlib_export->ai.UnifyWhiteSpaces( string ); },
	[]( char *string, unsigned long int context ) { botlib_export->ai.BotReplaceSynonyms( string, context ); },
	[]( int chatstate, char *chatfile, char *chatname ) { return botlib_export->ai.BotLoadChatFile( chatstate, chatfile, chatname ); },
	[]( int chatstate, int gender ) { botlib_export->ai.BotSetChatGender( chatstate, gender ); },
	[]( int chatstate, char *name ) { botlib_export->ai.BotSetChatName( chatstate, name ); },
	[]( int goalstate ) { botlib_export->ai.BotResetGoalState( goalstate ); },
	[]( int goalstate ) { botlib_export->ai.BotResetAvoidGoals( goalstate ); },
	[]( int goalstate, int number ) { botlib_export->ai.BotRemoveFromAvoidGoals( goalstate, number ); },
	[]( int goalstate, void *goal ) { botlib_export->ai.BotPushGoal( goalstate, (struct bot_goal_s *)goal ); },
	[]( int goalstate ) { botlib_export->ai.BotPopGoal( goalstate ); },
	[]( int goalstate ) { botlib_export->ai.BotEmptyGoalStack( goalstate ); },
	[]( int goalstate ) { botlib_export->ai.BotDumpAvoidGoals( goalstate ); },
	[]( int goalstate ) { botlib_export->ai.BotDumpGoalStack( goalstate ); },
	[]( int number, char *name, int size ) { botlib_export->ai.BotGoalName( number, name, size ); },
	[]( int goalstate, void *goal ) { return botlib_export->ai.BotGetTopGoal( goalstate, (struct bot_goal_s *)goal ); },
	[]( int goalstate, void *goal ) { return botlib_export->ai.BotGetSecondGoal( goalstate, (struct bot_goal_s *)goal ); },
	[]( int goalstate, vec3_t origin, int *inventory, int travelflags ) { return botlib_export->ai.BotChooseLTGItem( goalstate, (vec_t *)origin, inventory, travelflags ); },
	[]( int goalstate, vec3_t origin, int *inventory, int travelflags, void *ltg, float maxtime ) { return botlib_export->ai.BotChooseNBGItem( goalstate, (vec_t *)origin, inventory, travelflags, (struct bot_goal_s *)ltg, maxtime ); },
	[]( vec3_t origin, void *goal ) { return botlib_export->ai.BotTouchingGoal( (vec_t *)origin, (struct bot_goal_s *)goal ); },
	[]( int viewer, vec3_t eye, vec3_t viewangles, void *goal ) { return botlib_export->ai.BotItemGoalInVisButNotVisible( viewer, (vec_t *)eye, (vec_t *)viewangles, (struct bot_goal_s *)goal ); },
	[]( int index, char *classname, void *goal ) { return botlib_export->ai.BotGetLevelItemGoal( index, classname, (struct bot_goal_s *)goal ); },
	[]( int num, void *goal ) { return botlib_export->ai.BotGetNextCampSpotGoal( num, (struct bot_goal_s *)goal ); },
	[]( char *name, void *goal ) { return botlib_export->ai.BotGetMapLocationGoal( name, (struct bot_goal_s *)goal ); },
	[]( int goalstate, int number ) { return botlib_export->ai.BotAvoidGoalTime( goalstate, number ); },
	[]() { botlib_export->ai.BotInitLevelItems(); },
	[]() { botlib_export->ai.BotUpdateEntityItems(); },
	[]( int goalstate, char *filename ) { return botlib_export->ai.BotLoadItemWeights( goalstate, filename ); },
	[]( int goalstate ) { botlib_export->ai.BotFreeItemWeights( goalstate ); },
	[]( int parent1, int parent2, int child ) { botlib_export->ai.BotInterbreedGoalFuzzyLogic( parent1, parent2, child ); },
	[]( int goalstate, char *filename ) { botlib_export->ai.BotSaveGoalFuzzyLogic( goalstate, filename ); },
	[]( int goalstate, float range ) { botlib_export->ai.BotMutateGoalFuzzyLogic( goalstate, range ); },
	[]( int state ) { return botlib_export->ai.BotAllocGoalState( state ); },
	[]( int handle ) { botlib_export->ai.BotFreeGoalState( handle ); },
	[]( int movestate ) { botlib_export->ai.BotResetMoveState( movestate ); },
	[]( void *result, int movestate, void *goal, int travelflags ) { botlib_export->ai.BotMoveToGoal( (struct bot_moveresult_s *)result, movestate, (struct bot_goal_s *)goal, travelflags ); },
	[]( int movestate, vec3_t dir, float speed, int type ) { return botlib_export->ai.BotMoveInDirection( movestate, (vec_t *)dir, speed, type ); },
	[]( int movestate ) { botlib_export->ai.BotResetAvoidReach( movestate ); },
	[]( int movestate ) { botlib_export->ai.BotResetLastAvoidReach( movestate ); },
	[]( vec3_t origin, int testground ) { return botlib_export->ai.BotReachabilityArea( (vec_t *)origin, testground ); },
	[]( int movestate, void *goal, int travelflags, float lookahead, vec3_t target ) { return botlib_export->ai.BotMovementViewTarget( movestate, (struct bot_goal_s *)goal, travelflags, lookahead, (vec_t *)target ); },
	[]( vec3_t origin, int areanum, void *goal, int travelflags, vec3_t target ) { return botlib_export->ai.BotPredictVisiblePosition( (vec_t *)origin, areanum, (struct bot_goal_s *)goal, travelflags, (vec_t *)target ); },
	[]() { return botlib_export->ai.BotAllocMoveState(); },
	[]( int handle ) { botlib_export->ai.BotFreeMoveState( handle ); },
	[]( int handle, void *initmove ) { botlib_export->ai.BotInitMoveState( handle, (struct bot_initmove_s *)initmove ); },
	[]( int handle ) { botlib_export->ai.BotInitAvoidReach( handle ); },
	[]( int weaponstate, int *inventory ) { return botlib_export->ai.BotChooseBestFightWeapon( weaponstate, inventory ); },
	[]( int weaponstate, int weapon, void *weaponinfo ) { botlib_export->ai.BotGetWeaponInfo( weaponstate, weapon, (struct weaponinfo_s *)weaponinfo ); },
	[]( int weaponstate, char *filename ) { return botlib_export->ai.BotLoadWeaponWeights( weaponstate, filename ); },
	[]() { return botlib_export->ai.BotAllocWeaponState(); },
	[]( int weaponstate ) { botlib_export->ai.BotFreeWeaponState( weaponstate ); },
	[]( int weaponstate ) { botlib_export->ai.BotResetWeaponState( weaponstate ); },
	[]( int numranks, float *ranks, int *parent1, int *parent2, int *child ) { return botlib_export->ai.GeneticParentsAndChildSelection( numranks, ranks, parent1, parent2, child ); },
	[]( vec3_t origin, vec3_t goal, int flags, vec3_t point ) { return botlib_export->aas.AAS_Route( origin, goal, flags, point ); },
	[]( vec3_t origin, vec3_t goal, int flags, void *route ) { return botlib_export->aas.AAS_DoomPlayerRoute( origin, goal, flags, (doomAasRoute_t *)route ); },
};
//==============================================

// The loaded game DLL and the exports it provides.
static void *gameDllHandle = NULL;
gameExport_t *gameExport = NULL;

/*
===============
SV_LoadGame

Load the game DLL and get its exports
===============
*/
static void SV_LoadGame( void ) {
	gameExport = (gameExport_t *)Sys_LoadDll( "qagame", &gameDllHandle, "GetGameAPI", (void *)&gameImportTable );
	if ( !gameExport ) {
		Com_Error( ERR_FATAL, "Sys_LoadDll on game failed" );
	}
}

/*
===============
SV_ShutdownGameProgs

Called every time a map changes
===============
*/
void SV_ShutdownGameProgs( void ) {
	if ( !gameExport ) {
		return;
	}
	gameExport->Shutdown( qfalse );
	Sys_UnloadDll( gameDllHandle );
	gameDllHandle = NULL;
	gameExport = NULL;
}

/*
==================
SV_InitGameVM

Called for both a full init and a restart
==================
*/
static void SV_InitGameVM(qboolean restart) {
    int i;

    // Native DLL reloads can relocate game entities. GAME_INIT publishes
    // configstrings, so invalidate client pointers before it can broadcast.
    for (i = 0; i < sv_maxclients->integer; ++i) {
        svs.clients[i].gentity = NULL;
    }

    // start the entity parsing at the beginning
    sv.entityParsePoint = CM_EntityString();

    // use the current msec count for a random seed
    // init for this gamestate
    gameExport->Init(svs.time, Com_Milliseconds(), restart);
}

/*
===================
SV_RestartGameProgs

Called on a map_restart, but not on a normal map change
===================
*/
void SV_RestartGameProgs( void ) {
	if ( !gameExport ) {
		return;
	}
	gameExport->Shutdown( qtrue );

	// do a restart instead of a free: unload the DLL and load it from disk again
	Sys_UnloadDll( gameDllHandle );
	gameDllHandle = NULL;
	gameExport = NULL;
	SV_LoadGame();

	SV_InitGameVM( qtrue );
}


/*
===============
SV_InitGameProgs

Called on a normal map change, not on a map_restart
===============
*/
void SV_InitGameProgs( void ) {
	cvar_t  *var;
	//FIXME these are temp while I make bots run in vm
	extern int bot_enable;

	var = Cvar_Get( "bot_enable", "1", CVAR_LATCH );
	if ( var ) {
		bot_enable = var->integer;
	} else {
		bot_enable = 0;
	}

	SV_LoadGame();

	SV_InitGameVM( qfalse );
}


/*
====================
SV_GameCommand

See if the current console command is claimed by the game
====================
*/
qboolean SV_GameCommand( void ) {
	if ( sv.state != SS_GAME ) {
		return qfalse;
	}

	return gameExport->ConsoleCommand();
}


/*
====================
SV_SendMoveSpeedsToGame
====================
*/
void SV_SendMoveSpeedsToGame( int entnum, char *text ) {
	if ( !gameExport ) {
		return;
	}
	gameExport->RetrieveMoveSpeedsFromClient( entnum, text );
}

/*
====================
SV_GetTag

  return qfalse if unable to retrieve tag information for this client
====================
*/
extern qboolean CL_GetTag( int clientNum, char *tagname, orientation_t *or );

qboolean SV_GetTag( int clientNum, char *tagname, orientation_t *or ) {
	if ( com_dedicated->integer ) {
		return qfalse;
	}

	return CL_GetTag( clientNum, tagname, or );
}

/*
===================
SV_GetModelInfo

  request this modelinfo from the game
===================
*/
qboolean SV_GetModelInfo( int clientNum, char *modelName, animModelInfo_t **modelInfo ) {
	return gameExport->GetModelInfo( clientNum, modelName, (void **)modelInfo );
}

