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

TEST(sdl_sprite_UT, CreateShaderWithoutExceptions)
{
	ShaderPathPair shaderPathPair( "resources/sprite.vs.txt", "resources/sprite.fs.txt" );

 	SDLShader spriteShader( shaderPathPair );
	//spriteShader.initialize( );
}

TEST(sdl_sprite_UT, CreateSDLWindowWithoutExceptions)
{
	{
		SpriteShader window;

		window.initialize( { 4, 4, 1, false, false } );

		window.run( );
	}

	BLOCK_EXECUTION;
}
