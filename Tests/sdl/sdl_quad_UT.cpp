#ifndef OPENGLES
#include "test.hpp"
#include "SDLQuadShader.hpp"

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
	SDLQuadShader test;

	test.initialize( { 3, 1, 1, false, false } );

	test.run( );
}
#endif
