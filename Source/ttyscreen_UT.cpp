#include "test.hpp"
#include "ttyc.hpp"
#include "ttycolor.hpp"
#include "ttyscreen.hpp"

#include <ctime>
#include <clocale>
#include <stdint.h>

using sisu::TTYC;
using sisu::TTYCTransform;

namespace
{
        class TTYScreen_UT : public context
        {
		protected:
		        sisu::TerminalScreen mScreen;

			char * mLocale;

                public:
                        TTYScreen_UT( )
				: context( )
				, mScreen( )
				, mLocale( 0 )
			{
				;
			}

                        ~TTYScreen_UT( ) { }

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

TEST(TTYScreen_UT, TestScanLine)
{
	sisu::TerminalScreen termSut;

	sisu::Screen::buffer b = termSut.scanLine( 0 );

        sisu::eTTYColor c = sisu::eTTYCNone;
        sisu::eTTYModifier m = sisu::eModNone;

        static TTYC const expectedTTYC[] = { TTYCTransform::color2ttyc( 'X', ++c, ++c, ++m )
	                                , TTYCTransform::color2ttyc( 'Y', ++c, ++c, ++m )
	                                , TTYCTransform::color2ttyc( 'Z', ++c, ++c, ++m )
	                                , TTYCTransform::color2ttyc( 'W', ++c, ++c, ++m ) };

	static uint8_t const bound = sizeof(expectedTTYC) / sizeof(TTYC);

	sisu::Screen::buffer line = termSut.scanLine( 0 );

	for ( uint8_t i = 0; i < bound; ++i )
	{
		line [ i ] = expectedTTYC [ i ];
	}

	for ( uint8_t i = 0; i < bound; ++i )
	{
		MUSTEQ( line [ i ],  expectedTTYC [ i ] );
	}

	termSut.refresh( );
}

TEST(TTYScreen_UT, TestSetPosition)
{
	// TODO - Add expect throws for out of bounds.
}

TEST(TTYScreen_UT, TestRefresh)
{
	sisu::TerminalScreen termSut;

	termSut.refresh( );
}

TEST(TTYScreen_UT, TestSetPixel)
{
	sisu::TerminalScreen termSut;

	uint8_t const w = termSut.getWidth( ), h = termSut.getHeight( );

	static uint8_t const sFrames = 30;

	for ( uint8_t i = 0; i < sFrames; ++i )
	{
		for ( uint8_t x = 0; x < w; ++x )
		{
			for ( uint8_t y = 0; y < h; ++y )
			{
				termSut.setPixel( x, y, sisu::TTYCMap::randomTTYC( ) );
			}
		}
	}
}
