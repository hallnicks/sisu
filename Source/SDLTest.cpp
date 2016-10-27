#include "SDLTest.hpp"

#include <iostream>
#include <string>

#ifdef LINUX
#include <X11/Xlib.h>
#endif

#include "AndroidLogWrapper.hpp"

namespace sisu {

void _checkForGLError( const char * xAddendum )
{
	GLenum const error = glGetError( );
	if ( error != GL_NO_ERROR )
	{
#ifdef ANDROID
		__android_log_print( ANDROID_LOG_VERBOSE
				   , "sisu"
				   , "OpenGL Error# %d: %s\n"
				   , (int32_t)error
				   , xAddendum == NULL ? "" : xAddendum );
#else
		std::cerr << "OpenGL: Error# "
			  << error
			  << " "
			  << (const char*)(xAddendum == NULL ? "" : xAddendum)
			  << std::endl;
#endif
		exit( -1 );
	}
}

void SDLTestWindow::_setOpenGLAttributes( OpenGLAttributes const & xAttributes )
{
#ifdef OPENGLES
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES   );
#else
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
#endif

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, xAttributes.mMajorGLVersion );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, xAttributes.mMinorGLVersion );

#ifndef OPENGLES
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

	if ( xAttributes.mDoubleBufferingEnabled )
	{
		SISULOG( "Enabling double buffering." );
	        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	}

	if ( xAttributes.mMultisampleBufferingEnabled )
	{
		SISULOG( "Enabling multisample buffering." );
	        SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
	}

	// TODO: add additional attributes as necessary.. eventually make configurable, etc
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
#endif

}

void SDLTestWindow::_hide( )
{
	SDL_SetWindowSize( mMainWindow, 1, 1 );
	SDL_MinimizeWindow( mMainWindow );
}

void SDLTestWindow::_stealContext( )
{
	if ( mMainWindow == NULL )
	{
		SISULOG( "Cannot steal context because main window was null." );
		exit( -1 );
	}

	uint32_t const flags = SDL_GetWindowFlags( mMainWindow );

        if ( ! ( flags & SDL_WINDOW_BORDERLESS ) )
        {
		SISULOG( "Window wasn't borderless!" );
                exit(-1);
        }

        if ( ! ( flags & SDL_WINDOW_MAXIMIZED ) )
        {
      	       SDL_MaximizeWindow( mMainWindow );
               SDL_RaiseWindow( mMainWindow );
               SDL_SetWindowGrab( mMainWindow, SDL_TRUE );

#ifdef WIN32 // Never set on linux (!) bug?
	       if ( ! ( flags & SDL_WINDOW_INPUT_FOCUS) )
	       {
			_checkSDLError( "SDL_MaximizeWindow/RaiseWindow/SetWindowGrab" );
	              	exit( -1 );
	       }
#endif
        }
}

void SDLTestWindow::_checkSDLError( const char * xAddendum )
{
	std::string const error = SDL_GetError();

	if ( error != "" )
        {
#ifdef ANDROID
		__android_log_print( ANDROID_LOG_VERBOSE
				   , "sisu"
				   , "%s -- SDL error: %s\n"
				   , xAddendum == NULL ? "Unknown:" : xAddendum
				   , SDL_GetError( ) );

#else
        	std::cout << "SDL Error : " << error << std::endl;
#endif
                SDL_ClearError();
	}
}

SDLTestWindow::SDLTestWindow( )
	: mQuit( )
	, mMainWindow( NULL )
        , mMainContext( )
{
	;
}

SDLTestWindow::~SDLTestWindow( )
{
	SDL_GL_DeleteContext( mMainContext );

	mMainContext = NULL;

        SDL_DestroyWindow( mMainWindow );

	mMainWindow = NULL;

        SDL_Quit();
}

void SDLTestWindow::initialize( OpenGLAttributes const & xAttributes )
{
	uint32_t w = 1, h = 1;

        if ( SDL_Init( SDL_INIT_VIDEO ) < 0)
        {
		_checkSDLError( "SDL_Init" );
                exit(-1);
	}

        _setOpenGLAttributes( xAttributes );

#ifdef LINUX
	Display * d = XOpenDisplay( NULL );
	Screen *  s = DefaultScreenOfDisplay( d );

	w = s->width;
	h = s->height;

	XCloseDisplay( d );
#endif
        mMainWindow = SDL_CreateWindow( "SDL2 OpenGL"
                                       , SDL_WINDOWPOS_CENTERED
                                       , SDL_WINDOWPOS_CENTERED
                                       , w
                                       , h
                                       , SDL_WINDOW_OPENGL      | SDL_WINDOW_BORDERLESS    |
                                         SDL_WINDOW_SHOWN       | SDL_WINDOW_INPUT_GRABBED |
                                         SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS );
	if( mMainWindow == NULL )
        {
                _checkSDLError( "SDL_CreateWindow" );
                exit( -1 );
	}

	SDL_VERSION(&mWindowInfo.version);

	if ( !SDL_GetWindowWMInfo( mMainWindow, &mWindowInfo ) )
	{
		_checkSDLError( "SDL_GetWindowWMInfo" );
		exit( -1 );
	}

	_stealContext( );

        SDL_GetWindowSize( mMainWindow, &mW, &mH );

        mMainContext = SDL_GL_CreateContext( mMainWindow );

	if ( mMainContext == NULL )
	{
		_checkSDLError( "SDL_GL_CreateContext" );
		exit( -1 );
	}

	if ( SDL_GL_MakeCurrent( mMainWindow, mMainContext ) != 0 )
	{
		_checkSDLError( "SDL_GL_MakeCurrent" );
		exit( -1 );
	}

#ifndef OPENGLES
	// Initialize GLEW with experimental extensions enabled
	glewExperimental = true;

	GLenum const err = glewInit( );

	if ( err != GLEW_OK )
	{
		_checkSDLError( "glewInit" );
		exit( -1 );
	}
#endif
        if ( SDL_GL_SetSwapInterval( xAttributes.mSwapInterval ) < 0 )
	{
		_checkSDLError( "SDL_GL_SetSwapInterval" );
		exit( -1 );
	}
}

} // namespace SISU
