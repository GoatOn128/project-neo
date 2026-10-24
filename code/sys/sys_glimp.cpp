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

In addition, the RTCW SP Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied RTCW SP Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

/*
** SYS_GLIMP.CPP
**
** This file contains all the platform specific stuff having to do with the
** OpenGL refresh, using SDL3 for the window and the GL context. The
** functions the renderer calls are unchanged: GLimp_EndFrame, GLimp_Init,
** GLimp_LogComment and GLimp_Shutdown. The SMP render thread still uses
** Win32 events, which is fine on the Windows target.
*/
#include <assert.h>
#include "../renderer/tr_local.h"
#include "../qcommon/qcommon.h"
#include "sys_local.h"
#include "win32/win_local.h"

typedef enum {
	RSERR_OK,

	RSERR_INVALID_FULLSCREEN,
	RSERR_INVALID_MODE,

	RSERR_UNKNOWN
} rserr_t;

static qboolean s_windowMaximized = qfalse;


//
// variable declarations
//
glwstate_t glw_state;

// exported through GetRefAPI so the engine can reach the window the
// renderer owns
SDL_Window *RE_GetMainWindow( void ) {
	return glw_state.window;
}

SDL_GLContext RE_GetMainContext( void ) {
	return glw_state.context;
}

cvar_t  *r_allowSoftwareGL;     // don't abort out if the pixelformat claims software
cvar_t  *r_maskMinidriver;      // kept for compatibility, SDL owns the driver now


/*
** GLW_SetMode

** Creates the window and the GL context for the requested video mode.
*/
static rserr_t GLW_SetMode( const char *drivername,
							int mode,
							int colorbits,
							qboolean cdsFullscreen ) {
	const char *win_fs[] = { "W", "FS" };
	cvar_t *vid_xpos, *vid_ypos;
	SDL_WindowFlags flags;
	int x, y;
	const SDL_DisplayMode *desktop;

	//
	// print out informational messages
	//
	ri.Printf( PRINT_ALL, "...setting mode %d:", mode );
	if ( !R_GetModeInfo( &glConfig.vidWidth, &glConfig.vidHeight, &glConfig.windowAspect, mode ) ) {
		ri.Printf( PRINT_ALL, " invalid mode\n" );
		return RSERR_INVALID_MODE;
	}
	ri.Printf( PRINT_ALL, " %d %d %s\n", glConfig.vidWidth, glConfig.vidHeight, win_fs[cdsFullscreen ? 1 : 0] );

	//
	// check our desktop attributes
	//
	desktop = SDL_GetDesktopDisplayMode( SDL_GetPrimaryDisplay() );
	if ( desktop ) {
		glw_state.desktopBitsPixel = 32;
		glw_state.desktopWidth = desktop->w;
		glw_state.desktopHeight = desktop->h;
		glConfig.displayFrequency = (int)desktop->refresh_rate;
	} else
	{
		// no display info, fall back to something sane
		glw_state.desktopBitsPixel = 32;
		glw_state.desktopWidth = 1920;
		glw_state.desktopHeight = 1080;
	}

	//
	// create the window if one does not already exist
	//
	if ( !glw_state.window ) {
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
		if ( cdsFullscreen ) {
			flags |= SDL_WINDOW_FULLSCREEN;
		}
		if ( ri.Cvar_Get( "r_hiddenWindow", "0", CVAR_TEMP )->integer ) {
			// the headless tests run the engine without showing a window
			flags |= SDL_WINDOW_HIDDEN;
		}

		glw_state.window = SDL_CreateWindow( "Doom", glConfig.vidWidth, glConfig.vidHeight, flags );
		if ( !glw_state.window ) {
			ri.Printf( PRINT_ALL, "...SDL_CreateWindow failed: %s\n", SDL_GetError() );
			return RSERR_INVALID_MODE;
		}

		if ( !cdsFullscreen ) {
			vid_xpos = ri.Cvar_Get( "vid_xpos", "3", CVAR_ARCHIVE );
			vid_ypos = ri.Cvar_Get( "vid_ypos", "22", CVAR_ARCHIVE );
			x = vid_xpos->integer;
			y = vid_ypos->integer;

			// adjust window coordinates if necessary
			// so that the window is completely on screen
			if ( x < 0 ) {
				x = 0;
			}
			if ( y < 0 ) {
				y = 0;
			}
			if ( glConfig.vidWidth < glw_state.desktopWidth &&
				 glConfig.vidHeight < glw_state.desktopHeight ) {
				if ( x + glConfig.vidWidth > glw_state.desktopWidth ) {
					x = ( glw_state.desktopWidth - glConfig.vidWidth );
				}
				if ( y + glConfig.vidHeight > glw_state.desktopHeight ) {
					y = ( glw_state.desktopHeight - glConfig.vidHeight );
				}
			}
			SDL_SetWindowPosition( glw_state.window, x, y );

			// remember a maximized window across vid_restart
			if ( s_windowMaximized ) {
				SDL_MaximizeWindow( glw_state.window );
			}
		}

		SDL_SetWindowMinimumSize( glw_state.window, 640, 480 );

		// publish the native handle for the editor bridge
		ri.SetGameWindow( SDL_GetPointerProperty(
			SDL_GetWindowProperties( glw_state.window ),
			SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL ) );

		ri.Printf( PRINT_ALL, "...created window %dx%d\n", glConfig.vidWidth, glConfig.vidHeight );
	} else
	{
		ri.Printf( PRINT_ALL, "...window already present, SDL_CreateWindow skipped\n" );
	}

	// SDL only delivers text input events while they are enabled on the window
	SDL_StartTextInput( glw_state.window );

	// SDL flips the window to the desktop resolution in fullscreen, so the
	// exact requested mode is a borderless desktop window either way
	glConfig.isFullscreen = cdsFullscreen;
	glw_state.cdsFullscreen = cdsFullscreen;

	return RSERR_OK;
}


/*
** GLW_CreateContext

** Sets the GL attributes and creates the context, two attempts like the
** old pixel format code: first the requested color/depth/stencil, then the
** desktop color bits without stencil.
*/
static qboolean GLW_CreateContext( int colorbits, int depthbits, int stencilbits, qboolean stereo ) {
	int value;

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, colorbits / 4 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, colorbits / 4 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, colorbits / 4 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, depthbits );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, stencilbits );

	if ( stereo ) {
		ri.Printf( PRINT_ALL, "...attempting to use stereo\n" );
		SDL_GL_SetAttribute( SDL_GL_STEREO, 1 );
		glConfig.stereoEnabled = qtrue;
	} else
	{
		SDL_GL_SetAttribute( SDL_GL_STEREO, 0 );
		glConfig.stereoEnabled = qfalse;
	}

	ri.Printf( PRINT_ALL, "...creating GL context: " );
	glw_state.context = SDL_GL_CreateContext( glw_state.window );
	if ( !glw_state.context ) {
		ri.Printf( PRINT_ALL, "failed (%s)\n", SDL_GetError() );
		return qfalse;
	}

	ri.Printf( PRINT_ALL, "succeeded\n" );

	ri.Printf( PRINT_ALL, "...making context current: " );
	if ( !SDL_GL_MakeCurrent( glw_state.window, glw_state.context ) ) {
		ri.Printf( PRINT_ALL, "failed (%s)\n", SDL_GetError() );
		SDL_GL_DestroyContext( glw_state.context );
		glw_state.context = NULL;
		return qfalse;
	}
	ri.Printf( PRINT_ALL, "succeeded\n" );

	/*
	** report if stereo is desired but unavailable
	*/
	if ( stereo && SDL_GL_GetAttribute( SDL_GL_STEREO, &value ) && !value ) {
		ri.Printf( PRINT_ALL, "...failed to select stereo pixel format\n" );
		glConfig.stereoEnabled = qfalse;
	}

	/*
	** store what the driver actually gave us
	*/
	if ( SDL_GL_GetAttribute( SDL_GL_RED_SIZE, &value ) ) {
		glConfig.colorBits = value * 4;
	}
	if ( SDL_GL_GetAttribute( SDL_GL_DEPTH_SIZE, &value ) ) {
		glConfig.depthBits = value;
	}
	if ( SDL_GL_GetAttribute( SDL_GL_STENCIL_SIZE, &value ) ) {
		glConfig.stencilBits = value;
	}

	return qtrue;
}


/*
** GLW_InitDriver

** Picks the depth/stencil sizes from the cvars and creates the context.
*/
static qboolean GLW_InitDriver( int colorbits ) {
	int depthbits, stencilbits;

	ri.Printf( PRINT_ALL, "Initializing OpenGL driver\n" );

	if ( colorbits == 0 ) {
		colorbits = glw_state.desktopBitsPixel;
	}

	//
	// implicitly assume Z-buffer depth == desktop color depth
	//
	if ( r_depthbits->integer == 0 ) {
		if ( colorbits > 16 ) {
			depthbits = 24;
		} else {
			depthbits = 16;
		}
	} else {
		depthbits = r_depthbits->integer;
	}

	//
	// do not allow stencil if Z-buffer depth likely won't contain it
	//
	stencilbits = r_stencilbits->integer;
	if ( depthbits < 24 ) {
		stencilbits = 0;
	}

	//
	// first attempt: r_colorbits, depthbits, and r_stencilbits
	//
	if ( !GLW_CreateContext( colorbits, depthbits, stencilbits, r_stereo->integer != 0 ) ) {
		//
		// second attempt: desktop's color bits and no stencil
		//
		if ( colorbits > glw_state.desktopBitsPixel ) {
			colorbits = glw_state.desktopBitsPixel;
		}
		if ( !GLW_CreateContext( colorbits, depthbits, 0, r_stereo->integer != 0 ) ) {
			ri.Printf( PRINT_ALL, "...failed to find an appropriate PIXELFORMAT\n" );
			return qfalse;
		}
	}

	return qtrue;
}


/*
** GLW_StartDriverAndSetMode
*/
static qboolean GLW_StartDriverAndSetMode( const char *drivername,
										  int mode,
										  int colorbits,
										  qboolean cdsFullscreen ) {
	rserr_t err;

	err = GLW_SetMode( drivername, mode, colorbits, cdsFullscreen );

	switch ( err )
	{
	case RSERR_INVALID_FULLSCREEN:
		ri.Printf( PRINT_ALL, "...WARNING: fullscreen unavailable in this mode\n" );
		return qfalse;
	case RSERR_INVALID_MODE:
		ri.Printf( PRINT_ALL, "...WARNING: could not set the given mode (%d)\n", mode );
		return qfalse;
	default:
		break;
	}

	if ( !GLW_InitDriver( colorbits ) ) {
		// the window is useless without a context
		if ( glw_state.window ) {
			SDL_DestroyWindow( glw_state.window );
			glw_state.window = NULL;
			ri.SetGameWindow( NULL );
		}
		return qfalse;
	}

	return qtrue;
}


/*
** GLW_InitExtensions
*/
static void GLW_InitExtensions( void ) {

	glConfig.textureCompression = TC_NONE;

	// the GL_ADD texture environment is core since OpenGL 1.3
	glConfig.textureEnvAddAvailable = qtrue;

	ri.Printf( PRINT_ALL, "Initializing OpenGL extensions\n" );

	// GL_EXT_texture_compression_s3tc
	if ( strstr( glConfig.extensions_string, "GL_EXT_texture_compression_s3tc" ) ) {
		if ( r_ext_compressed_textures->integer ) {
			glConfig.textureCompression = TC_EXT_COMP_S3TC;
			ri.Printf( PRINT_ALL, "...using GL_EXT_texture_compression_s3tc\n" );
		} else {
			glConfig.textureCompression = TC_NONE;
			ri.Printf( PRINT_ALL, "...ignoring GL_EXT_texture_compression_s3tc\n" );
		}
	} else {
		ri.Printf( PRINT_ALL, "...GL_EXT_texture_compression_s3tc not found\n" );
	}

	// multitexture is core since OpenGL 1.3
	qglGetIntegerv( GL_MAX_TEXTURE_UNITS, &glConfig.maxActiveTextures );
	ri.Printf( PRINT_ALL, "...using GL_ARB_multitexture (%i texture units)\n", glConfig.maxActiveTextures );

	// swap control goes through SDL_GL_SetSwapInterval now, force a set
	ri.Printf( PRINT_ALL, "...using SDL swap control\n" );
	r_swapInterval->modified = qtrue;   // force a set next frame
}


/*
** GLW_StartOpenGL

** SDL owns the driver, so this only sets the mode and creates the context,
** with the 1280x720 windowed fallback the old code had.
*/
static void GLW_StartOpenGL( void ) {
	qboolean cdsFullscreen;

	glConfig.driverType = GLDRV_ICD;

	cdsFullscreen = r_fullscreen->integer != 0;

	// create the window and set up the context
	if ( !GLW_StartDriverAndSetMode( "opengl32", r_mode->integer, r_colorbits->integer, cdsFullscreen ) ) {
		ri.Printf( PRINT_WARNING, "Falling back to 1280x720 windowed.\n" );
		if ( !GLW_StartDriverAndSetMode( "opengl32", 2, 32, qfalse ) ) {
			ri.Error( ERR_FATAL, "GLW_StartOpenGL() - could not load OpenGL subsystem\n" );
		}
		ri.Cvar_Set( "r_mode", "2" );
		ri.Cvar_Set( "r_fullscreen", "0" );
	}
}


/*
** GLimp_EndFrame
*/
void GLimp_EndFrame( void ) {
	//
	// swapinterval stuff
	//
	if ( r_swapInterval->modified ) {
		r_swapInterval->modified = qfalse;

		if ( !glConfig.stereoEnabled ) {    // why?
			SDL_GL_SetSwapInterval( r_swapInterval->integer );
		}
	}

	// don't flip if drawing to front buffer
	if ( Q_stricmp( r_drawBuffer->string, "GL_FRONT" ) != 0 ) {
		SDL_GL_SwapWindow( glw_state.window );
	}

}


/*
** GLimp_Init

** This is the platform specific OpenGL initialization function. It sets up
** the SDL window and the GL context, reads the config strings and
** initializes the extensions.
*/
void GLimp_Init( void ) {
	cvar_t *lastValidRenderer = ri.Cvar_Get( "r_lastValidRenderer", "(uninitialized)", CVAR_ARCHIVE );

	ri.Printf( PRINT_ALL, "Initializing OpenGL subsystem\n" );

	r_allowSoftwareGL = ri.Cvar_Get( "r_allowSoftwareGL", "0", CVAR_LATCH );
	r_maskMinidriver = ri.Cvar_Get( "r_maskMinidriver", "0", CVAR_LATCH );

	// create the window and the context
	GLW_StartOpenGL();

	// bind the OpenGL entry points through glad
	if ( !gladLoadGLLoader( (GLADloadproc)SDL_GL_GetProcAddress ) ) {
		ri.Error( ERR_FATAL, "GLimp_Init() - could not load the OpenGL entry points\n" );
	}

	// get our config strings
	Q_strncpyz( glConfig.vendor_string,(const char *)( qglGetString( GL_VENDOR )), sizeof( glConfig.vendor_string ) );
	Q_strncpyz( glConfig.renderer_string,(const char *)( qglGetString( GL_RENDERER )), sizeof( glConfig.renderer_string ) );
	Q_strncpyz( glConfig.version_string,(const char *)( qglGetString( GL_VERSION )), sizeof( glConfig.version_string ) );
	Q_strncpyz( glConfig.extensions_string,(const char *)( qglGetString( GL_EXTENSIONS )), sizeof( glConfig.extensions_string ) );

	// OpenGL 3.3 compatibility is the minimum this renderer supports
	if ( !GLAD_GL_VERSION_3_3 ) {
		ri.Error( ERR_FATAL, "OpenGL 3.3 or newer is required, this system reports %s\n", glConfig.version_string );
	}

	glConfig.hardwareType = GLHW_GENERIC;

	ri.Cvar_Set( "r_lastValidRenderer", glConfig.renderer_string );

	GLW_InitExtensions();
}


/*
** GLimp_Shutdown

** This routine does all OS specific shutdown procedures for the OpenGL
** subsystem.
*/
void GLimp_Shutdown( void ) {
	ri.Printf( PRINT_ALL, "Shutting down OpenGL subsystem\n" );

	// set current context to NULL, then delete it
	if ( glw_state.context ) {
		SDL_GL_MakeCurrent( glw_state.window, NULL );
		SDL_GL_DestroyContext( glw_state.context );
		glw_state.context = NULL;
	}

	// destroy window
	if ( glw_state.window ) {
		// remember whether the user kept the window maximized
		s_windowMaximized = ( SDL_GetWindowFlags( glw_state.window ) & SDL_WINDOW_MAXIMIZED ) != 0;
		ri.Printf( PRINT_ALL, "...destroying window\n" );
		SDL_DestroyWindow( glw_state.window );
		glw_state.window = NULL;
		ri.SetGameWindow( NULL );
		glw_state.cdsFullscreen = qfalse;
	}


	memset( &glConfig, 0, sizeof( glConfig ) );
	memset( &glState, 0, sizeof( glState ) );
}


/*
** GLimp_LogComment
*/
void GLimp_LogComment( char *comment ) {
	if ( glw_state.log_fp ) {
		fprintf( glw_state.log_fp, "%s", comment );
	}
}


/*
===========================================================

SMP acceleration

===========================================================
*/

HANDLE renderCommandsEvent;
HANDLE renderCompletedEvent;
HANDLE renderActiveEvent;

void ( *glimpRenderThread )( void );

static DWORD WINAPI GLimp_RenderThreadWrapper( LPVOID ) {
	glimpRenderThread();

	// unbind the context before we die
	SDL_GL_MakeCurrent( glw_state.window, NULL );
	return 0;
}

/*
=======================
GLimp_SpawnRenderThread
=======================
*/
HANDLE renderThreadHandle;
DWORD renderThreadId;
qboolean GLimp_SpawnRenderThread( void ( *function )( void ) ) {

	renderCommandsEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	renderCompletedEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	renderActiveEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	glimpRenderThread = function;

	renderThreadHandle = CreateThread(
		NULL, 0, GLimp_RenderThreadWrapper, NULL, 0, &renderThreadId );

	if ( !renderThreadHandle ) {
		return qfalse;
	}

	return qtrue;
}

static void    *smpData;
static int smpErrors;

void *GLimp_RendererSleep( void ) {
	void    *data;

	if ( !SDL_GL_MakeCurrent( glw_state.window, NULL ) ) {
		smpErrors++;
	}

	ResetEvent( renderActiveEvent );

	// after this, the front end can exit GLimp_FrontEndSleep
	SetEvent( renderCompletedEvent );

	WaitForSingleObject( renderCommandsEvent, INFINITE );

	if ( !SDL_GL_MakeCurrent( glw_state.window, glw_state.context ) ) {
		smpErrors++;
	}

	ResetEvent( renderCompletedEvent );
	ResetEvent( renderCommandsEvent );

	data = smpData;

	// after this, the main thread can exit GLimp_WakeRenderer
	SetEvent( renderActiveEvent );

	return data;
}


void GLimp_FrontEndSleep( void ) {
	WaitForSingleObject( renderCompletedEvent, INFINITE );

	if ( !SDL_GL_MakeCurrent( glw_state.window, glw_state.context ) ) {
		smpErrors++;
	}
}


void GLimp_WakeRenderer( void *data ) {
	smpData = data;

	if ( !SDL_GL_MakeCurrent( glw_state.window, NULL ) ) {
		smpErrors++;
	}

	// after this, the renderer can continue through GLimp_RendererSleep
	SetEvent( renderCommandsEvent );

	WaitForSingleObject( renderActiveEvent, INFINITE );
}

