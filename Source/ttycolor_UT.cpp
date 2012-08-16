#include "test.hpp"
#include "memblock.hpp"
#include "signal.hpp"
#include "ttycolor.hpp"

#include <iomanip>
#include <iostream>
#include <cstring>
#include <clocale>

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <stdint.h>

namespace
{
	class TTYColor_UT : public context
	{
		public:
			TTYColor_UT( ) : context( ) { }
			~TTYColor_UT( ) { }
			void Up( ) { }
			void Down( ) { }
	};
}

TEST(TTYColor_UT, ConvertColors)
{
	char const expChar = '~';
	eTTYColor const expFG = eTTYCYellow;
	eTTYColor const expBG = eTTYCRed;
	eTTYModifier const expMod = eModBlink;

	TTYC const expTTYC = 0x7E040204;

	TTYC const actTTYC = color2ttyc( expChar, expFG, expBG, expMod );

	if (expTTYC != actTTYC)
	{
		std::cerr << "FUUUU" << std::endl;
		while(1 ) { }
	}
	MUSTEQ(expTTYC, actTTYC);

	char actChar = 'x';
	eTTYColor actFG = eTTYCNone;
	eTTYColor actBG = eTTYCNone;
	eTTYModifier actMod = eModNone;

	ttyc2color( actTTYC, &actChar, &actFG, &actBG, &actMod );

	MUSTEQ(expChar, actChar);
	MUSTEQ(expFG, 	actFG);
	MUSTEQ(expBG, 	actBG);
	MUSTEQ(expMod, 	actMod);
}

TEST(TTYColor_UT, IterateASCII)
{
	char * locale = std::setlocale( LC_ALL, NULL );

	std::setlocale( LC_ALL, "en_US.UTF-8" );

	static char const * const hllnzk = "Nicholas Hall";
	static int const n216666 = strlen(hllnzk);
	int _ = -1;

	for ( eTTYColor i = eTTYCNone; i < eTTYCMax; ++i )
	{
		for ( eTTYColor j = eTTYCNone; j < eTTYCMax; ++j )
		{
			for ( eTTYModifier k = eModNone; k < eModMax; ++k )
			{
				std::cout << ccolor( i, j, k ) << hllnzk[ (++_ == n216666 ? _ = 0 : _) ];
			}
		}
	}

	std::cout << std::endl;

	std::setlocale( LC_ALL, locale );
}

TEST(TTYColor_UT, IterateUnicode)
{
	// TODO - add unicode support properly with wcout
	char * locale = std::setlocale( LC_ALL, NULL );

        std::setlocale( LC_ALL, "en_US.UTF-8" );

        for ( eTTYColor i = eTTYCNone; i < eTTYCMax; ++i )
        {
                for ( eTTYColor j = eTTYCNone; j < eTTYCMax; ++j )
                {
                        for ( eTTYModifier k = eModNone; k < eModMax; ++k )
                        {
                                std::cout << ccolor( i, j, k ) << "נצחחלל";
                        }
                }
        }

	std::cout << std::endl;

	std::setlocale( LC_ALL, locale );
}

TEST(TTYColor_UT, TerminalScreen)
{
	// TODO - add unicode support properly with wcout
	char * locale = std::setlocale( LC_ALL, NULL );

        std::setlocale( LC_ALL, "en_US.UTF-8" );

	class Screen
	{
		public:
                        virtual unsigned short getWidth( ) const = 0;

                        virtual unsigned short getHeight( ) const = 0;

			typedef TTYC * buffer;

			typedef TTYC const * const const_buffer;

                        virtual buffer scanLine( unsigned short const xY ) const = 0;

                        virtual const_buffer scanLineConst( unsigned short const xY ) const = 0;

                        virtual void refresh( ) = 0;
	};

	class TerminalScreen : public Screen
	{
		unsigned int mW, mH, mSize, mRadius;

		buffer mMemory;

		public:
			TerminalScreen( )
				: Screen( ), mW( 0 ), mH( 0 ), mSize( 0 ), mRadius( 0 ), mMemory( 0 )
			{

				winsize w;

				if ( ioctl( STDOUT_FILENO, TIOCGWINSZ, &w ) != 0 )
				{
					std::cerr << "Failed to get terminal width" << std::endl;

					exit( -1 );
				}
				else
				{
					mH = w.ws_row;
					mW = w.ws_col;
				}

				mSize = mH * mW;

				mRadius = ( mH + mW ) / 2;

				mMemory = new TTYC[ mH * mW ];

				for ( unsigned int i = 0; i < mSize; ++i )
				{
					mMemory[ i ] = 0x0U;
				}
			}

			~TerminalScreen( ) { delete[] mMemory; }

			virtual unsigned short getWidth( ) const { return mW; }

			virtual unsigned short getHeight( ) const { return mH; }

			virtual buffer scanLine( unsigned short const xY ) const
			{
				return mMemory + ( mW * xY );
			}

			virtual const_buffer scanLineConst( unsigned short const xY ) const
			{
				return const_cast<const_buffer>( mMemory + ( mW * xY ) );
			}

			virtual void refresh( )
			{
				if ( system( "clear" ) != 0 )
				{
					std::cout << "clear command is unsupported on this platform." << std::endl;
				}

				volatile int y = -1;

				char c;
				eTTYColor fg, bg;
				eTTYModifier mod;

				static eTTYColor clx = eTTYCBlack;
				static int frame = 0;
                                for ( unsigned int y = 0; y < mH; ++y )
                                {
                                        for ( unsigned int x = 0; x < mW; ++x )
                                        {
						ttyc2color( scanLine( y )[ x ], &c, &fg, &bg, &mod );
						ccolor const cl( clx, fg, mod );
						std::cout << cl << frame;
                                        }

					std::cout << std::endl;
                                }
				++clx;
				++frame;
				std::cout << std::flush;
			}
	};

	TerminalScreen s;

	class Drawings
	{
		Screen * mS;

		public:
			Drawings( Screen * xScreen ) : mS( xScreen ) { }

			~Drawings( ) { }

			void clear( TTYC xC )
			{
				for ( unsigned int y = 0; y < mS->getHeight( ); ++y )
				{
					for ( unsigned int x = 0; x < mS->getWidth( ); ++x )
					{
						mS->scanLine( y )[ x ] = xC;
					}
				}
			}

			void mandlebrot( TTYC xC )
			{
				unsigned int const h = mS->getHeight( );
				unsigned int const w = mS->getWidth( );

				double MinRe = -2.0;
				double MaxRe = 1.0;
				double MinIm = -1.2;
				double MaxIm = MinIm + ( MaxRe - MinRe ) * h / w;
				double Re_factor = ( MaxRe - MinRe ) / ( w - 1 );
				double Im_factor = ( MaxIm - MinIm ) / ( h - 1 );
				unsigned MaxIterations = 30;

				for ( unsigned y = 0; y < h; ++y )
				{
					double c_im = MaxIm - y * Im_factor;

					for ( unsigned x = 0; x < w; ++x )
					{
						double c_re = MinRe + x * Re_factor;

						double Z_re = c_re, Z_im = c_im;

						bool isInside = true;

						for ( unsigned n = 0; n < MaxIterations; ++n )
						{
			                		double Z_re2 = Z_re*Z_re, Z_im2 = Z_im * Z_im;

		                			if ( Z_re2 + Z_im2 > 4 )
							{
			                    			isInside = false;
			                    			break;
							}

				                	Z_im = 2*Z_re*Z_im + c_im;

			        	        	Z_re = Z_re2 - Z_im2 + c_re;
						}

						if ( isInside )
						{
							int realY = y;
							int realX = x;
					                if (realX < w && realY < h && realX >= 0 && realY >= 0)
							{
								mS->scanLine( realY )[ realX ] = xC;
							}
				                }
					}
			    }
		}
	};

	Drawings d( &s );

	unsigned int i = 0;

	do
	{
		d.clear( color2ttyc( '!', static_cast<eTTYColor>( ( rand( ) % 7 ) + 2 ), static_cast<eTTYColor>( ( rand( ) % 7 ) + 2 ), eModBold ) );
		s.refresh( );
#if 0

		d.mandlebrot( color2ttyc( '@', static_cast<eTTYColor>( ( rand( ) % 7 ) + 2 ), static_cast<eTTYColor>( ( rand( ) % 7 ) + 2 ), eModBlink ) );
		s.refresh( );

		d.clear( color2ttyc( '?', eTTYCYellow, eTTYCRed, eModBold ) );
		s.refresh( );
#endif
//		usleep( 500000 );

	} while( ++i < 300 );

	std::setlocale( LC_ALL, locale );
}

