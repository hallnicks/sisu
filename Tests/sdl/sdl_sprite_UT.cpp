#include "test.hpp"
#include "SDLTest.hpp"
#include "SDLShaderTest.hpp"
#include "ioassist.hpp"
#include "VBM.hpp"

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

class sdl_sprite_UT : public context
{
	public:
		sdl_sprite_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

#if 0
TEST(sdl_sprite_UT, CreateSpriteShaderWindowWithoutExceptions)
{
	{
		SpriteShader window;

		window.initialize( { 4, 4, 0, false, false } );

		window.run( );
	}
}
#endif

TEST(sdl_sprite_UT, CreatePBOSpriteShaderWithoutExceptions)
{
	{
		SpriteShader window(true);

		window.initialize( { 4, 4, 0, true, true } );

		window.run( );
	}

}
