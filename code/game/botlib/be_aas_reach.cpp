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

/*****************************************************************************
 * name:		be_aas_reach.cpp
 *
 * desc:		reachability calculations
 *
 *
 *****************************************************************************/

#include "../game/q_shared.h"
#include "l_log.h"
#include "l_memory.h"
#include "l_script.h"
#include "l_libvar.h"
#include "l_precomp.h"
#include "l_struct.h"
#include "aasfile.h"
#include "../game/botlib.h"
#include "../game/be_aas.h"
#include "be_aas_funcs.h"
#include "be_aas_def.h"

extern int Sys_MilliSeconds( void );

//#include "../../../gladiator/bspc/aas_store.h"

extern botlib_import_t botimport;

//#define REACHDEBUG

//NOTE: all travel times are in hundreth of a second
//maximum fall delta before getting damaged

// Ridah, tweaked for Wolf AI
#define FALLDELTA_5DAMAGE                   25  //40
#define FALLDELTA_10DAMAGE                  40  //60
// done.

//maximum number of reachability links
#define AAS_MAX_REACHABILITYSIZE            65536
//number of areas reachability is calculated for each frame
#define REACHABILITYAREASPERCYCLE           15
//number of units reachability points are placed inside the areas
#define INSIDEUNITS                         2

// Ridah, tweaked this, routing issues around small areas
#define INSIDEUNITS_WALKEND             5   // original
//#define INSIDEUNITS_WALKEND					0.2		// new

// Ridah, added this for better walking off ledges
#define INSIDEUNITS_WALKOFFLEDGEEND         15

#define INSIDEUNITS_WALKSTART               0.1
#define INSIDEUNITS_WATERJUMP               15
//travel times in hundreth of a second

// Ridah, tweaked these for Wolf AI
#define REACH_MIN_TIME                      4   // always at least this much time for a reachability
#define WATERJUMP_TIME                      700 //7 seconds
#define TELEPORT_TIME                       50  //0.5 seconds
#define BARRIERJUMP_TIME                    900 //fixed value?
#define STARTCROUCH_TIME                    300 //3 sec to start crouching
#define STARTGRAPPLE_TIME                   500 //using the grapple costs a lot of time
#define STARTWALKOFFLEDGE_TIME              300 //3 seconds
#define STARTJUMP_TIME                      500 //3 seconds for jumping

#define FALLDAMAGE_5_TIME                   400 //extra travel time when falling hurts
#define FALLDAMAGE_10_TIME                  900 //extra travel time when falling hurts
// done.

//maximum height the bot may fall down when jumping
#define MAX_JUMPFALLHEIGHT                  450
//area flag used for weapon jumping
#define AREA_WEAPONJUMP                     8192    //valid area to weapon jump to
//number of reachabilities of each type
int reach_swim;         //swim
int reach_equalfloor;   //walk on floors with equal height
int reach_step;         //step up
int reach_walk;         //walk of step
int reach_barrier;      //jump up to a barrier
int reach_waterjump;    //jump out of water
int reach_walkoffledge; //walk of a ledge
int reach_jump;         //jump
int reach_ladder;       //climb or descent a ladder
int reach_teleport;     //teleport
int reach_elevator;     //use an elevator
int reach_funcbob;      //use a func bob
int reach_grapple;      //grapple hook
int reach_doublejump;   //double jump
int reach_rampjump;     //ramp jump
int reach_strafejump;   //strafe jump (just normal jump but further)
int reach_rocketjump;   //rocket jump
int reach_bfgjump;      //bfg jump
int reach_jumppad;      //jump pads
//if true grapple reachabilities are skipped
int calcgrapplereach;
//linked reachability
typedef struct aas_lreachability_s
{
	int areanum;                    //number of the reachable area
	int facenum;                    //number of the face towards the other area
	int edgenum;                    //number of the edge towards the other area
	vec3_t start;                   //start point of inter area movement
	vec3_t end;                     //end point of inter area movement
	int traveltype;                 //type of travel required to get to the area
	unsigned short int traveltime;  //travel time of the inter area movement
	//
	struct aas_lreachability_s *next;
} aas_lreachability_t;
//temporary reachabilities
aas_lreachability_t *reachabilityheap;  //heap with reachabilities
aas_lreachability_t *nextreachability;  //next free reachability from the heap
aas_lreachability_t **areareachability; //reachability links for every area
int numlreachabilities;

//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_BestReachableLinkArea( aas_link_t *areas ) {
	aas_link_t *link;

	for ( link = areas; link; link = link->next_area )
	{
		if ( AAS_AreaGrounded( link->areanum ) || AAS_AreaSwim( link->areanum ) ) {
			return link->areanum;
		} //end if
	} //end for
	  //
	for ( link = areas; link; link = link->next_area )
	{
		if ( link->areanum ) {
			return link->areanum;
		}
		//FIXME: cannot enable next line right now because the reachability
		// does not have to be calculated when the level items are loaded
		//if (AAS_AreaReachability(link->areanum)) return link->areanum;
	} //end for
	return 0;
} //end of the function AAS_BestReachableLinkArea
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_BestReachableArea( vec3_t origin, vec3_t mins, vec3_t maxs, vec3_t goalorigin ) {
	int areanum, i, j, k, l;
	aas_link_t *areas;
	vec3_t absmins, absmaxs;
	//vec3_t bbmins, bbmaxs;
	vec3_t start, end;
	aas_trace_t trace;

	if ( !( *aasworld ).loaded ) {
		botimport.Print( PRT_ERROR, "AAS_BestReachableArea: aas not loaded\n" );
		return 0;
	} //end if
	  //find a point in an area
	VectorCopy( origin, start );
	areanum = AAS_PointAreaNum( start );
	//while no area found fudge around a little
	for ( i = 0; i < 5 && !areanum; i++ )
	{
		for ( j = 0; j < 5 && !areanum; j++ )
		{
			for ( k = -1; k <= 1 && !areanum; k++ )
			{
				for ( l = -1; l <= 1 && !areanum; l++ )
				{
					VectorCopy( origin, start );
					start[0] += (float) j * 4 * k;
					start[1] += (float) j * 4 * l;
					start[2] += (float) i * 4;
					areanum = AAS_PointAreaNum( start );
				} //end for
			} //end for
		} //end for
	} //end for
	  //if an area was found
	if ( areanum ) {
		//drop client bbox down and try again
		VectorCopy( start, end );
		start[2] += 0.25;
		end[2] -= 50;
		trace = AAS_TraceClientBBox( start, end, PRESENCE_CROUCH, -1 );
		if ( !trace.startsolid ) {
			areanum = AAS_PointAreaNum( trace.endpos );
			VectorCopy( trace.endpos, goalorigin );
			//FIXME: cannot enable next line right now because the reachability
			// does not have to be calculated when the level items are loaded
			//if the origin is in an area with reachability
			//if (AAS_AreaReachability(areanum)) return areanum;
			if ( areanum ) {
				return areanum;
			}
		} //end if
		else
		{
			//it can very well happen that the AAS_PointAreaNum function tells that
			//a point is in an area and that starting a AAS_TraceClientBBox from that
			//point will return trace.startsolid true
			/*
			if (AAS_PointAreaNum(start))
			{
				Log_Write("point %f %f %f in area %d but trace startsolid", start[0], start[1], start[2], areanum);
				AAS_DrawPermanentCross(start, 4, LINECOLOR_RED);
			} //end if
			botimport.Print(PRT_MESSAGE, "AAS_BestReachableArea: start solid\n");
	  */
			VectorCopy( start, goalorigin );
			return areanum;
		} //end else
	} //end if
	  //
	  //AAS_PresenceTypeBoundingBox(PRESENCE_CROUCH, bbmins, bbmaxs);
	  //NOTE: the goal origin does not have to be in the goal area
	  // because the bot will have to move towards the item origin anyway
	VectorCopy( origin, goalorigin );
	//
	VectorAdd( origin, mins, absmins );
	VectorAdd( origin, maxs, absmaxs );
	//add bounding box size
	//VectorSubtract(absmins, bbmaxs, absmins);
	//VectorSubtract(absmaxs, bbmins, absmaxs);
	//link an invalid (-1) entity
	areas = AAS_AASLinkEntity( absmins, absmaxs, -1 );
	//get the reachable link arae
	areanum = AAS_BestReachableLinkArea( areas );
	//unlink the invalid entity
	AAS_UnlinkFromAreas( areas );
	//
	return areanum;
} //end of the function AAS_BestReachableArea
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void AAS_SetupReachabilityHeap( void ) {
	int i;

	reachabilityheap = (aas_lreachability_t *) GetClearedMemory(
		AAS_MAX_REACHABILITYSIZE * sizeof( aas_lreachability_t ) );
	for ( i = 0; i < AAS_MAX_REACHABILITYSIZE - 1; i++ )
	{
		reachabilityheap[i].next = &reachabilityheap[i + 1];
	} //end for
	reachabilityheap[AAS_MAX_REACHABILITYSIZE - 1].next = NULL;
	nextreachability = reachabilityheap;
	numlreachabilities = 0;
} //end of the function AAS_InitReachabilityHeap
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void AAS_ShutDownReachabilityHeap( void ) {
	FreeMemory( reachabilityheap );
	numlreachabilities = 0;
} //end of the function AAS_ShutDownReachabilityHeap
//===========================================================================
// returns a reachability link
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
aas_lreachability_t *AAS_AllocReachability( void ) {
	aas_lreachability_t *r;

	if ( !nextreachability ) {
		return NULL;
	}
	//make sure the error message only shows up once
	if ( !nextreachability->next ) {
		AAS_Error( "AAS_MAX_REACHABILITYSIZE" );
	}
	//
	r = nextreachability;
	nextreachability = nextreachability->next;
	numlreachabilities++;
	return r;
} //end of the function AAS_AllocReachability
//===========================================================================
// frees a reachability link
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void AAS_FreeReachability( aas_lreachability_t *lreach ) {
	memset( lreach, 0, sizeof( aas_lreachability_t ) );

	lreach->next = nextreachability;
	nextreachability = lreach;
	numlreachabilities--;
} //end of the function AAS_FreeReachability
//===========================================================================
// returns true if the area has reachability links
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaReachability( int areanum ) {
	if ( areanum < 0 || areanum >= ( *aasworld ).numareas ) {
		AAS_Error( "AAS_AreaReachability: areanum %d out of range", areanum );
		return 0;
	} //end if
	  // RF, if this area is disabled, then fail
	if ( ( *aasworld ).areasettings[areanum].areaflags & AREA_DISABLED ) {
		return 0;
	}
	return ( *aasworld ).areasettings[areanum].numreachableareas;
} //end of the function AAS_AreaReachability
//===========================================================================
// returns the surface area of the given face
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
float AAS_FaceArea( aas_face_t *face ) {
	int i, edgenum, side;
	float total;
	vec_t *v;
	vec3_t d1, d2, cross;
	aas_edge_t *edge;

	edgenum = ( *aasworld ).edgeindex[face->firstedge];
	side = edgenum < 0;
	edge = &( *aasworld ).edges[abs( edgenum )];
	v = ( *aasworld ).vertexes[edge->v[side]];

	total = 0;
	for ( i = 1; i < face->numedges - 1; i++ )
	{
		edgenum = ( *aasworld ).edgeindex[face->firstedge + i];
		side = edgenum < 0;
		edge = &( *aasworld ).edges[abs( edgenum )];
		VectorSubtract( ( *aasworld ).vertexes[edge->v[side]], v, d1 );
		VectorSubtract( ( *aasworld ).vertexes[edge->v[!side]], v, d2 );
		CrossProduct( d1, d2, cross );
		total += 0.5 * VectorLength( cross );
	} //end for
	return total;
} //end of the function AAS_FaceArea
//===========================================================================
// returns the volume of an area
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
float AAS_AreaVolume( int areanum ) {
	int i, edgenum, facenum;
	vec_t d, a, volume;
	vec3_t corner;
	aas_plane_t *plane;
	aas_edge_t *edge;
	aas_face_t *face;
	aas_area_t *area;

	area = &( *aasworld ).areas[areanum];
	facenum = ( *aasworld ).faceindex[area->firstface];
	face = &( *aasworld ).faces[abs( facenum )];
	edgenum = ( *aasworld ).edgeindex[face->firstedge];
	edge = &( *aasworld ).edges[abs( edgenum )];
	//
	VectorCopy( ( *aasworld ).vertexes[edge->v[0]], corner );

	//make tetrahedrons to all other faces
	volume = 0;
	for ( i = 0; i < area->numfaces; i++ )
	{
		facenum = abs( ( *aasworld ).faceindex[area->firstface + i] );
		face = &( *aasworld ).faces[facenum];
		plane = &( *aasworld ).planes[face->planenum];
		d = -( DotProduct( corner, plane->normal ) - plane->dist );
		a = AAS_FaceArea( face );
		volume += d * a;
	} //end for

	volume /= 3;
	return volume;
} //end of the function AAS_AreaVolume
//===========================================================================
// returns the surface area of all ground faces together of the area
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
float AAS_AreaGroundFaceArea( int areanum ) {
	int i;
	float total;
	aas_area_t *area;
	aas_face_t *face;

	total = 0;
	area = &( *aasworld ).areas[areanum];
	for ( i = 0; i < area->numfaces; i++ )
	{
		face = &( *aasworld ).faces[abs( ( *aasworld ).faceindex[area->firstface + i] )];
		if ( !( face->faceflags & FACE_GROUND ) ) {
			continue;
		}
		//
		total += AAS_FaceArea( face );
	} //end for
	return total;
} //end of the function AAS_AreaGroundFaceArea
//===========================================================================
// returns the center of a face
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void AAS_FaceCenter( int facenum, vec3_t center ) {
	int i;
	float scale;
	aas_face_t *face;
	aas_edge_t *edge;

	face = &( *aasworld ).faces[facenum];

	VectorClear( center );
	for ( i = 0; i < face->numedges; i++ )
	{
		edge = &( *aasworld ).edges[abs( ( *aasworld ).edgeindex[face->firstedge + i] )];
		VectorAdd( center, ( *aasworld ).vertexes[edge->v[0]], center );
		VectorAdd( center, ( *aasworld ).vertexes[edge->v[1]], center );
	} //end for
	scale = 0.5 / face->numedges;
	VectorScale( center, scale, center );
} //end of the function AAS_FaceCenter
//===========================================================================
// returns the maximum distance a player can fall before being damaged
// damage = deltavelocity*deltavelocity  * 0.0001
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_FallDamageDistance( void ) {
	float maxzvelocity, gravity, t;

	maxzvelocity = sqrt( 30 * 10000 );
	gravity = aassettings.sv_gravity;
	t = maxzvelocity / gravity;
	return 0.5 * gravity * t * t;
} //end of the function AAS_FallDamageDistance
//===========================================================================
// distance = 0.5 * gravity * t * t
// vel = t * gravity
// damage = vel * vel * 0.0001
//
// Parameter:			-
// Returns:				-
// Changes Globals:		-
//===========================================================================
float AAS_FallDelta( float distance ) {
	float t, delta, gravity;

	gravity = aassettings.sv_gravity;
	t = sqrt( fabs( distance ) * 2 / gravity );
	delta = t * gravity;
	return delta * delta * 0.0001;
} //end of the function AAS_FallDelta
//===========================================================================
//
// Parameter:			-
// Returns:				-
// Changes Globals:		-
//===========================================================================
float AAS_MaxJumpHeight( float sv_jumpvel ) {
	float sv_gravity;

	sv_gravity = aassettings.sv_gravity;
	//maximum height a player can jump with the given initial z velocity
	return 0.5 * sv_gravity * ( sv_jumpvel / sv_gravity ) * ( sv_jumpvel / sv_gravity );
} //end of the function MaxJumpHeight
//===========================================================================
// returns true if a player can only crouch in the area
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
float AAS_MaxJumpDistance( float sv_jumpvel ) {
	float sv_gravity, sv_maxvelocity, t;

	sv_gravity = aassettings.sv_gravity;
	sv_maxvelocity = aassettings.sv_maxvelocity;
	//time a player takes to fall the height
	t = sqrt( MAX_JUMPFALLHEIGHT / ( 0.5 * sv_gravity ) );
	//maximum distance
	return sv_maxvelocity * ( t + sv_jumpvel / sv_gravity );
} //end of the function AAS_MaxJumpDistance
//===========================================================================
// returns true if a player can only crouch in the area
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaCrouch( int areanum ) {
	if ( !( ( *aasworld ).areasettings[areanum].presencetype & PRESENCE_NORMAL ) ) {
		return true;
	} else { return false;}
} //end of the function AAS_AreaCrouch
//===========================================================================
// returns true if it is possible to swim in the area
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaSwim( int areanum ) {
	if ( ( *aasworld ).areasettings[areanum].areaflags & AREA_LIQUID ) {
		return true;
	} else { return false;}
} //end of the function AAS_AreaSwim
//===========================================================================
// returns true if the area contains a liquid
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaLiquid( int areanum ) {
	if ( ( *aasworld ).areasettings[areanum].areaflags & AREA_LIQUID ) {
		return true;
	} else { return false;}
} //end of the function AAS_AreaLiquid
//===========================================================================
//
// Parameter:			-
// Returns:				-
// Changes Globals:		-
//===========================================================================
int AAS_AreaLava( int areanum ) {
	return ( ( *aasworld ).areasettings[areanum].contents & AREACONTENTS_LAVA );
} //end of the function AAS_AreaLava
//===========================================================================
//
// Parameter:			-
// Returns:				-
// Changes Globals:		-
//===========================================================================
int AAS_AreaSlime( int areanum ) {
	return ( ( *aasworld ).areasettings[areanum].contents & AREACONTENTS_SLIME );
} //end of the function AAS_AreaSlime
//===========================================================================
// returns true if the area contains ground faces
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaGrounded( int areanum ) {
	return ( ( *aasworld ).areasettings[areanum].areaflags & AREA_GROUNDED );
} //end of the function AAS_AreaGround
//===========================================================================
// returns true if the area contains ladder faces
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaLadder( int areanum ) {
	return ( ( *aasworld ).areasettings[areanum].areaflags & AREA_LADDER );
} //end of the function AAS_AreaLadder
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaJumpPad( int areanum ) {
	return ( ( *aasworld ).areasettings[areanum].contents & AREACONTENTS_JUMPPAD );
} //end of the function AAS_AreaJumpPad
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaTeleporter( int areanum ) {
	return ( ( *aasworld ).areasettings[areanum].contents & AREACONTENTS_TELEPORTER );
} //end of the function AAS_AreaTeleporter
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaDoNotEnter( int areanum ) {
	return ( ( *aasworld ).areasettings[areanum].contents & AREACONTENTS_DONOTENTER );
} //end of the function AAS_AreaDoNotEnter
//===========================================================================
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_AreaDoNotEnterLarge( int areanum ) {
	return ( ( *aasworld ).areasettings[areanum].contents & AREACONTENTS_DONOTENTER_LARGE );
} //end of the function AAS_AreaDoNotEnter
//===========================================================================
// returns the time it takes perform a barrier jump
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
unsigned short int AAS_BarrierJumpTravelTime( void ) {
	return aassettings.sv_jumpvel / ( aassettings.sv_gravity * 0.1 );
} //end op the function AAS_BarrierJumpTravelTime
//===========================================================================
// returns true if there already exists a reachability from area1 to area2
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
bool AAS_ReachabilityExists( int area1num, int area2num ) {
	aas_lreachability_t *r;

	for ( r = areareachability[area1num]; r; r = r->next )
	{
		if ( r->areanum == area2num ) {
			return true;
		}
	} //end for
	return false;
} //end of the function AAS_ReachabilityExists
//===========================================================================
// returns true if there is a solid just after the end point when going
// from start to end
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_NearbySolidOrGap( vec3_t start, vec3_t end ) {
	vec3_t dir, testpoint;
	int areanum;

	VectorSubtract( end, start, dir );
	dir[2] = 0;
	VectorNormalize( dir );
	VectorMA( end, 48, dir, testpoint );

	areanum = AAS_PointAreaNum( testpoint );
	if ( !areanum ) {
		testpoint[2] += 16;
		areanum = AAS_PointAreaNum( testpoint );
		if ( !areanum ) {
			return true;
		}
	} //end if
	VectorMA( end, 64, dir, testpoint );
	areanum = AAS_PointAreaNum( testpoint );
	if ( areanum ) {
		if ( !AAS_AreaSwim( areanum ) && !AAS_AreaGrounded( areanum ) ) {
			return true;
		}
	} //end if
	return false;
} //end of the function AAS_SolidGapTime
//===========================================================================
// searches for swim reachabilities between adjacent areas
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_Reachability_Swim( int area1num, int area2num ) {
	int i, j, face1num, face2num, side1;
	aas_area_t *area1, *area2;
	aas_lreachability_t *lreach;
	aas_face_t *face1;
	aas_plane_t *plane;
	vec3_t start;

	if ( !AAS_AreaSwim( area1num ) || !AAS_AreaSwim( area2num ) ) {
		return false;
	}
	//if the second area is crouch only
	if ( !( ( *aasworld ).areasettings[area2num].presencetype & PRESENCE_NORMAL ) ) {
		return false;
	}

	area1 = &( *aasworld ).areas[area1num];
	area2 = &( *aasworld ).areas[area2num];

	//if the areas are not near anough
	for ( i = 0; i < 3; i++ )
	{
		if ( area1->mins[i] > area2->maxs[i] + 10 ) {
			return false;
		}
		if ( area1->maxs[i] < area2->mins[i] - 10 ) {
			return false;
		}
	} //end for
	  //find a shared face and create a reachability link
	for ( i = 0; i < area1->numfaces; i++ )
	{
		face1num = ( *aasworld ).faceindex[area1->firstface + i];
		side1 = face1num < 0;
		face1num = abs( face1num );
		//
		for ( j = 0; j < area2->numfaces; j++ )
		{
			face2num = abs( ( *aasworld ).faceindex[area2->firstface + j] );
			//
			if ( face1num == face2num ) {
				AAS_FaceCenter( face1num, start );
				//
				if ( AAS_PointContents( start ) & ( CONTENTS_LAVA | CONTENTS_SLIME | CONTENTS_WATER ) ) {
					//
					face1 = &( *aasworld ).faces[face1num];
					//create a new reachability link
					lreach = AAS_AllocReachability();
					if ( !lreach ) {
						return false;
					}
					lreach->areanum = area2num;
					lreach->facenum = face1num;
					lreach->edgenum = 0;
					VectorCopy( start, lreach->start );
					plane = &( *aasworld ).planes[face1->planenum ^ side1];
					VectorMA( lreach->start, INSIDEUNITS, plane->normal, lreach->end );
					lreach->traveltype = TRAVEL_SWIM;
					lreach->traveltime = 1;
					//if the volume of the area is rather small
					if ( AAS_AreaVolume( area2num ) < 800 ) {
						lreach->traveltime += 200;
					}
					//if (!(AAS_PointContents(start) & MASK_WATER)) lreach->traveltime += 500;
					//link the reachability
					lreach->next = areareachability[area1num];
					areareachability[area1num] = lreach;
					reach_swim++;
					return true;
				} //end if
			} //end if
		} //end for
	} //end for
	return false;
} //end of the function AAS_Reachability_Swim
//===========================================================================
// searches for reachabilities between adjacent areas with equal floor
// heights
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_Reachability_EqualFloorHeight( int area1num, int area2num ) {
	int i, j, edgenum, edgenum1, edgenum2, foundreach, side;
	float height, bestheight, length, bestlength;
	vec3_t dir, start, end, normal, invgravity, gravitydirection = {0, 0, -1};
	vec3_t edgevec;
	aas_area_t *area1, *area2;
	aas_face_t *face1, *face2;
	aas_edge_t *edge;
	aas_plane_t *plane2;
	aas_lreachability_t lr, *lreach;

	if ( !AAS_AreaGrounded( area1num ) || !AAS_AreaGrounded( area2num ) ) {
		return false;
	}

	area1 = &( *aasworld ).areas[area1num];
	area2 = &( *aasworld ).areas[area2num];
	//if the areas are not near anough in the x-y direction
	for ( i = 0; i < 2; i++ )
	{
		if ( area1->mins[i] > area2->maxs[i] + 10 ) {
			return false;
		}
		if ( area1->maxs[i] < area2->mins[i] - 10 ) {
			return false;
		}
	} //end for
	  //if area 2 is too high above area 1
	if ( area2->mins[2] > area1->maxs[2] ) {
		return false;
	}
	//
	VectorCopy( gravitydirection, invgravity );
	VectorInverse( invgravity );
	//
	bestheight = 99999;
	bestlength = 0;
	foundreach = false;
	memset( &lr, 0, sizeof( aas_lreachability_t ) ); //make the compiler happy
	//
	//check if the areas have ground faces with a common edge
	//if existing use the lowest common edge for a reachability link
	for ( i = 0; i < area1->numfaces; i++ )
	{
		face1 = &( *aasworld ).faces[abs( ( *aasworld ).faceindex[area1->firstface + i] )];
		if ( !( face1->faceflags & FACE_GROUND ) ) {
			continue;
		}
		//
		for ( j = 0; j < area2->numfaces; j++ )
		{
			face2 = &( *aasworld ).faces[abs( ( *aasworld ).faceindex[area2->firstface + j] )];
			if ( !( face2->faceflags & FACE_GROUND ) ) {
				continue;
			}
			//if there is a common edge
			for ( edgenum1 = 0; edgenum1 < face1->numedges; edgenum1++ )
			{
				for ( edgenum2 = 0; edgenum2 < face2->numedges; edgenum2++ )
				{
					if ( abs( ( *aasworld ).edgeindex[face1->firstedge + edgenum1] ) !=
						 abs( ( *aasworld ).edgeindex[face2->firstedge + edgenum2] ) ) {
						continue;
					}
					edgenum = ( *aasworld ).edgeindex[face1->firstedge + edgenum1];
					side = edgenum < 0;
					edge = &( *aasworld ).edges[abs( edgenum )];
					//get the length of the edge
					VectorSubtract( ( *aasworld ).vertexes[edge->v[1]],
									( *aasworld ).vertexes[edge->v[0]], dir );
					length = VectorLength( dir );
					//get the start point
					VectorAdd( ( *aasworld ).vertexes[edge->v[0]],
							   ( *aasworld ).vertexes[edge->v[1]], start );
					VectorScale( start, 0.5, start );
					VectorCopy( start, end );
					//get the end point several units inside area2
					//and the start point several units inside area1
					//NOTE: normal is pointing into area2 because the
					//face edges are stored counter clockwise
					VectorSubtract( ( *aasworld ).vertexes[edge->v[side]],
									( *aasworld ).vertexes[edge->v[!side]], edgevec );
					plane2 = &( *aasworld ).planes[face2->planenum];
					CrossProduct( edgevec, plane2->normal, normal );
					VectorNormalize( normal );
					//
					//VectorMA(start, -1, normal, start);
					VectorMA( end, INSIDEUNITS_WALKEND, normal, end );
					VectorMA( start, INSIDEUNITS_WALKSTART, normal, start );
					end[2] += 0.125;
					//
					height = DotProduct( invgravity, start );
					//NOTE: if there's nearby solid or a gap area after this area
					//disabled this crap
					//if (AAS_NearbySolidOrGap(start, end)) height += 200;
					//NOTE: disabled because it disables reachabilities to very small areas
					//if (AAS_PointAreaNum(end) != area2num) continue;
					//get the longest lowest edge
					if ( height < bestheight ||
						 ( height < bestheight + 1 && length > bestlength ) ) {
						bestheight = height;
						bestlength = length;
						//create a new reachability link
						lr.areanum = area2num;
						lr.facenum = 0;
						lr.edgenum = edgenum;
						VectorCopy( start, lr.start );
						VectorCopy( end, lr.end );
						lr.traveltype = TRAVEL_WALK;
						lr.traveltime = 1;
						foundreach = true;
					} //end if
				} //end for
			} //end for
		} //end for
	} //end for
	if ( foundreach ) {
		//create a new reachability link
		lreach = AAS_AllocReachability();
		if ( !lreach ) {
			return false;
		}
		lreach->areanum = lr.areanum;
		lreach->facenum = lr.facenum;
		lreach->edgenum = lr.edgenum;
		VectorCopy( lr.start, lreach->start );
		VectorCopy( lr.end, lreach->end );
		lreach->traveltype = lr.traveltype;
		lreach->traveltime = lr.traveltime;
		lreach->next = areareachability[area1num];
		areareachability[area1num] = lreach;
		//if going into a crouch area
		if ( !AAS_AreaCrouch( area1num ) && AAS_AreaCrouch( area2num ) ) {
			lreach->traveltime += STARTCROUCH_TIME;
		} //end if
		  /*
		  //NOTE: if there's nearby solid or a gap area after this area
		  if (!AAS_NearbySolidOrGap(lreach->start, lreach->end))
		  {
			  lreach->traveltime += 100;
		  } //end if
		  */
		  //avoid rather small areas
		  //if (AAS_AreaGroundFaceArea(lreach->areanum) < 500) lreach->traveltime += 100;
		  //
		reach_equalfloor++;
		return true;
	} //end if
	return false;
} //end of the function AAS_Reachability_EqualFloorHeight
//===========================================================================
// searches step, barrier, waterjump and walk off ledge reachabilities
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int AAS_Reachability_Step_Barrier_WaterJump_WalkOffLedge( int area1num, int area2num ) {
	int i, j, k, l, edge1num, edge2num;
	int ground_bestarea2groundedgenum, ground_foundreach;
	int water_bestarea2groundedgenum, water_foundreach;
	int side1, area1swim, faceside1, groundface1num;
	float dist, dist1, dist2, diff, ortdot;
	float x1, x2, x3, x4, y1, y2, y3, y4, tmp, y;
	float length, ground_bestlength, water_bestlength, ground_bestdist, water_bestdist;
	vec3_t v1, v2, v3, v4, tmpv, p1area1, p1area2, p2area1, p2area2;
	vec3_t normal, ort, edgevec, start, end, dir;
	vec3_t ground_beststart, ground_bestend, ground_bestnormal;
	vec3_t water_beststart, water_bestend, water_bestnormal;
	vec3_t invgravity = {0, 0, 1};
	vec3_t testpoint;
	aas_plane_t *plane;
	aas_area_t *area1, *area2;
	aas_face_t *groundface1, *groundface2;
	aas_edge_t *edge1, *edge2;
	aas_lreachability_t *lreach;
	aas_trace_t trace;

	//must be able to walk or swim in the first area
	if ( !AAS_AreaGrounded( area1num ) && !AAS_AreaSwim( area1num ) ) {
		return false;
	}
	//
	if ( !AAS_AreaGrounded( area2num ) && !AAS_AreaSwim( area2num ) ) {
		return false;
	}
	//
	area1 = &( *aasworld ).areas[area1num];
	area2 = &( *aasworld ).areas[area2num];
	//if the first area contains a liquid
	area1swim = AAS_AreaSwim( area1num );
	//if the areas are not near anough in the x-y direction
	for ( i = 0; i < 2; i++ )
	{
		if ( area1->mins[i] > area2->maxs[i] + 10 ) {
			return false;
		}
		if ( area1->maxs[i] < area2->mins[i] - 10 ) {
			return false;
		}
	} //end for
	  //
	ground_foundreach = false;
	ground_bestdist = 99999;
	ground_bestlength = 0;
	ground_bestarea2groundedgenum = 0;
	//
	water_foundreach = false;
	water_bestdist = 99999;
	water_bestlength = 0;
	water_bestarea2groundedgenum = 0;
	//
	for ( i = 0; i < area1->numfaces; i++ )
	{
		groundface1num = ( *aasworld ).faceindex[area1->firstface + i];
		faceside1 = groundface1num < 0;
		groundface1 = &( *aasworld ).faces[abs( groundface1num )];
		//if this isn't a ground face
		if ( !( groundface1->faceflags & FACE_GROUND ) ) {
			//if we can swim in the first area
			if ( area1swim ) {
				//face plane must be more or less horizontal
				plane = &( *aasworld ).planes[groundface1->planenum ^ ( !faceside1 )];
				if ( DotProduct( plane->normal, invgravity ) < 0.7 ) {
					continue;
				}
			} //end if
			else
			{
				//if we can't swim in the area it must be a ground face
				continue;
			} //end else
		} //end if
		  //
		for ( k = 0; k < groundface1->numedges; k++ )
		{
			edge1num = ( *aasworld ).edgeindex[groundface1->firstedge + k];
			side1 = ( edge1num < 0 );
			//NOTE: for water faces we must take the side area 1 is
			// on into account because the face is shared and doesn't
			// have to be oriented correctly
			if ( !( groundface1->faceflags & FACE_GROUND ) ) {
				side1 = ( side1 == faceside1 );
			}
			edge1num = abs( edge1num );
			edge1 = &( *aasworld ).edges[edge1num];
			//vertexes of the edge
			VectorCopy( ( *aasworld ).vertexes[edge1->v[!side1]], v1 );
			VectorCopy( ( *aasworld ).vertexes[edge1->v[side1]], v2 );
			//get a vertical plane through the edge
			//NOTE: normal is pointing into area 2 because the
			//face edges are stored counter clockwise
			VectorSubtract( v2, v1, edgevec );
			CrossProduct( edgevec, invgravity, normal );
			VectorNormalize( normal );
			dist = DotProduct( normal, v1 );
			//check the faces from the second area
			for ( j = 0; j < area2->numfaces; j++ )
			{
				groundface2 = &( *aasworld ).faces[abs( ( *aasworld ).faceindex[area2->firstface + j] )];
				//must be a ground face
				if ( !( groundface2->faceflags & FACE_GROUND ) ) {
					continue;
				}
				//check the edges of this ground face
				for ( l = 0; l < groundface2->numedges; l++ )
				{
					edge2num = abs( ( *aasworld ).edgeindex[groundface2->firstedge + l] );
					edge2 = &( *aasworld ).edges[edge2num];
					//vertexes of the edge
					VectorCopy( ( *aasworld ).vertexes[edge2->v[0]], v3 );
					VectorCopy( ( *aasworld ).vertexes[edge2->v[1]], v4 );
					//check the distance between the two points and the vertical plane
					//through the edge of area1
					diff = DotProduct( normal, v3 ) - dist;
					if ( diff < -0.1 || diff > 0.1 ) {
						continue;
					}
					diff = DotProduct( normal, v4 ) - dist;
					if ( diff < -0.1 || diff > 0.1 ) {
						continue;
					}
					//
					//project the two ground edges into the step side plane
					//and calculate the shortest distance between the two
					//edges if they overlap in the direction orthogonal to
					//the gravity direction
					CrossProduct( invgravity, normal, ort );
					ortdot = DotProduct( ort, ort );
					//projection into the step plane
					//NOTE: since gravity is vertical this is just the z coordinate
					y1 = v1[2]; //DotProduct(v1, invgravity) / invgravitydot;
					y2 = v2[2]; //DotProduct(v2, invgravity) / invgravitydot;
					y3 = v3[2]; //DotProduct(v3, invgravity) / invgravitydot;
					y4 = v4[2]; //DotProduct(v4, invgravity) / invgravitydot;
					//
					x1 = DotProduct( v1, ort ) / ortdot;
					x2 = DotProduct( v2, ort ) / ortdot;
					x3 = DotProduct( v3, ort ) / ortdot;
					x4 = DotProduct( v4, ort ) / ortdot;
					//
					if ( x1 > x2 ) {
						tmp = x1; x1 = x2; x2 = tmp;
						tmp = y1; y1 = y2; y2 = tmp;
						VectorCopy( v1, tmpv ); VectorCopy( v2, v1 ); VectorCopy( tmpv, v2 );
					} //end if
					if ( x3 > x4 ) {
						tmp = x3; x3 = x4; x4 = tmp;
						tmp = y3; y3 = y4; y4 = tmp;
						VectorCopy( v3, tmpv ); VectorCopy( v4, v3 ); VectorCopy( tmpv, v4 );
					} //end if
					  //if the two projected edge lines have no overlap
					if ( x2 <= x3 || x4 <= x1 ) {
//						Log_Write("lines no overlap: from area %d to %d\r\n", area1num, area2num);
						continue;
					} //end if
					  //if the two lines fully overlap
					if ( ( x1 - 0.5 < x3 && x4 < x2 + 0.5 ) &&
						 ( x3 - 0.5 < x1 && x2 < x4 + 0.5 ) ) {
						dist1 = y3 - y1;
						dist2 = y4 - y2;
						VectorCopy( v1, p1area1 );
						VectorCopy( v2, p2area1 );
						VectorCopy( v3, p1area2 );
						VectorCopy( v4, p2area2 );
					} //end if
					else
					{
						//if the points are equal
						if ( x1 > x3 - 0.1 && x1 < x3 + 0.1 ) {
							dist1 = y3 - y1;
							VectorCopy( v1, p1area1 );
							VectorCopy( v3, p1area2 );
						} //end if
						else if ( x1 < x3 ) {
							y = y1 + ( x3 - x1 ) * ( y2 - y1 ) / ( x2 - x1 );
							dist1 = y3 - y;
							VectorCopy( v3, p1area1 );
							p1area1[2] = y;
							VectorCopy( v3, p1area2 );
						} //end if
						else
						{
							y = y3 + ( x1 - x3 ) * ( y4 - y3 ) / ( x4 - x3 );
							dist1 = y - y1;
							VectorCopy( v1, p1area1 );
							VectorCopy( v1, p1area2 );
							p1area2[2] = y;
						} //end if
						  //if the points are equal
						if ( x2 > x4 - 0.1 && x2 < x4 + 0.1 ) {
							dist2 = y4 - y2;
							VectorCopy( v2, p2area1 );
							VectorCopy( v4, p2area2 );
						} //end if
						else if ( x2 < x4 ) {
							y = y3 + ( x2 - x3 ) * ( y4 - y3 ) / ( x4 - x3 );
							dist2 = y - y2;
							VectorCopy( v2, p2area1 );
							VectorCopy( v2, p2area2 );
							p2area2[2] = y;
						} //end if
						else
						{
							y = y1 + ( x4 - x1 ) * ( y2 - y1 ) / ( x2 - x1 );
							dist2 = y4 - y;
							VectorCopy( v4, p2area1 );
							p2area1[2] = y;
							VectorCopy( v4, p2area2 );
						} //end else
					} //end else
					  //if both distances are pretty much equal
					  //then we take the middle of the points
					if ( dist1 > dist2 - 1 && dist1 < dist2 + 1 ) {
						dist = dist1;
						VectorAdd( p1area1, p2area1, start );
						VectorScale( start, 0.5, start );
						VectorAdd( p1area2, p2area2, end );
						VectorScale( end, 0.5, end );
					} //end if
					else if ( dist1 < dist2 ) {
						dist = dist1;
						VectorCopy( p1area1, start );
						VectorCopy( p1area2, end );
					} //end else if
					else
					{
						dist = dist2;
						VectorCopy( p2area1, start );
						VectorCopy( p2area2, end );
					} //end else
					  //get the length of the overlapping part of the edges of the two areas
					VectorSubtract( p2area2, p1area2, dir );
					length = VectorLength( dir );
					//
					if ( groundface1->faceflags & FACE_GROUND ) {
						//if the vertical distance is smaller
						if ( dist < ground_bestdist ||
							 //or the vertical distance is pretty much the same
							 //but the overlapping part of the edges is longer
							 ( dist < ground_bestdist + 1 && length > ground_bestlength ) ) {
							ground_bestdist = dist;
							ground_bestlength = length;
							ground_foundreach = true;
							ground_bestarea2groundedgenum = edge1num;
							//best point towards area1
							VectorCopy( start, ground_beststart );
							//normal is pointing into area2
							VectorCopy( normal, ground_bestnormal );
							//best point towards area2
							VectorCopy( end, ground_bestend );
						} //end if
					} //end if
					else
					{
						//if the vertical distance is smaller
						if ( dist < water_bestdist ||
							 //or the vertical distance is pretty much the same
							 //but the overlapping part of the edges is longer
							 ( dist < water_bestdist + 1 && length > water_bestlength ) ) {
							water_bestdist = dist;
							water_bestlength = length;
							water_foundreach = true;
							water_bestarea2groundedgenum = edge1num;
							//best point towards area1
							VectorCopy( start, water_beststart );
							//normal is pointing into area2
							VectorCopy( normal, water_bestnormal );
							//best point towards area2
							VectorCopy( end, water_bestend );
						} //end if
					} //end else
				} //end for
			} //end for
		} //end for
	} //end for
	  //
	  // NOTE: swim reachabilities are already filtered out
	  //
	  // Steps
	  //
	  //        ---------
	  //        |          step height -> TRAVEL_WALK
	  //--------|
	  //
	  //        ---------
	  //~~~~~~~~|          step height and low water -> TRAVEL_WALK
	  //--------|
	  //
	  //~~~~~~~~~~~~~~~~~~
	  //        ---------
	  //        |          step height and low water up to the step -> TRAVEL_WALK
	  //--------|
	  //
	  //check for a step reachability
	if ( ground_foundreach ) {
		//if area2 is higher but lower than the maximum step height
		//NOTE: ground_bestdist >= 0 also catches equal floor reachabilities
		if ( ground_bestdist >= 0 && ground_bestdist < aassettings.sv_maxstep ) {
			//create walk reachability from area1 to area2
			lreach = AAS_AllocReachability();
			if ( !lreach ) {
				return false;
			}
			lreach->areanum = area2num;
			lreach->facenum = 0;
			lreach->edgenum = ground_bestarea2groundedgenum;
			VectorMA( ground_beststart, INSIDEUNITS_WALKSTART, ground_bestnormal, lreach->start );
			VectorMA( ground_bestend, INSIDEUNITS_WALKEND, ground_bestnormal, lreach->end );
			lreach->traveltype = TRAVEL_WALK;
			lreach->traveltime = 0; //1;
			//if going into a crouch area
			if ( !AAS_AreaCrouch( area1num ) && AAS_AreaCrouch( area2num ) ) {
				lreach->traveltime += STARTCROUCH_TIME;
			} //end if
			lreach->next = areareachability[area1num];
			areareachability[area1num] = lreach;
			//NOTE: if there's nearby solid or a gap area after this area
			/*
			if (!AAS_NearbySolidOrGap(lreach->start, lreach->end))
			{
				lreach->traveltime += 100;
			} //end if
			*/
			//avoid rather small areas
			//if (AAS_AreaGroundFaceArea(lreach->areanum) < 500) lreach->traveltime += 100;
			//
			reach_step++;
			return true;
		} //end if
	} //end if
	  //
	  // Water Jumps
	  //
	  //        ---------
	  //        |
	  //~~~~~~~~|
	  //        |
	  //        |          higher than step height and water up to waterjump height -> TRAVEL_WATERJUMP
	  //--------|
	  //
	  //~~~~~~~~~~~~~~~~~~
	  //        ---------
	  //        |
	  //        |
	  //        |
	  //        |          higher than step height and low water up to the step -> TRAVEL_WATERJUMP
	  //--------|
	  //
	  //check for a waterjump reachability
	if ( water_foundreach ) {
		//get a test point a little bit towards area1
		VectorMA( water_bestend, -INSIDEUNITS, water_bestnormal, testpoint );
		//go down the maximum waterjump height
		testpoint[2] -= aassettings.sv_maxwaterjump;
		//if there IS water the sv_maxwaterjump height below the bestend point
		if ( ( *aasworld ).areasettings[AAS_PointAreaNum( testpoint )].areaflags & AREA_LIQUID ) {
			//don't create rediculous water jump reachabilities from areas very far below
			//the water surface
			if ( water_bestdist < aassettings.sv_maxwaterjump + 24 ) {
				//waterjumping from or towards a crouch only area is not possible in Quake2
				if ( ( ( *aasworld ).areasettings[area1num].presencetype & PRESENCE_NORMAL ) &&
					 ( ( *aasworld ).areasettings[area2num].presencetype & PRESENCE_NORMAL ) ) {
					//create water jump reachability from area1 to area2
					lreach = AAS_AllocReachability();
					if ( !lreach ) {
						return false;
					}
					lreach->areanum = area2num;
					lreach->facenum = 0;
					lreach->edgenum = water_bestarea2groundedgenum;
					VectorCopy( water_beststart, lreach->start );
					VectorMA( water_bestend, INSIDEUNITS_WATERJUMP, water_bestnormal, lreach->end );
					lreach->traveltype = TRAVEL_WATERJUMP;
					lreach->traveltime = WATERJUMP_TIME;
					lreach->next = areareachability[area1num];
					areareachability[area1num] = lreach;
					//we've got another waterjump reachability
					reach_waterjump++;
					return true;
				} //end if
			} //end if
		} //end if
	} //end if
	  //
	  // Barrier Jumps
	  //
	  //        ---------
	  //        |
	  //        |
	  //        |
	  //        |         higher than step height lower than barrier height -> TRAVEL_BARRIERJUMP
	  //--------|
	  //
	  //        ---------
	  //        |
	  //        |
	  //        |
	  //~~~~~~~~|         higher than step height lower than barrier height
	  //--------|         and a thin layer of water in the area to jump from -> TRAVEL_BARRIERJUMP
	  //
	  //check for a barrier jump reachability
	if ( ground_foundreach ) {
		//if area2 is higher but lower than the maximum barrier jump height
		if ( ground_bestdist > 0 && ground_bestdist < aassettings.sv_maxbarrier ) {
			//if no water in area1 or a very thin layer of water on the ground
			if ( !water_foundreach || ( ground_bestdist - water_bestdist < 16 ) ) {
				//cannot perform a barrier jump towards or from a crouch area in Quake2
				if ( !AAS_AreaCrouch( area1num ) && !AAS_AreaCrouch( area2num ) ) {
					//create barrier jump reachability from area1 to area2
					lreach = AAS_AllocReachability();
					if ( !lreach ) {
						return false;
					}
					lreach->areanum = area2num;
					lreach->facenum = 0;
					lreach->edgenum = ground_bestarea2groundedgenum;
					VectorMA( ground_beststart, INSIDEUNITS_WALKSTART, ground_bestnormal, lreach->start );
					VectorMA( ground_bestend, INSIDEUNITS_WALKEND, ground_bestnormal, lreach->end );
					lreach->traveltype = TRAVEL_BARRIERJUMP;
					lreach->traveltime = BARRIERJUMP_TIME; //AAS_BarrierJumpTravelTime();
					lreach->next = areareachability[area1num];
					areareachability[area1num] = lreach;
					//we've got another barrierjump reachability
					reach_barrier++;
					return true;
				} //end if
			} //end if
		} //end if
	} //end if
	  //
	  // Walk and Walk Off Ledge
	  //
	  //--------|
	  //        |          can walk or step back -> TRAVEL_WALK
	  //        ---------
	  //
	  //--------|
	  //        |
	  //        |
	  //        |
	  //        |          cannot walk/step back -> TRAVEL_WALKOFFLEDGE
	  //        ---------
	  //
	  //--------|
	  //        |
	  //        |~~~~~~~~
	  //        |
	  //        |          cannot step back but can waterjump back -> TRAVEL_WALKOFFLEDGE
	  //        ---------  FIXME: create TRAVEL_WALK reach??
	  //
	  //check for a walk or walk off ledge reachability
	if ( ground_foundreach ) {
		if ( ground_bestdist < 0 ) {
			if ( ground_bestdist > -aassettings.sv_maxstep ) {
				//create walk reachability from area1 to area2
				lreach = AAS_AllocReachability();
				if ( !lreach ) {
					return false;
				}
				lreach->areanum = area2num;
				lreach->facenum = 0;
				lreach->edgenum = ground_bestarea2groundedgenum;
				VectorMA( ground_beststart, INSIDEUNITS_WALKSTART, ground_bestnormal, lreach->start );

				// Ridah
//				VectorMA(ground_bestend, INSIDEUNITS_WALKEND, ground_bestnormal, lreach->end);
				VectorMA( ground_bestend, INSIDEUNITS_WALKOFFLEDGEEND, ground_bestnormal, lreach->end );

				lreach->traveltype = TRAVEL_WALK;
				lreach->traveltime = 1;
				lreach->next = areareachability[area1num];
				areareachability[area1num] = lreach;
				//we've got another walk reachability
				reach_walk++;
				return true;
			} //end if
			  //trace a bounding box vertically to check for solids
			VectorMA( ground_bestend, INSIDEUNITS, ground_bestnormal, ground_bestend );
			VectorCopy( ground_bestend, start );
			start[2] = ground_beststart[2];
			VectorCopy( ground_bestend, end );
			end[2] += 4;
			trace = AAS_TraceClientBBox( start, end, PRESENCE_NORMAL, -1 );
			//if no solids were found
			if ( !trace.startsolid && trace.fraction >= 1.0 ) {
				//the trace end point must be in the goal area
				trace.endpos[2] += 1;
				if ( AAS_PointAreaNum( trace.endpos ) == area2num ) {
					//create a walk off ledge reachability from area1 to area2
					lreach = AAS_AllocReachability();
					if ( !lreach ) {
						return false;
					}
					lreach->areanum = area2num;
					lreach->facenum = 0;
					lreach->edgenum = ground_bestarea2groundedgenum;
					VectorCopy( ground_beststart, lreach->start );
					VectorCopy( ground_bestend, lreach->end );
					lreach->traveltype = TRAVEL_WALKOFFLEDGE;
					lreach->traveltime = STARTWALKOFFLEDGE_TIME + fabs( ground_bestdist ) * 50 / aassettings.sv_gravity;
					//if falling from too high and not falling into water
					if ( !AAS_AreaSwim( area2num ) && !AAS_AreaJumpPad( area2num ) ) {
						if ( AAS_FallDelta( ground_bestdist ) > FALLDELTA_5DAMAGE ) {
							lreach->traveltime += FALLDAMAGE_5_TIME;
						} //end if
						if ( AAS_FallDelta( ground_bestdist ) > FALLDELTA_10DAMAGE ) {
							lreach->traveltime += FALLDAMAGE_10_TIME;
						} //end if
					} //end if
					lreach->next = areareachability[area1num];
					areareachability[area1num] = lreach;
					//
					reach_walkoffledge++;
					//NOTE: don't create a weapon (rl, bfg) jump reachability here
					//because it interferes with other reachabilities
					//like the ladder reachability
					return true;
				} //end if
			} //end if
		} //end else
	} //end if
	return false;
} //end of the function AAS_Reachability_Step_Barrier_WaterJump_WalkOffLedge
//===========================================================================
// returns the distance between the two vectors
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
/* Ridah, moved to q_math.cpp
float VectorDistance(vec3_t v1, vec3_t v2)
{
	vec3_t dir;

	VectorSubtract(v2, v1, dir);
	return VectorLength(dir);
} //end of the function VectorDistance
*/
//===========================================================================
// returns true if the first vector is between the last two vectors
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
int VectorBetweenVectors( vec3_t v, vec3_t v1, vec3_t v2 ) {
	vec3_t dir1, dir2;

	VectorSubtract( v, v1, dir1 );
	VectorSubtract( v, v2, dir2 );
	return ( DotProduct( dir1, dir2 ) <= 0 );
} //end of the function VectorBetweenVectors
//===========================================================================
// returns the mid point between the two vectors
//
// Parameter:				-
// Returns:					-
// Changes Globals:		-
//===========================================================================
void VectorMiddle( vec3_t v1, vec3_t v2, vec3_t middle ) {
	VectorAdd( v1, v2, middle );
	VectorScale( middle, 0.5, middle );
} //end of the function VectorMiddle
//===========================================================================
// calculate a range of points closest to each other on both edges
//
// Parameter:			beststart1		start of the range of points on edge v1-v2
//						beststart2		end of the range of points  on edge v1-v2
//						bestend1		start of the range of points on edge v3-v4
//						bestend2		end of the range of points  on edge v3-v4
//						bestdist		best distance so far
// Returns:				-
// Changes Globals:		-
//===========================================================================
/*
float AAS_ClosestEdgePoints(vec3_t v1, vec3_t v2, vec3_t v3, vec3_t v4,
							aas_plane_t *plane1, aas_plane_t *plane2,
							vec3_t beststart, vec3_t bestend, float bestdist)
{
	vec3_t dir1, dir2, p1, p2, p3, p4;
	float a1, a2, b1, b2, dist;
	int founddist;

	//edge vectors
	VectorSubtract(v2, v1, dir1);
	VectorSubtract(v4, v3, dir2);
	//get the horizontal directions
	dir1[2] = 0;
	dir2[2] = 0;
	//
	// p1 = point on an edge vector of area2 closest to v1
	// p2 = point on an edge vector of area2 closest to v2
	// p3 = point on an edge vector of area1 closest to v3
	// p4 = point on an edge vector of area1 closest to v4
	//
	if (dir2[0])
	{
		a2 = dir2[1] / dir2[0];
		b2 = v3[1] - a2 * v3[0];
		//point on the edge vector of area2 closest to v1
		p1[0] = (DotProduct(v1, dir2) - (a2 * dir2[0] + b2 * dir2[1])) / dir2[0];
		p1[1] = a2 * p1[0] + b2;
		//point on the edge vector of area2 closest to v2
		p2[0] = (DotProduct(v2, dir2) - (a2 * dir2[0] + b2 * dir2[1])) / dir2[0];
		p2[1] = a2 * p2[0] + b2;
	} //end if
	else
	{
		//point on the edge vector of area2 closest to v1
		p1[0] = v3[0];
		p1[1] = v1[1];
		//point on the edge vector of area2 closest to v2
		p2[0] = v3[0];
		p2[1] = v2[1];
	} //end else
	//
	if (dir1[0])
	{
		//
		a1 = dir1[1] / dir1[0];
		b1 = v1[1] - a1 * v1[0];
		//point on the edge vector of area1 closest to v3
		p3[0] = (DotProduct(v3, dir1) - (a1 * dir1[0] + b1 * dir1[1])) / dir1[0];
		p3[1] = a1 * p3[0] + b1;
		//point on the edge vector of area1 closest to v4
		p4[0] = (DotProduct(v4, dir1) - (a1 * dir1[0] + b1 * dir1[1])) / dir1[0];
		p4[1] = a1 * p4[0] + b1;
	} //end if
	else
	{
		//point on the edge vector of area1 closest to v3
		p3[0] = v1[0];
		p3[1] = v3[1];
		//point on the edge vector of area1 closest to v4
		p4[0] = v1[0];
		p4[1] = v4[1];
	} //end else
	//start with zero z-coordinates
	p1[2] = 0;
	p2[2] = 0;
	p3[2] = 0;
	p4[2] = 0;
	//calculate the z-coordinates from the ground planes
	p1[2] = (plane2->dist - DotProduct(plane2->normal, p1)) / plane2->normal[2];
	p2[2] = (plane2->dist - DotProduct(plane2->normal, p2)) / plane2->normal[2];
	p3[2] = (plane1->dist - DotProduct(plane1->normal, p3)) / plane1->normal[2];
	p4[2] = (plane1->dist - DotProduct(plane1->normal, p4)) / plane1->normal[2];
	//
	founddist = false;
	//
	if (VectorBetweenVectors(p1, v3, v4))
	{
		dist = VectorDistance(v1, p1);
		if (dist > bestdist - 0.5 && dist < bestdist + 0.5)
		{
			VectorMiddle(beststart, v1, beststart);
			VectorMiddle(bestend, p1, bestend);
		} //end if
		else if (dist < bestdist)
		{
			bestdist = dist;
			VectorCopy(v1, beststart);
			VectorCopy(p1, bestend);
		} //end if
		founddist = true;
	} //end if
	if (VectorBetweenVectors(p2, v3, v4))
	{
		dist = VectorDistance(v2, p2);
		if (dist > bestdist - 0.5 && dist < bestdist + 0.5)
		{
			VectorMiddle(beststart, v2, beststart);
			VectorMiddle(bestend, p2, bestend);
		} //end if
		else if (dist < bestdist)
		{
			bestdist = dist;
			VectorCopy(v2, beststart);
			VectorCopy(p2, bestend);
		} //end if
		founddist = true;
	} //end else if
	if (VectorBetweenVectors(p3, v1, v2))
	{
		dist = VectorDistance(v3, p3);
		if (dist > bestdist - 0.5 && dist < bestdist + 0.5)
		{
			VectorMiddle(beststart, p3, beststart);
			VectorMiddle(bestend, v3, bestend);
		} //end if
		else if (dist < bestdist)
		{
			bestdist = dist;
			VectorCopy(p3, beststart);
			VectorCopy(v3, bestend);
		} //end if
		founddist = true;
	} //end else if
	if (VectorBetweenVectors(p4, v1, v2))
	{
		dist = VectorDistance(v4, p4);
		if (dist > bestdist - 0.5 && dist < bestdist + 0.5)
		{
			VectorMiddle(beststart, p4, beststart);
			VectorMiddle(bestend, v4, bestend);
		} //end if
		else if (dist < bestdist)
		{
			bestdist = dist;
			VectorCopy(p4, beststart);
			VectorCopy(v4, bestend);
		} //end if
		founddist = true;
	} //end else if
	//if no shortest distance was found the shortest distance
	//is between one of the vertexes of edge1 and one of edge2
	if (!founddist)
	{
		dist = VectorDistance(v1, v3);
		if (dist < bestdist)
		{
			bestdist = dist;
			VectorCopy(v1, beststart);
			VectorCopy(v3, bestend);
		} //end if
		dist = VectorDistance(v1, v4);
		if (dist < bestdist)
		{
			bestdist = dist;
			VectorCopy(v1, beststart);
			VectorCopy(v4, bestend);
		} //end if
		dist = VectorDistance(v2, v3);
		if (dist < bestdist)
		{
			bestdist = dist;
			VectorCopy(v2, beststart);
			VectorCopy(v3, bestend);
		} //end if
		dist = VectorDistance(v2, v4);
		if (dist < bestdist)
		{
			bestdist = dist;
			VectorCopy(v2, beststart);
			VectorCopy(v4, bestend);
		} //end if
	} //end if
	return bestdist;
} //end of the function AAS_ClosestEdgePoints*/

float AAS_ClosestEdgePoints( vec3_t v1, vec3_t v2, vec3_t v3, vec3_t v4,
							 aas_plane_t *plane1, aas_plane_t *plane2,
							 vec3_t beststart1, vec3_t bestend1,
							 vec3_t beststart2, vec3_t bestend2, float bestdist ) {
	vec3_t dir1, dir2, p1, p2, p3, p4;
	float a1, a2, b1, b2, dist, dist1, dist2;
	int founddist;

	//edge vectors
	VectorSubtract( v2, v1, dir1 );
	VectorSubtract( v4, v3, dir2 );
	//get the horizontal directions
	dir1[2] = 0;
	dir2[2] = 0;
	//
	// p1 = point on an edge vector of area2 closest to v1
	// p2 = point on an edge vector of area2 closest to v2
	// p3 = point on an edge vector of area1 closest to v3
	// p4 = point on an edge vector of area1 closest to v4
	//
	if ( dir2[0] ) {
		a2 = dir2[1] / dir2[0];
		b2 = v3[1] - a2 * v3[0];
		//point on the edge vector of area2 closest to v1
		p1[0] = ( DotProduct( v1, dir2 ) - ( a2 * dir2[0] + b2 * dir2[1] ) ) / dir2[0];
		p1[1] = a2 * p1[0] + b2;
		//point on the edge vector of area2 closest to v2
		p2[0] = ( DotProduct( v2, dir2 ) - ( a2 * dir2[0] + b2 * dir2[1] ) ) / dir2[0];
		p2[1] = a2 * p2[0] + b2;
	} //end if
	else
	{
		//point on the edge vector of area2 closest to v1
		p1[0] = v3[0];
		p1[1] = v1[1];
		//point on the edge vector of area2 closest to v2
		p2[0] = v3[0];
		p2[1] = v2[1];
	} //end else
	  //
	if ( dir1[0] ) {
		//
		a1 = dir1[1] / dir1[0];
		b1 = v1[1] - a1 * v1[0];
		//point on the edge vector of area1 closest to v3
		p3[0] = ( DotProduct( v3, dir1 ) - ( a1 * dir1[0] + b1 * dir1[1] ) ) / dir1[0];
		p3[1] = a1 * p3[0] + b1;
		//point on the edge vector of area1 closest to v4
		p4[0] = ( DotProduct( v4, dir1 ) - ( a1 * dir1[0] + b1 * dir1[1] ) ) / dir1[0];
		p4[1] = a1 * p4[0] + b1;
	} //end if
	else
	{
		//point on the edge vector of area1 closest to v3
		p3[0] = v1[0];
		p3[1] = v3[1];
		//point on the edge vector of area1 closest to v4
		p4[0] = v1[0];
		p4[1] = v4[1];
	} //end else
	  //start with zero z-coordinates
	p1[2] = 0;
	p2[2] = 0;
	p3[2] = 0;
	p4[2] = 0;
	//calculate the z-coordinates from the ground planes
	p1[2] = ( plane2->dist - DotProduct( plane2->normal, p1 ) ) / plane2->normal[2];
	p2[2] = ( plane2->dist - DotProduct( plane2->normal, p2 ) ) / plane2->normal[2];
	p3[2] = ( plane1->dist - DotProduct( plane1->normal, p3 ) ) / plane1->normal[2];
	p4[2] = ( plane1->dist - DotProduct( plane1->normal, p4 ) ) / plane1->normal[2];
	//
	founddist = false;
	//
	if ( VectorBetweenVectors( p1, v3, v4 ) ) {
		dist = VectorDistance( v1, p1 );
		if ( dist > bestdist - 0.5 && dist < bestdist + 0.5 ) {
			dist1 = VectorDistance( beststart1, v1 );
			dist2 = VectorDistance( beststart2, v1 );
			if ( dist1 > dist2 ) {
				if ( dist1 > VectorDistance( beststart1, beststart2 ) ) {
					VectorCopy( v1, beststart2 );
				}
			} //end if
			else
			{
				if ( dist2 > VectorDistance( beststart1, beststart2 ) ) {
					VectorCopy( v1, beststart1 );
				}
			} //end else
			dist1 = VectorDistance( bestend1, p1 );
			dist2 = VectorDistance( bestend2, p1 );
			if ( dist1 > dist2 ) {
				if ( dist1 > VectorDistance( bestend1, bestend2 ) ) {
					VectorCopy( p1, bestend2 );
				}
			} //end if
			else
			{
				if ( dist2 > VectorDistance( bestend1, bestend2 ) ) {
					VectorCopy( p1, bestend1 );
				}
			} //end else
		} //end if
		else if ( dist < bestdist ) {
			bestdist = dist;
			VectorCopy( v1, beststart1 );
			VectorCopy( v1, beststart2 );
			VectorCopy( p1, bestend1 );
			VectorCopy( p1, bestend2 );
		} //end if
		founddist = true;
	} //end if
	if ( VectorBetweenVectors( p2, v3, v4 ) ) {
		dist = VectorDistance( v2, p2 );
		if ( dist > bestdist - 0.5 && dist < bestdist + 0.5 ) {
			dist1 = VectorDistance( beststart1, v2 );
			dist2 = VectorDistance( beststart2, v2 );
			if ( dist1 > dist2 ) {
				if ( dist1 > VectorDistance( beststart1, beststart2 ) ) {
					VectorCopy( v2, beststart2 );
				}
			} //end if
			else
			{
				if ( dist2 > VectorDistance( beststart1, beststart2 ) ) {
					VectorCopy( v2, beststart1 );
				}
			} //end else
			dist1 = VectorDistance( bestend1, p2 );
			dist2 = VectorDistance( bestend2, p2 );
			if ( dist1 > dist2 ) {
				if ( dist1 > VectorDistance( bestend1, bestend2 ) ) {
					VectorCopy( p2, bestend2 );
				}
			} //end if
			else
			{
				if ( dist2 > VectorDistance( bestend1, bestend2 ) ) {
					VectorCopy( p2, bestend1 );
				}
			} //end else
		} //end if
		else if ( dist < bestdist ) {
			bestdist = dist;
			VectorCopy( v2, beststart1 );
			VectorCopy( v2, beststart2 );
			VectorCopy( p2, bestend1 );
			VectorCopy( p2, bestend2 );
		} //end if
		founddist = true;
	} //end else if
	if ( VectorBetweenVectors( p3, v1, v2 ) ) {
		dist = VectorDistance( v3, p3 );
		if ( dist > bestdist - 0.5 && dist < bestdist + 0.5 ) {
			dist1 = VectorDistance( beststart1, p3 );
			dist2 = VectorDistance( beststart2, p3 );
			if ( dist1 > dist2 ) {
				if ( dist1 > VectorDistance( beststart1, beststart2 ) ) {
					VectorCopy( p3, beststart2 );
				}
			} //end if
			else
			{
				if ( dist2 > VectorDistance( beststart1, beststart2 ) ) {
