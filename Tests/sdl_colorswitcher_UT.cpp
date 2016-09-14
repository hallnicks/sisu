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

} // namespace


TEST(sdl_UT, CreateSDLWindowWithoutExceptions)
{

	{
		class SDLTest
		{
			SDL_Window * mMainWindow;

			SDL_GLContext mMainContext;

			uint8_t mR, mG, mB, mA;

			GLfloat r, g, b, a;


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

			static void _printSDL_GL_Attributes()
			{
				int value = 0;
				SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &value);
				std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION : " << value << std::endl;

				SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &value);
				std::cout << "SDL_GL_CONTEXT_MINOR_VERSION: " << value << std::endl;
			}

			public:
				SDLTest( )
					: mMainWindow( NULL )
					, mMainContext( )
					, mR( 255 )
					, mG( 0 )
					, mB( 0 )
					, mA( 255 )
					, r( 0.5 )
					, g( 0.25 )
					, b( 00.75 )
					, a( 0.5 )
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

					//_printSDL_GL_Attributes();

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

					glClearColor( 1.0, 0.0, 0.0, 1.0 );

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
						
						switch( val ) 
						{
							case 0: { r += 0.01 * reverseR ? -1 : 1; if( r >= 1.0 || r <= 0.0) { reverseR = !reverseR; } } break; 
							case 1: { g += 0.01 * reverseG ? -1 : 1; if( g >= 1.0 || g <= 0.0) { reverseG = !reverseG; } } break; 
							case 2: { b += 0.01 * reverseB ? -1 : 1; if( b >= 1.0 || b <= 0.0) { reverseB = !reverseB; } } break; 
							case 3: { a += 0.01 * reverseA ? -1 : 1; if( a >= 1.0 || a <= 0.0) { reverseA = !reverseA; } } break; 
							default: break; 
						}
						
						glClear( GL_COLOR_BUFFER_BIT );

						SDL_GL_SwapWindow( mMainWindow );
					}
				}


		};

		SDLTest test;

		test.initialize( 1 );

		test.run();
	}
}
