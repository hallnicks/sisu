#include "test.hpp" 
#include "SDLTest.hpp"
#include "SDLShaderTest.hpp" 
#include "ioassist.hpp"
#include "Font.hpp"

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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace sisu;

namespace {

class sdl_font_UT : public context
{
	public:
		sdl_font_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(sdl_font_UT, CreateFontObjectWithoutExceptions)
{	
	Font font;
}
