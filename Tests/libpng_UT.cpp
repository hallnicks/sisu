#if 0
#include "test.hpp" 

// C style IO is necessary to talk to libpng. This could be seen as a wrapper between stl-style c++ and libpng

#include "memblock.hpp"
#include "ttyc.hpp"
#include "ioassist.hpp"

#include "PNGImage.hpp"

using namespace sisu;

namespace
{
	class libpng_UT : public context
	{
		public:
			libpng_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};
};

// Ensures writes are stateless ( at least for one iteration ) ..
TEST(libpng_UT, writePNGFile)
{
	PNGImage test1("resources/spring_breakers_png_by_flawlessduck-d5xdmmp.png");
	std::ofstream ofs1( "render01.png", std::ios::binary);

	ofs1 << test1;
	ofs1.close( );

	std::ofstream ofs2( "render02.png", std::ios::binary);

	ofs2 << test1;
	ofs2.close( );

	MUSTEQ(filesAreEqual( "render01.png", "render02.png" ), true);
}

TEST(libpng_UT, readPNGfile)
{
	PNGImage test1("resources/spring_breakers_png_by_flawlessduck-d5xdmmp.png");

	test1( [&]( PNGImage & xImage )
			{
				std::ofstream ofs("libpng_UT_output01.png", std::ios::binary );
				ofs << xImage; 
				ofs.close( ); 
			}
	    )( [&]( PNGImage & xImage ) 
		{
			xImage.perPixel( [&]( PNGPixel xPixel )
					{
						xPixel.data[0] = (xPixel.data[0] + (rand( ) % 255 ))/ 2;
						xPixel.data[1] = (xPixel.data[1] + (rand( ) % 255 ))/ 2;
						xPixel.data[2] = (xPixel.data[2] + (rand( ) % 255 ))/ 2;
						xPixel.data[3] = 255;
					});
		}
	    ) ( [&]( PNGImage & xImage )
			{
				std::ofstream ofs("libpng_UT_output02.png", std::ios::binary );
				ofs << xImage;
				ofs.close( );
			});

	test1.perPixel( [&] ( PNGPixel xPixel )
	{

		xPixel.data[0] = 0;
		xPixel.data[1] = 0;
		xPixel.data[2] = 0;
		xPixel.data[3] = 255;

	} );

	std::ofstream ofs2( "final_render.png", std::ios::binary);

	ofs2 << test1;
	ofs2.close( );
}

TEST(libpng_UT, writeBlankPng)
{
	const char * outputPath01 = "blank.png";
	const char * outputPath02 = "filled.png";
	const char * outputPath03 = "usexy.png";

	PNGImage image( { 1920, 1080 } );

	std::ofstream ofs1( outputPath01, std::ios::binary);

	ofs1 << image;
	ofs1.close( );

	(image)( [&]( PNGImage & xImage )
	{
		xImage.perPixel( [ & ] ( PNGPixel xPixel )
				{
					xPixel.data[0] = rand( ) % 255;
					xPixel.data[1] = rand( ) % 255;
					xPixel.data[2] = rand( ) % 255;
					xPixel.data[3] = 255;
				});
	});


	std::ofstream ofs2( outputPath02, std::ios::binary);

	ofs2 << image;
	ofs2.close( );

	(image)( [&]( PNGImage & xImage )
	{
		xImage.perPixel( [ & ] ( PNGPixel xPixel )
				{
					double const xRatio = (double)xPixel.x / (double)image.getWidth( );
					double const yRatio = (double)xPixel.y / (double)image.getHeight( );

					xPixel.data[ 0 ] =
					xPixel.data[ 1 ] =
					xPixel.data[ 2 ] = 0;

					uint8_t const index = rand( ) % 3;

					uint8_t const result = xRatio * 255 + yRatio * 255;

					xPixel.data[ index ] = result;

				});
	});

	std::ofstream ofs3( outputPath03, std::ios::binary);

	ofs3 << image;
	ofs3.close( );
}

TEST(libpng_UT, writePNGByIndices)
{
	const char * outputPath01 = "green.png";

	PNGImage image( { 1920, 1080 } );

	for ( int32_t y = 0; y < image.getHeight( ); ++y )
	{
		for (int32_t x = 0; x < image.getWidth( ); ++x )
		{
			uint8_t* data = (uint8_t*)image[y][x];

			data[ 0 ] = (rand( ) % 255 )/ 2;
			data[ 1 ] = 255;
			data[ 2 ] = (rand( ) % 255 )/ 2;
			data[ 3 ] = 255;
		}
	}

	std::ofstream ofs1( outputPath01, std::ios::binary);

	ofs1 << image;
	ofs1.close( );
}
#endif
