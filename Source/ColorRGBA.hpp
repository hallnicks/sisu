#ifndef COLOR_GBA_7F53F9E58B1A4806A13076928FE2B49E_HPP_
#define COLOR_GBA_7F53F9E58B1A4806A13076928FE2B49E_HPP_

#include <SDL2/SDL_opengl.h>
namespace sisu 
{
	// TODO: Fix to use union all the way about, consider endianness.
	struct ColorRGBA { GLfloat r, g, b, a; };
} // namespace sisu

#endif// COLOR_GBA_7F53F9E58B1A4806A13076928FE2B49E_HPP_
