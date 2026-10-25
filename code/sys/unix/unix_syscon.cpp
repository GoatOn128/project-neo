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

// unix_syscon.cpp: the terminal console for the Linux build. A small SDL
// thread reads stdin lines into a slot the engine polls through
// Sys_ConsoleInput, and Conbuf_AppendText prints to stdout, the same roles
// the Win32 console window code plays on Windows.

#include "../client/client.h"
#include "../sys_local.h"

#include <SDL3/SDL.h>

#include <stdio.h>
#include <string.h>

static SDL_Mutex *conLock;
static char conLine[1024];
static bool conHasLine;

static int SDLCALL ConsoleThread( void * ) {
	char buffer[sizeof( conLine )];
	size_t length;

	while ( fgets( buffer, sizeof( buffer ), stdin ) ) {
		// drop the newline, the old console code did the same
		length = strlen( buffer );
		if ( length && buffer[length - 1] == '\n' ) {
			buffer[length - 1] = 0;
		}

		SDL_LockMutex( conLock );
		Q_strncpyz( conLine, buffer, sizeof( conLine ) );
		conHasLine = true;
		SDL_UnlockMutex( conLock );
	}

	return 0;
}

void Sys_CreateConsole( void ) {
	if ( !conLock ) {
		conLock = SDL_CreateMutex();
		SDL_DetachThread( SDL_CreateThread( ConsoleThread, "console", NULL ) );
	}
}

void Sys_DestroyConsole( void ) {
}

void Sys_ShowConsole( int visLevel, bool quitOnClose ) {
	( void )visLevel;
	( void )quitOnClose;
}

void Sys_SetErrorText( const char *text ) {
	fprintf( stderr, "********************\nERROR: %s\n********************\n", text );
}

char *Sys_ConsoleInput( void ) {
	static char returned[ sizeof( conLine ) ];
	bool has;

	if ( !conLock ) {
		return NULL;
	}

	SDL_LockMutex( conLock );
	has = conHasLine;
	if ( has ) {
		Q_strncpyz( returned, conLine, sizeof( returned ) );
		conHasLine = false;
	}
	SDL_UnlockMutex( conLock );

	return has ? returned : NULL;
}

void Conbuf_AppendText( const char *pMsg ) {
	char buffer[8192];
	int n = 0;

	// strip the Quake color codes, drop carriage returns, print the rest
	for ( const char *p = pMsg; *p && n < ( int )sizeof( buffer ) - 1; p++ ) {
		if ( Q_IsColorString( p ) ) {
			p++;
			continue;
		}
		if ( *p == '\r' ) {
			continue;
		}
		buffer[n++] = *p;
	}
	buffer[n] = 0;

	fputs( buffer, stdout );
	fflush( stdout );
}
