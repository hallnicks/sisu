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
using sisu::eTTYColor;
using sisu::eTTYCBlack;
using sisu::eTTYCMax;
using sisu::eTTYModifier;

using sisu::eModBold;
using sisu::eModBlink;
using sisu::eModMax;

namespace
{
        class TTYScreen_UT : public context
        {
		protected:
		        TerminalScreen mScreen;

			Drawings mPainter;

			char * mLocale;

                public:
                        TTYScreen_UT( )
				: context( )
				, mScreen( )
				, mPainter( &mScreen )
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

			void clear( )
			{
				eTTYColor fg = TTYCMap::randomColor( ), bg = TTYCMap::randomColor( );

        			unsigned int const w = mScreen.getWidth( ), h = mScreen.getHeight( );

				mPainter.clear( color2ttyc( toggleChar( ), fg, bg, eModBold ) );
			}

			static char toggleChar( )
			{
			        static char const charas[] = { '@', '#', '$', '%', '&', '_', '=',
								'Q', 'P', 'W', 'U', 'T', 'X', 'M',
								'H', '0' };

				static size_t const bound = sizeof(charas) / sizeof(char const);

				static int pos = -1;

				return charas[ ++pos == bound ? pos = 0 : pos ];
			}
        };
}

TEST(TTYScreen_UT, TerminalClear)
{
	clear( );

	mScreen.refresh( );
}

TEST(TTYScreen_UT, TerminalMandlebrot)
{
        static double const step = 0.1;

        eTTYColor ii, jj;

        double dub = step;

	clear( );

	for ( ; dub <= 1.00; dub += step )
	{
        	ii = TTYCMap::randomColor( );

		jj = TTYCMap::randomColor( );

               	mPainter.mandlebrot( color2ttyc( toggleChar( ), ii, jj, eModBold ), dub );

                mScreen.refresh( );
        }
}

TEST(TTYScreen_UT, TerminalMidPointCircle)
{
	eTTYColor ii, jj;

	eTTYModifier kk = eModBold;

	unsigned int const rBound = ( mScreen.getWidth( ) + mScreen.getHeight( ) ) / 2;

	for ( unsigned int i = 0; i < 30; ++i )
	{
	        ii = TTYCMap::randomColor( ), jj = TTYCMap::randomColor( );

		clear( );

		mPainter.midpointCircle( mScreen.getWidth( ) % rand( )
					, mScreen.getHeight( ) % rand( )
					, rand( ) % rBound
					, color2ttyc( 'H', ii, jj, kk ) );

		mScreen.refresh( );
	}
}

TEST(TTYScreen_UT, Noise)
{
        eTTYColor fg = eTTYCBlack, bg = eTTYCBlack;

        eTTYModifier mod = eModBold;

	unsigned const interval = 10 + ( rand( ) % 30 );

	unsigned int const w = mScreen.getWidth( ), h = mScreen.getHeight( );

	clear( );

	for ( unsigned int i = 0; i < interval; ++i )
	{
		for ( unsigned int ww = 0; ww < w; ++ww )
		{
			for ( unsigned int hh = 0; hh < h; ++hh )
			{
				fg = TTYCMap::randomColor( );

				bg = TTYCMap::randomColor( );

				mod = TTYCMap::randomModifier( );

				mPainter.setPixel( ww, hh, color2ttyc( 'D', fg, bg, mod ) );
			}
		}

		mScreen.refresh( );
	}
}

TEST(TTYScreen_UT, TerminalHLines)
{
	unsigned int width = 0, start = 0;

        eTTYColor fg = eTTYCBlack, bg = eTTYCBlack;

        eTTYModifier mod = eModBold;

	clear( );

	for ( unsigned int y = 0; y < mScreen.getHeight( ); ++y )
	{
		width = ( rand( ) % ( mScreen.getWidth( ) - 50 ) ) + 50;

		start = rand( ) % width;

		fg = TTYCMap::randomColor( );

		bg = TTYCMap::randomColor( );

		mod = TTYCMap::randomModifier( );

		mPainter.hline( y, start, width, color2ttyc( 'S', fg, bg, mod ) );

		mScreen.refresh( );
	}

	width = mScreen.getWidth( );

	start = 0;

	clear( );

	for ( unsigned int y = 0; y < mScreen.getHeight( ); ++y )
	{
		fg = TTYCMap::randomColor( );

		bg = TTYCMap::randomColor( );

		mod = TTYCMap::randomModifier( );

		mPainter.hline( y, start, width, color2ttyc( 'D', fg, bg, mod ) );

		mScreen.refresh( );
	}
}

TEST(TTYScreen_UT, TerminalRects)
{
	unsigned int const w = mScreen.getWidth( ), h = mScreen.getHeight( );

//	Quad quad( );
//	d.hquad( );
}
