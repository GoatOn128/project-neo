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

// unix_shared.cpp: the Linux replacements for the Win_* bridges the SDL
// files call into. SDL owns the clock and the process here, so the
// functions are just the no-op half of the Windows implementation in
// sys/win32/win_shared.cpp.

#include "../game/q_shared.h"
#include "../sys_local.h"

void Win_SetProcessErrorMode( void ) {
	// there are no abort/retry/fail popups on Linux
}

void Win_SetTimerResolution( bool enable ) {
	// the Linux timers are high precision already
	( void )enable;
}

bool Win_PumpConsoleMessages( void ) {
	// there is no console window to pump; false makes Sys_Error quit
	return false;
}
