#if 0
#include "test.hpp"
#include "SDLTest.hpp"
#include "SDLShaderTest.hpp"
#include "ioassist.hpp"

#include <string>
#include <iostream>
#include <functional>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.H>
#include <GL/glu.h>
#include <stdio.h>
#include <string>

using namespace sisu;

namespace {

class sdl_RedBookCh03_UT : public context
{
	public:
		sdl_RedBookCh03_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(sdl_RedBookCh03_UT, CreateSDLWindowWithoutExceptions)
{

	RedbookCh03Shader window;

	window.initialize( { 4, 4, 1, true, true } );
	window.run( );
}
#endif
