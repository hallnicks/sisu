#include "test.hpp" 

// C style IO is necessary to talk to libpng. This could be seen as a wrapper between stl-style c++ and libpng
#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <fstream>
#include <math.h>
#include "memblock.hpp"
#include "ttyc.hpp"

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
			#if 0
			xImage.perPixel( [&]( PNGPixel xPixel )
					{
						xPixel.data[0] = rand( ) % 255;
			 			xPixel.data[1] = rand( ) % 255;
						xPixel.data[2] = rand( ) % 255;
						xPixel.data[3] = 255;
					});
			#endif
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
