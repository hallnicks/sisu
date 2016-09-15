#include "SDLTest.hpp"

#include <iostream>
#include <string>

namespace sisu {
void SDLTestWindow::_setOpenGLAttributes()             
{
	// Set our OpenGL version.
        // SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

        // 3.2 is part of the modern versions of OpenGL, but most video cards whould be able to run it
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );

        // Turn on double buffering with a 24bit Z buffer.
        // You may need to change this to 16 or 32 for your system
        SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
}

                
void SDLTestWindow::_checkSDLError()
{
	std::string const error = SDL_GetError();
	
	if ( error != "" )
        {
        	std::cout << "SDL Error : " << error << std::endl;
                SDL_ClearError();
                        
	}
                
}

SDLTestWindow::SDLTestWindow( )
	: mMainWindow( NULL )
        , mMainContext( )
{
	;
}

                        
SDLTestWindow::~SDLTestWindow( )
{
	SDL_GL_DeleteContext( mMainContext );
        SDL_DestroyWindow( mMainWindow );
        SDL_Quit();                        
}

                        
void SDLTestWindow::initialize( int const xSwapInterval )                     
{
	int result1;

        if ( ( result1 = SDL_Init( SDL_INIT_VIDEO ) ) < 0)
        {
		std::cout << "SDL init failed." << result1 << std::endl;
                exit(-1);
	}

        mMainWindow = SDL_CreateWindow( "SDL2 OpenGL"
                                        , SDL_WINDOWPOS_CENTERED
                                        , SDL_WINDOWPOS_CENTERED
                                        , 1
                                        , 1
                                        , SDL_WINDOW_OPENGL      | SDL_WINDOW_BORDERLESS    |
                                          SDL_WINDOW_SHOWN       | SDL_WINDOW_INPUT_GRABBED |
                                          SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS );
                                
	if( mMainWindow == NULL )
        {
        	std::cout << "SDL_CreateWindow() failed." << std::endl;
                _checkSDLError( );
                exit( -1 );
	}

        mMainContext = SDL_GL_CreateContext( mMainWindow );

        _setOpenGLAttributes( );

        glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );

        glClear( GL_COLOR_BUFFER_BIT );

        SDL_GL_SetSwapInterval( xSwapInterval );

        SDL_GL_SwapWindow( mMainWindow );
}

} // namespace SISU
