#ifdef DEVIL_UT
#include "test.hpp"

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

// From SDK
TEST(DevIL_UT, LoadImageRGBDataWithoutExceptions)
{
	ILuint imgID = 0;
	TRACE;
	ilInit( );

	ilGenImages( 1, &imgID );
	ilBindImage( imgID );

	TRACE;
	ilLoadImage( "resources/testinput/testinput01.png" );

	uint32_t const width  = ilGetInteger( IL_IMAGE_WIDTH  )
		     , height = ilGetInteger( IL_IMAGE_HEIGHT );


	std::cout << "width  = " << width << std::endl;
	std::cout << "height = " << height << std::endl;

	TRACE;
	BYTE * data = ilGetData();

	ilBindImage (0 );
	ilDeleteImage( imgID );
}
#endif
