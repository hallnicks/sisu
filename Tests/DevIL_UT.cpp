#ifdef DEVIL_UT
#include "test.hpp"

#include "DevILImage.hpp"

#include <IL/il.h>

using namespace sisu;

namespace {

class DevIL_UT : public context
{

	public:
		DevIL_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace



TEST(DevIL_UT, LoadImageRGBDataUsingClassWithoutExceptions)
{
	DevILImage image( "resources/testinput/testinput01.png" );

	image.toGLTextureBuffer( );
}
TEST(DevIL_UT, LoadImageRGBDataWithoutExceptions)
{
	ILuint imgID = 0;

	ilInit( );

	ilGenImages( 1, &imgID );
	ilBindImage( imgID );

	ilLoadImage( "resources/testinput/testinput01.png" );

	uint32_t const width  = ilGetInteger( IL_IMAGE_WIDTH  )
		     , height = ilGetInteger( IL_IMAGE_HEIGHT );


	uint8_t * data = ilGetData();

	ilBindImage (0 );
	ilDeleteImage( imgID );
}
#endif
