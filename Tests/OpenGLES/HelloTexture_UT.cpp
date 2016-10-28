#ifdef OPENGLES_HELLOTEXTURE_UT
#ifdef OPENGLES
#include "test.hpp"

#include "HelloTexture.hpp"

using namespace sisu;

namespace
{

class HelloTexture_UT : public context
{
	public:
		HelloTexture_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(HelloTexture_UT, HelloTexture)
{
	HelloTexture * window = new HelloTexture;
	window->initialize( { 3, 0, 1, false, false } );
	window->run( );
	delete window;
}
#endif // OPENGLES
#endif // OPENGLES_HELLOTRIANGLE_UT
