#include "SDLTest.hpp"

#include <iostream>
#include <string>

namespace sisu {

void _printGLProgramLog( GLuint const xProgram )
{
	if ( !glIsProgram( xProgram ) )
	{
		std::cerr << "Parameter was not a valid GL program." << std::endl;
		exit( -1 );
	}

	int32_t logLength = 0, maxLength = 0;

	glGetProgramiv( xProgram, GL_INFO_LOG_LENGTH, &maxLength );

        char * infoLog = new char[ maxLength ];

        glGetProgramInfoLog( xProgram, maxLength, &logLength, infoLog );

        if( logLength > 0 )
        {
		std::cout << infoLog << std::endl;
        }

        delete[] infoLog;
}

void _printGLShaderLog( GLuint const xShader )
{
	if ( !glIsShader( xShader ) )
	{
		std::cerr << "Parameter was not a valid GL shader." << std::endl;
		exit( -1 );
	}

	int32_t logLength = 0, maxLength = 0;

	glGetShaderiv( xShader, GL_INFO_LOG_LENGTH, &maxLength );

        char * infoLog = new char[ maxLength ];

        glGetShaderInfoLog( xShader, maxLength, &logLength, infoLog );

        if( logLength > 0 )
        {
		std::cout << infoLog << std::endl;
        }

        delete[] infoLog;
}

void _checkForGLError( )
{
	GLenum const error = glGetError( );
	if ( error != GL_NO_ERROR )
	{
		std::cerr << "Error in OpenGL: " << error << std::endl;
		exit( -1 );
	}
}

void SDLTestWindow::_setOpenGLAttributes( OpenGLAttributes const & xAttributes )
{
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, xAttributes.mMajorGLVersion );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, xAttributes.mMinorGLVersion );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK , SDL_GL_CONTEXT_PROFILE_CORE );

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

	       if ( ! ( flags & SDL_WINDOW_INPUT_FOCUS) )
	       {
	              std::cerr << "Failed to get window focus." << std::endl;
	              exit( -1 );
	       }
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

void SDLTestWindow::initialize( OpenGLAttributes const & xAttributes )
{
	int result1;

        if ( ( result1 = SDL_Init( SDL_INIT_VIDEO ) ) < 0)
        {
		std::cout << "SDL init failed." << result1 << std::endl;
                exit(-1);
	}

        _setOpenGLAttributes( xAttributes );

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

	_stealContext( ); 

        SDL_GetWindowSize( mMainWindow, &mW, &mH );

        mMainContext = SDL_GL_CreateContext( mMainWindow );

	if ( mMainContext == NULL )
	{
		std::cerr << "Failed to create GL context from window." << std::endl;
		exit( -1 ); 
	}

	// Initialize GLEW

	glewExperimental = true;

	GLenum err = glewInit( );

	if ( err != GLEW_OK )
	{
		std::cerr << "glewInit( ) failed." << std::endl;
		exit( -1 );
	}

        if ( SDL_GL_SetSwapInterval( xAttributes.mSwapInterval ) < 0 )
	{
		std::cerr << "Setup vsync failed." << std::endl;
		exit( -1 );
	}
}

} // namespace SISU