#ifdef SDL_COLOR_SWITCHER_UT
#include "test.hpp"
#include "SDLTest.hpp"

#include <string>
#include <iostream>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>

#ifdef OPENGLES
#include <GLES2/gl2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

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

				if ( ! ( flags & SDL_WINDOW_MAXIMIZED ) )
				{
				        SDL_MaximizeWindow( mMainWindow );
				        SDL_RaiseWindow( mMainWindow );
				        SDL_SetWindowGrab( mMainWindow, SDL_TRUE );
				}

// TODO: Bug? Flag is never set on linux.
#ifdef WIN32
			        if ( ! ( flags & SDL_WINDOW_INPUT_FOCUS) )
			        {
			                std::cout << "Failed to get window focus." << std::endl;
			                exit( -1 );
			        }
#endif

				glClearColor( _randomGLfloat( )
					    , _randomGLfloat( )
					    , _randomGLfloat( )
					    , _randomGLfloat( ) );

				glClear( GL_COLOR_BUFFER_BIT );

				SDL_GL_SwapWindow( mMainWindow );
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

#ifndef OPENGLES
	test.initialize( { 3, 1, 1, false, false } );
#else
	test.initialize( { 3, 0, 0, false, false } );
#endif

	test.run();
}
#endif // SDL_COLOR_SWITCHER_UT
