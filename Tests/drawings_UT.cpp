#if 0
#include "drawings.hpp"
#include "test.hpp"
#include "ttycolor.hpp"
#include "ttyscreen.hpp"

#include <ctime>
#include <clocale>

using sisu::TerminalScreen;

using sisu::Drawings;

using sisu::TTYC;
using sisu::TTYCMap;
using sisu::TTYString;

using sisu::eTTYColor;
using sisu::eTTYCBlack;
using sisu::eTTYCMax;
using sisu::eTTYModifier;

using sisu::eModBold;
using sisu::eModBlink;
using sisu::eModMax;

namespace
{
        class Drawings_UT : public context
        {
		protected:
		        TerminalScreen mScreen;

			char * mLocale;

                public:
			Drawings_UT( )
				: context( )
				, mScreen( )
				, mLocale( 0 )
			{
				;
			}

                        ~Drawings_UT( ) { }

                        void Up( )
			{
				srand( time( NULL ) );

				mLocale = std::setlocale( LC_ALL, NULL );

			        std::setlocale( LC_ALL, "en_US.UTF-8" );
			}

                        void Down( ) { std::setlocale( LC_ALL, mLocale ); }

			void clear( Drawings * xPainter ) { xPainter->clear( TTYCMap::randomTTYC( ) ); }
        };
}

// To debug a specific mode, use the appropriate definition
//#define FORALLMODES(xVar) sisu::eDrawingMode xVar = sisu::eRedrawImmediately;
//#define FORALLMODES(xVar) sisu::eDrawingMode xVar = sisu::eVsync;
//#define FORALLMODES(xVar) sisu::eDrawingMode xVar = sisu::eOnlyChanged;

// For regular testing, use this.
#define FORALLMODES(xVar) for ( sisu::eDrawingMode xVar = sisu::eRedrawImmediately; xVar < sisu::eDrawingModeMax; ++xVar )

#define PAINTERSUT Drawings painterSut( &mScreen, e);

TEST(Drawings_UT, TerminalClear)
{
	FORALLMODES(e)
	{
		PAINTERSUT;

		clear( &painterSut );

		mScreen.refresh( );
	}
}

TEST(Drawings_UT, TerminalMandlebrot)
{
	FORALLMODES(e)
	{
		PAINTERSUT;

	        static double const step = 0.1;

	        double dub = step;

		clear( &painterSut );

		for ( ; dub <= 1.00; dub += step )
		{
	               	painterSut.mandlebrot( TTYCMap::randomTTYC( ), dub );

			painterSut.refresh( );
	        }
	}
}

TEST(Drawings_UT, TerminalMidPointCircle)
{
	FORALLMODES(e)
	{
		PAINTERSUT;

		unsigned int const rBound = ( mScreen.getWidth( ) + mScreen.getHeight( ) ) / 2;

		for ( unsigned int i = 0; i < 30; ++i )
		{
			clear( &painterSut );

			painterSut.midpointCircle( mScreen.getWidth( ) % rand( )
						, mScreen.getHeight( ) % rand( )
						, rand( ) % rBound
						, TTYCMap::randomTTYC( ) );

			painterSut.refresh( );
		}
	}
}

TEST(Drawings_UT, Noise)
{
	FORALLMODES(e)
	{
		PAINTERSUT;

		unsigned const interval = 10 + ( rand( ) % 30 );

		unsigned int const w = mScreen.getWidth( ), h = mScreen.getHeight( );

		clear( &painterSut );

		for ( unsigned int i = 0; i < interval; ++i )
		{
			for ( unsigned int ww = 0; ww < w; ++ww )
			{
				for ( unsigned int hh = 0; hh < h; ++hh )
				{
					painterSut.setPixel( ww, hh, TTYCMap::randomTTYC( ) );
				}
			}

			painterSut.refresh( );
		}
	}
}

TEST(Drawings_UT, TerminalHLines)
{
	FORALLMODES(e)
	{
		PAINTERSUT;

		unsigned int width = -1, start = 0;

		clear( &painterSut );

		for ( unsigned int y = 0; y < mScreen.getHeight( ); ++y )
		{
			start = 0;

			painterSut.hline( y, start, ++width, TTYCMap::randomTTYC( ) );

			painterSut.refresh( );
		}

		width = mScreen.getWidth( );

		start = 0;

		clear( &painterSut );

		for ( unsigned int y = 0; y < mScreen.getHeight( ); ++y )
		{
			painterSut.hline( y, start, width, TTYCMap::randomTTYC( ) );

			painterSut.refresh( );
		}
	}
}

TEST(Drawings_UT, TerminalTextBlocks)
{
	FORALLMODES(e)
	{
		PAINTERSUT;

#define SIZEOF(x1) sizeof(x1) / sizeof(char const)

		static char const m1[] = "0 : Kok-le xovu ortuzunda";
		static char const m2[] = "1 : Xolbelcijneen kok-le xadi";
		static char const m3[] = "2 : Xolegelenip seriitkengen";
		static char const m4[] = "3 : Xookuy bodum, corumal bodum ...";

		clear( &painterSut );

		TTYString< SIZEOF(m1) > ms1( m1, TTYCMap::randomTTYC( ) );
		TTYString< SIZEOF(m2) > ms2( m2, TTYCMap::randomTTYC( ) );
		TTYString< SIZEOF(m3) > ms3( m3, TTYCMap::randomTTYC( ) );
		TTYString< SIZEOF(m4) > ms4( m4, TTYCMap::randomTTYC( ) );

		painterSut.drawText< SIZEOF(m1) >( 5, 10, ms1 );
//		painterSut.drawText< SIZEOF(m2) >( 5, 11, ms2 );
//		painterSut.drawText< SIZEOF(m3) >( 5, 12, ms3 );
//		painterSut.drawText< SIZEOF(m4) >( 5, 13, ms4 );

		painterSut.refresh( );
	}

//	while(1) { }
}

TEST(Drawings_UT, TerminalRects)
{
	FORALLMODES(e)
	{
		PAINTERSUT;

		clear( &painterSut );

		static sisu::ThoughtBox const box( "Test Message", 1, 1, 1, 1, TTYCMap::randomTTYC( ), TTYCMap::randomTTYC( ) );

		painterSut.thoughtBox( box );

		painterSut.refresh( );
	}
}

#endif
