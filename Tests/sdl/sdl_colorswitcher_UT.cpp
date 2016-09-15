#if 0
#include "test.hpp"
#include <string>
#include <iostream>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.H>

namespace
{

class sdl_UT : public context
{
	public:
		sdl_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};



class SDLTest
{
	SDL_Window * mMainWindow;
	SDL_GLContext mMainContext;
	GLfloat r, g, b, a;
	
	inline static GLfloat randomGLfloat( )
	{
 		return static_cast <GLfloat> (rand()) / static_cast <GLfloat> (RAND_MAX);
	}

	void _setOpenGLAttributes()
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
	
	static void _checkSDLError()
	{
		std::string const error = SDL_GetError();

		if ( error != "" )
		{
			std::cout << "SDL Error : " << error << std::endl;
			SDL_ClearError();
		}
	}

		public:
			SDLTest( )
				: mMainWindow( NULL )
				, mMainContext( )
				, r( randomGLfloat( ) )
				, g( randomGLfloat( ) )
				, b( randomGLfloat( ) )
				, a( randomGLfloat( ) )

			{

			}

			~SDLTest( )
			{
				SDL_GL_DeleteContext( mMainContext );
				SDL_DestroyWindow( mMainWindow );
				SDL_Quit();
			}

			void initialize( int const xSwapInterval )
			{
				int result1;
				
				if ( ( result1 = SDL_Init( SDL_INIT_VIDEO ) ) < 0) 
				{
					std::cout << "SDL init failed." << result1 << std::endl;
					exit(-1);
				}

				mMainWindow = SDL_CreateWindow( "SDL2 OpenGL initialization test"
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

				glClearColor( randomGLfloat( )
						, randomGLfloat( )
						, randomGLfloat( )
						, randomGLfloat( ) );

				glClear( GL_COLOR_BUFFER_BIT );

				SDL_GL_SetSwapInterval( xSwapInterval );

				SDL_GL_SwapWindow( mMainWindow );
			}

			void run()
			{

				for ( int ii = 0; ii < 250; ++ii )
				{
					SDL_Event event;

					uint32_t const flags = SDL_GetWindowFlags( mMainWindow );

					if ( ! ( flags & SDL_WINDOW_BORDERLESS ) )
					{
						std::cout << "Window wasn't borderless!" << std::endl;
						exit(-1);
					}

					if ( ! ( flags & SDL_WINDOW_MAXIMIZED ) )
					{
					        SDL_MaximizeWindow( mMainWindow );
					        SDL_RaiseWindow( mMainWindow );
					        SDL_SetWindowGrab( mMainWindow, SDL_TRUE );
					}

				        if ( ! ( flags & SDL_WINDOW_INPUT_FOCUS) )
				        {
				                std::cout << "Failed to get window focus." << std::endl;
				                exit( -1 );
				        }
								
					static bool reverseR, reverseG, reverseB, reverseA;

					glClearColor( r, g, b, a );

					int val = rand() % 4; 
				
					#if 0
					switch( val ) 
					{
						case 0: { r += 0.01 * reverseR ? -1 : 1; if( r >= 1.0 || r <= 0.0) { reverseR = !reverseR; } } break; 
						case 1: { g += 0.01 * reverseG ? -1 : 1; if( g >= 1.0 || g <= 0.0) { reverseG = !reverseG; } } break; 
						case 2: { b += 0.01 * reverseB ? -1 : 1; if( b >= 1.0 || b <= 0.0) { reverseB = !reverseB; } } break; 
						case 3: { a += 0.01 * reverseA ? -1 : 1; if( a >= 1.0 || a <= 0.0) { reverseA = !reverseA; } } break; 
						default: break; 
					}
					#endif

					r = randomGLfloat( );
					g = randomGLfloat( );
					b = randomGLfloat( );
					a = randomGLfloat( );

						
					glClear( GL_COLOR_BUFFER_BIT );

					SDL_GL_SwapWindow( mMainWindow );
				}
			}
		}; // class

} // namespace


TEST(sdl_UT, CreateSDLWindowWithoutExceptions)
{

	SDLTest test;

	test.initialize( 1 );

	test.run();
}
#endif
