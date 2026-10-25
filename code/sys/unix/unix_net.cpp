/*
===========================================================================

Return to Castle Wolfenstein single player GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of the Return to Castle Wolfenstein single player GPL Source Code ("RTCW SP Source Code").

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

===========================================================================
*/

// unix_net.cpp: the BSD sockets networking for the Linux build. It exports
// the same surface as the Winsock file; IPX and SOCKS stayed on Windows.

#include "../game/q_shared.h"
#include "../../qcommon/qcommon.h"
#include "../sys_local.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static bool networkingEnabled = false;

static cvar_t   *net_noudp;
static cvar_t   *net_noipx;

static int ip_socket;

#define MAX_IPS     16
static int numIP;
static byte localIP[MAX_IPS][4];

//=============================================================================

int recvfromCount;

static char *NET_ErrorString( void ) {
	return ( char * )strerror( errno );
}

void NetadrToSockadr( netadr_t *a, struct sockaddr *s ) {
	memset( s, 0, sizeof( *s ) );

	if ( a->type == NA_BROADCAST ) {
		( ( struct sockaddr_in * )s )->sin_family = AF_INET;
		( ( struct sockaddr_in * )s )->sin_port = a->port;
		( ( struct sockaddr_in * )s )->sin_addr.s_addr = INADDR_BROADCAST;
	} else if ( a->type == NA_IP ) {
		( ( struct sockaddr_in * )s )->sin_family = AF_INET;
		( ( struct sockaddr_in * )s )->sin_addr.s_addr = *( int * )&a->ip;
		( ( struct sockaddr_in * )s )->sin_port = a->port;
	}
}

void SockadrToNetadr( struct sockaddr *s, netadr_t *a ) {
	if ( s->sa_family == AF_INET ) {
		a->type = NA_IP;
		*( int * )&a->ip = ( ( struct sockaddr_in * )s )->sin_addr.s_addr;
		a->port = ( ( struct sockaddr_in * )s )->sin_port;
	}
}

/*
=============
Sys_StringToAdr

idnewt
192.246.40.70
=============
*/
bool Sys_StringToSockaddr( const char *s, struct sockaddr *sadr ) {
	struct hostent  *h;
	struct in_addr addr;

	memset( sadr, 0, sizeof( *sadr ) );

	( ( struct sockaddr_in * )sadr )->sin_family = AF_INET;
	( ( struct sockaddr_in * )sadr )->sin_port = 0;

	if ( Q_isnumeric( s[0] ) ) {
		if ( !inet_aton( s, &addr ) ) {
			return false;
		}
		*( struct in_addr * )&( ( struct sockaddr_in * )sadr )->sin_addr = addr;
	} else {
		if ( ( h = gethostbyname( s ) ) == 0 ) {
			return false;
		}
		*( struct in_addr * )&( ( struct sockaddr_in * )sadr )->sin_addr = *( struct in_addr * )h->h_addr_list[0];
	}

	return true;
}

bool Sys_StringToAdr( const char *s, netadr_t *a ) {
	struct sockaddr sadr;

	if ( !Sys_StringToSockaddr( s, &sadr ) ) {
		return false;
	}

	SockadrToNetadr( &sadr, a );
	return true;
}

//=============================================================================

/*
==================
Sys_GetPacket

Never called by the game logic, just the system event queing
==================
*/
bool Sys_GetPacket( netadr_t *net_from, msg_t *net_message ) {
	int ret;
	struct sockaddr from;
	socklen_t fromlen;

	if ( !ip_socket ) {
		return false;
	}

	fromlen = sizeof( from );
	recvfromCount++;        // performance check
	ret = recvfrom( ip_socket, ( char * )( net_message->data ), net_message->maxsize, 0, &from, &fromlen );
	if ( ret == -1 ) {
		const int err = errno;

		// wouldblock and a reset connection are silent
		if ( err == EWOULDBLOCK || err == ECONNRESET ) {
			return false;
		}

		Com_Printf( "NET_GetPacket: %s\n", NET_ErrorString() );
		return false;
	}

	SockadrToNetadr( &from, net_from );
	net_message->readcount = 0;

	if ( ret == net_message->maxsize ) {
		Com_Printf( "Oversize packet from %s\n", NET_AdrToString( *net_from ) );
		return false;
	}

	net_message->cursize = ret;
	return true;
}

//=============================================================================

/*
==================
Sys_SendPacket
==================
*/
void Sys_SendPacket( int length, const void *data, netadr_t to ) {
	int ret;
	struct sockaddr addr;

	if ( to.type == NA_BROADCAST ) {
		// falls through to the IP socket below
	} else if ( to.type == NA_IP ) {
		// falls through to the IP socket below
	} else {
		Com_Error( ERR_FATAL, "Sys_SendPacket: bad address type" );
		return;
	}

	if ( !ip_socket ) {
		return;
	}

	NetadrToSockadr( &to, &addr );

	ret = sendto( ip_socket, ( const char * )( data ), length, 0, &addr, sizeof( addr ) );
	if ( ret == -1 ) {
		const int err = errno;

		// wouldblock is silent
		if ( err == EWOULDBLOCK ) {
			return;
		}

		// some PPP links do not allow broadcasts and return an error
		if ( ( err == EADDRNOTAVAIL ) && ( to.type == NA_BROADCAST ) ) {
			return;
		}

		Com_Printf( "NET_SendPacket: %s\n", NET_ErrorString() );
	}
}

//=============================================================================

/*
==================
Sys_IsLANAddress

LAN clients will have their rate var ignored
==================
*/
bool Sys_IsLANAddress( netadr_t adr ) {
	int i;

	if ( adr.type == NA_LOOPBACK ) {
		return true;
	}

	if ( adr.type != NA_IP ) {
		return false;
	}

	// choose which comparison to use based on the class of the address being tested
	// any local adresses of a different class than the address being tested will fail based on the first byte

	if ( adr.ip[0] == 127 && adr.ip[1] == 0 && adr.ip[2] == 0 && adr.ip[3] == 1 ) {
		return true;
	}

	// Class A
	if ( ( adr.ip[0] & 0x80 ) == 0x00 ) {
		for ( i = 0 ; i < numIP ; i++ ) {
			if ( adr.ip[0] == localIP[i][0] ) {
				return true;
			}
		}
		// the RFC1918 class a block will pass the above test
		return false;
	}

	// Class B
	if ( ( adr.ip[0] & 0xc0 ) == 0x80 ) {
		for ( i = 0 ; i < numIP ; i++ ) {
			if ( adr.ip[0] == localIP[i][0] && adr.ip[1] == localIP[i][1] ) {
				return true;
			}
			// also check against the RFC1918 class b blocks
			if ( adr.ip[0] == 172 && localIP[i][0] == 172 && ( adr.ip[1] & 0xf0 ) == 16 && ( localIP[i][1] & 0xf0 ) == 16 ) {
				return true;
			}
		}
		return false;
	}

	// Class C
	for ( i = 0 ; i < numIP ; i++ ) {
		if ( adr.ip[0] == localIP[i][0] && adr.ip[1] == localIP[i][1] && adr.ip[2] == localIP[i][2] ) {
			return true;
		}
		// also check against the RFC1918 class c blocks
		if ( adr.ip[0] == 192 && localIP[i][0] == 192 && adr.ip[1] == 168 && localIP[i][1] == 168 ) {
			return true;
		}
	}
	return false;
}

/*
==================
Sys_ShowIP
==================
*/
void Sys_ShowIP( void ) {
	int i;

	for ( i = 0 ; i < numIP ; i++ ) {
		Com_Printf( "IP: %i.%i.%i.%i\n", localIP[i][0], localIP[i][1], localIP[i][2], localIP[i][3] );
	}
}

//=============================================================================

/*
====================
NET_IPSocket
====================
*/
static int NET_IPSocket( char *net_interface, int port ) {
	int newsocket;
	struct sockaddr_in address;
	int i = 1;
	int flags;

	if ( net_interface ) {
		Com_Printf( "Opening IP socket: %s:%i\n", net_interface, port );
	} else {
		Com_Printf( "Opening IP socket: localhost:%i\n", port );
	}

	if ( ( newsocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 ) {
		Com_Printf( "WARNING: UDP_OpenSocket: socket: %s\n", NET_ErrorString() );
		return 0;
	}

	// make it non-blocking
	flags = fcntl( newsocket, F_GETFL, 0 );
	if ( flags == -1 || fcntl( newsocket, F_SETFL, flags | O_NONBLOCK ) == -1 ) {
		Com_Printf( "WARNING: UDP_OpenSocket: fcntl O_NONBLOCK: %s\n", NET_ErrorString() );
		close( newsocket );
		return 0;
	}

	// make it broadcast capable
	if ( setsockopt( newsocket, SOL_SOCKET, SO_BROADCAST, ( char * )&i, sizeof( i ) ) == -1 ) {
		Com_Printf( "WARNING: UDP_OpenSocket: setsockopt SO_BROADCAST: %s\n", NET_ErrorString() );
		close( newsocket );
		return 0;
	}

	if ( !net_interface || !net_interface[0] || !Q_stricmp( net_interface, "localhost" ) ) {
		address.sin_addr.s_addr = INADDR_ANY;
	} else {
		Sys_StringToSockaddr( net_interface, ( struct sockaddr * )&address );
	}

	if ( port == PORT_ANY ) {
		address.sin_port = 0;
	} else {
		address.sin_port = htons( ( short )port );
	}

	address.sin_family = AF_INET;

	if ( bind( newsocket, ( const struct sockaddr * )( ( void * )&address ), sizeof( address ) ) == -1 ) {
		Com_Printf( "WARNING: UDP_OpenSocket: bind: %s\n", NET_ErrorString() );
		close( newsocket );
		return 0;
	}

	return newsocket;
}

/*
=====================
NET_GetLocalAddress
=====================
*/
static void NET_GetLocalAddress( void ) {
	char hostname[256];
	struct hostent      *hostInfo;
	char                *p;
	int ip;
	int n;

	if ( gethostname( hostname, sizeof( hostname ) ) == -1 ) {
		return;
	}

	hostInfo = gethostbyname( hostname );
	if ( !hostInfo ) {
		return;
	}

	Com_Printf( "Hostname: %s\n", hostInfo->h_name );
	n = 0;
	while ( ( p = hostInfo->h_aliases[n++] ) != NULL ) {
		Com_Printf( "Alias: %s\n", p );
	}

	if ( hostInfo->h_addrtype != AF_INET ) {
		return;
	}

	numIP = 0;
	while ( ( p = hostInfo->h_addr_list[numIP] ) != NULL && numIP < MAX_IPS ) {
		ip = ntohl( *( int * )p );
		localIP[ numIP ][0] = p[0];
		localIP[ numIP ][1] = p[1];
		localIP[ numIP ][2] = p[2];
		localIP[ numIP ][3] = p[3];
		Com_Printf( "IP: %i.%i.%i.%i\n", ( ip >> 24 ) & 0xff, ( ip >> 16 ) & 0xff, ( ip >> 8 ) & 0xff, ip & 0xff );
		numIP++;
	}
}

/*
====================
NET_OpenIP
====================
*/
static void NET_OpenIP( void ) {
	cvar_t  *ip;
	int port;
	int i;

	ip = Cvar_Get( "net_ip", "localhost", CVAR_LATCH );
	port = Cvar_Get( "net_port", va( "%i", PORT_SERVER ), CVAR_LATCH )->integer;

	// automatically scan for a valid port, so multiple
	// dedicated servers can be started without requiring
	// a different net_port for each one
	for ( i = 0 ; i < 10 ; i++ ) {
		ip_socket = NET_IPSocket( ip->string, port + i );
		if ( ip_socket ) {
			Cvar_SetValue( "net_port", port + i );
			NET_GetLocalAddress();
			return;
		}
	}
	Com_Printf( "WARNING: Couldn't allocate IP port\n" );
}

//===================================================================

/*
====================
NET_GetCvars
====================
*/
static bool NET_GetCvars( void ) {
	bool modified;

	modified = false;

	if ( net_noudp && net_noudp->modified ) {
		modified = true;
	}
	net_noudp = Cvar_Get( "net_noudp", "0", CVAR_LATCH | CVAR_ARCHIVE );

	if ( net_noipx && net_noipx->modified ) {
		modified = true;
	}
	net_noipx = Cvar_Get( "net_noipx", "0", CVAR_LATCH | CVAR_ARCHIVE );

	return modified;
}

/*
====================
NET_Config
====================
*/
void NET_Config( bool enableNetworking ) {
	bool modified;
	bool stop;
	bool start;

	// get any latched changes to cvars
	modified = NET_GetCvars();

	if ( net_noudp->integer && net_noipx->integer ) {
		enableNetworking = false;
	}

	// if enable state is the same and no cvars were modified, we have nothing to do
	if ( enableNetworking == networkingEnabled && !modified ) {
		return;
	}

	if ( enableNetworking == networkingEnabled ) {
		if ( enableNetworking ) {
			stop = true;
			start = true;
		} else {
			stop = false;
			start = false;
		}
	} else {
		if ( enableNetworking ) {
			stop = false;
			start = true;
		} else {
			stop = true;
			start = false;
		}
		networkingEnabled = enableNetworking;
	}

	if ( stop ) {
		if ( ip_socket ) {
			close( ip_socket );
			ip_socket = 0;
		}
	}

	if ( start ) {
		if ( !net_noudp->integer ) {
			NET_OpenIP();
		}
	}
}

/*
====================
NET_Init
====================
*/
void NET_Init( void ) {
	Com_Printf( "BSD sockets initialized\n" );

	// this is really just to get the cvars registered
	NET_GetCvars();

	//FIXME testing!
	NET_Config( true );
}

/*
====================
NET_Shutdown
====================
*/
void NET_Shutdown( void ) {
	NET_Config( false );
	networkingEnabled = false;
}

/*
====================
NET_Sleep

sleeps msec or until net socket is ready
====================
*/
void NET_Sleep( int msec ) {
	struct timeval timeout;
	fd_set fdset;

	if ( !ip_socket || msec < 0 ) {
		return;
	}

	FD_ZERO( &fdset );
	FD_SET( ip_socket, &fdset );
	timeout.tv_sec = msec / 1000;
	timeout.tv_usec = ( msec % 1000 ) * 1000;
	select( ip_socket + 1, &fdset, NULL, NULL, &timeout );
}

/*
====================
NET_Restart_f
====================
*/
void NET_Restart( void ) {
	NET_Config( networkingEnabled );
}
