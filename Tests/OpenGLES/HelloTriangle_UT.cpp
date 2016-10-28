#ifdef OPENGLES_HELLOTRIANGLE_UT
#ifdef OPENGLES
#include "test.hpp"

#include "SDLTestShaderWindow.hpp"
#include "Stopwatch.hpp"
#include "HelloTriangle.hpp"

#include <iostream>

using namespace sisu;

namespace
{

class HelloTriangle_UT : public context
{
	public:
		HelloTriangle_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(HelloTriangle_UT, HelloTriangle)
{
	HelloTriangle * window = new HelloTriangle;
	window->initialize( { 3, 0, 1, false, false } );
	window->run( );
	delete window;
}
#endif // OPENGLES
#endif // OPENGLES_HELLOTRIANGLE_UT
