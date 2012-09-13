#include "test.hpp"
#include "ttycolor.hpp"
#include "scanline.hpp"

#include <ctime>
#include <clocale>

using sisu::TTYC;
using sisu::TTYCTransform;

namespace
{
        class Scanline_UT : public context
        {
		protected:
			char * mLocale;

                public:
                        Scanline_UT( ) 	: context( ), mLocale( 0 ) { ; }

                        ~Scanline_UT( ) { }

                        void Up( )
			{
				srand( time( NULL ) );

				mLocale = std::setlocale( LC_ALL, NULL );

			        std::setlocale( LC_ALL, "en_US.UTF-8" );
			}

                        void Down( )
			{
			        std::setlocale( LC_ALL, mLocale );
			}
        };
}

TEST(Scanline_UT, TestScanLine)
{
	sisu::eTTYColor c = sisu::eTTYCNone;
	sisu::eTTYModifier m = sisu::eModNone;

	TTYC expectedTTYC[] = { TTYCTransform::color2ttyc( 'X', ++c, ++c, ++m )
				, TTYCTransform::color2ttyc( 'Y', ++c, ++c, ++m )
				, TTYCTransform::color2ttyc( 'Z', ++c, ++c, ++m )
				, TTYCTransform::color2ttyc( 'W', ++c, ++c, ++m ) };

	TTYC setTTYC[] = { TTYCTransform::color2ttyc( 'x', ++c, ++c, ++m )
			, TTYCTransform::color2ttyc( 'y', ++c, ++c, ++m )
			, TTYCTransform::color2ttyc( 'z', ++c, ++c, ++m )
			, TTYCTransform::color2ttyc( 'w', ++c, ++c, ++m ) };

	uint8_t const bound = sizeof(expectedTTYC) / sizeof(TTYC);

	sisu::Scanline sc( expectedTTYC, bound );

	for ( uint8_t i = 0; i < bound; ++i )
	{
		MUSTEQ( ( * sc [ i ] ), expectedTTYC [ i ] )
	}

	for ( uint8_t i = 0; i < bound; ++i )
	{
		( * sc [ i ] ) = setTTYC [ i ];
	}

	for ( uint8_t i = 0; i < bound; ++i )
	{
		MUSTEQ( ( * sc [ i ] ), setTTYC [ i ] )
	}
}
