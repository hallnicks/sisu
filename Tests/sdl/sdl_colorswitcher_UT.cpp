#include "test.hpp"
#include "SDLTest.hpp"

#include <string>
#include <iostream>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

using namespace sisu;

namespace
{

class sdl_colorswitcher_UT : public context
{
	public:
		sdl_colorswitcher_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

class SDLColorSwitcher : public SDLTestWindow
{
		protected:

			virtual void render( ) 
			{
				SDL_Event event;

				uint32_t const flags = SDL_GetWindowFlags( mMainWindow );

				if ( ! ( flags & SDL_WINDOW_BORDERLESS ) )
				{
					std::cout << "Window wasn't borderless!" << std::endl;
					exit(-1);
				}

				TRACE;
				if ( ! ( flags & SDL_WINDOW_MAXIMIZED ) )
				{
				        SDL_MaximizeWindow( mMainWindow );
				        SDL_RaiseWindow( mMainWindow );
				        SDL_SetWindowGrab( mMainWindow, SDL_TRUE );
				}

				#if 0
			        if ( ! ( flags & SDL_WINDOW_INPUT_FOCUS) )
			        {
			                std::cout << "Failed to get window focus." << std::endl;
			                exit( -1 );
			        }
				#endif

				TRACE;
				glClearColor( _randomGLfloat( )
					    , _randomGLfloat( )
					    , _randomGLfloat( )
					    , _randomGLfloat( ) );

				glClear( GL_COLOR_BUFFER_BIT );

				SDL_GL_SwapWindow( mMainWindow );

				TRACE;
			}
		public:
			SDLColorSwitcher( )
				: SDLTestWindow( )
			{
				;
			}

			virtual void run( )
			{
				for ( int ii = 0; ii < 250; ++ii )
				{
					render( );
				}
			}
		}; // class

} // namespace


TEST(sdl_colorswitcher_UT, CreateSDLWindowWithoutExceptions)
{
	SDLColorSwitcher test;

	test.initialize( { 3, 1, 1, false, false } );

	test.run();
}
