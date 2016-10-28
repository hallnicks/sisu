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

#include "SDLColorSwitcher.hpp"

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
