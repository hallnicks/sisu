#include "SDLTest.hpp"

#include <iostream>
#include <string>

#ifdef LINUX
#include <X11/Xlib.h>
#endif

namespace sisu {

void _checkForGLError( const char * xAddendum )
{
	GLenum const error = glGetError( );
	if ( error != GL_NO_ERROR )
	{
		std::cerr << "OpenGL: Error# "
			  << error
			  << " "
			  << (const char*)(xAddendum == NULL ? "" : xAddendum)
			  << std::endl;
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
		std::cout << "Enabling double buffering." << std::endl;
	        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	}

	if ( xAttributes.mMultisampleBufferingEnabled )
	{
		std::cout << "Enabling multisample buffering." << std::endl;
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
		std::cerr << "Cannot steal context because main window was null." << std::endl;
		exit( -1 );
	}

	uint32_t const flags = SDL_GetWindowFlags( mMainWindow );

        if ( ! ( flags & SDL_WINDOW_BORDERLESS ) )
        {
		std::cerr << "Window wasn't borderless!" << std::endl;
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
	              std::cerr << "Failed to get window focus: " << SDL_GetError( ) << std::endl;
	              exit( -1 );
	       }
#endif
        }
}

void SDLTestWindow::_checkSDLError( )
{
	std::string const error = SDL_GetError();

	if ( error != "" )
        {
        	std::cout << "SDL Error : " << error << std::endl;
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
        if ( SDL_Init( SDL_INIT_VIDEO ) < 0)
        {
		std::cout << "SDL init failed: " << SDL_GetError() << std::endl;
                exit(-1);
	}

        _setOpenGLAttributes( xAttributes );

	uint32_t w = 1, h = 1;

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
        	std::cout << "SDL_CreateWindow() failed." << std::endl;
                _checkSDLError( );
                exit( -1 );
	}

	_stealContext( );

        SDL_GetWindowSize( mMainWindow, &mW, &mH );

        mMainContext = SDL_GL_CreateContext( mMainWindow );

	if ( mMainContext == NULL )
	{
		std::cerr << "Failed to create GL context from window: " << SDL_GetError( ) << std::endl;
		exit( -1 );
	}

	if ( SDL_GL_MakeCurrent( mMainWindow, mMainContext ) != 0 )
	{
		std::cerr << "Failed to make GL context current: " << SDL_GetError( ) << std::endl;
		exit( -1 );
	}

#ifndef OPENGLES
	// Initialize GLEW with experimental extensions enabled
	glewExperimental = true;

	GLenum const err = glewInit( );

	if ( err != GLEW_OK )
	{
		std::cerr << "glewInit( ) failed: " << glewGetErrorString( err ) << std::endl;
		exit( -1 );
	}
#endif
        if ( SDL_GL_SetSwapInterval( xAttributes.mSwapInterval ) < 0 )
	{
		std::cerr << "Setup vsync failed: " << SDL_GetError( ) << std::endl;
		exit( -1 );
	}
}

} // namespace SISU
