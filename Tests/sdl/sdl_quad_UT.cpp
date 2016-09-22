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

class sdl_quad_UT : public context
{
	public:
		sdl_quad_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(sdl_quad_UT, CreateSDLWindowWithoutExceptions)
{

	{
		SDLQuadShader test;

		test.initialize( { 3, 1, 1, false, false } );

		test.run( );
	}
}
#endif
