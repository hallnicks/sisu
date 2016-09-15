#if 0
#include "test.hpp"
#include "SDLTest.hpp"

#include <string>
#include <iostream>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.H>

using namespace sisu;

namespace
{

class sdl_UT : public context
{
	public:
		sdl_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

class SDLColorSwitcher : public SDLTestWindow
{

	        inline static GLfloat _randomGLfloat( )
	        {
	                return static_cast <GLfloat> (rand()) / static_cast <GLfloat> (RAND_MAX);
	        }

                GLfloat mR, mG, mB, mA;

		public:
			SDLColorSwitcher( )
				: SDLTestWindow( )
			        , mR( _randomGLfloat( ) )
			        , mG( _randomGLfloat( ) )
			        , mB( _randomGLfloat( ) )
			        , mA( _randomGLfloat( ) )
			{
				;
			}

			virtual void run( )
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
								

					mR = _randomGLfloat( );
				        mG = _randomGLfloat( );
					mB = _randomGLfloat( );
					mA = _randomGLfloat( );

					glClearColor( mR, mG, mB, mA );

					glClear( GL_COLOR_BUFFER_BIT );

					SDL_GL_SwapWindow( mMainWindow );
				}
			}
		}; // class

} // namespace


TEST(sdl_UT, CreateSDLWindowWithoutExceptions)
{

	SDLColorSwitcher test;

	test.initialize( 1 );

	test.run();
}
#endif
