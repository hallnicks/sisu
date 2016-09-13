#if 0
#ifndef __linux__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <conio.h>
#include <io.h>
#endif


#include "test.hpp" 
#include "ttyc.hpp" 
#include "ttycolor.hpp" 
#include "ttyscreen.hpp"

#include <array>
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

TEST(TTYScreen_UT, TestSetPosition)
{
	// TODO - Add expect throws for out of bounds.
	sisu::TerminalScreen termSut;
	for (int y = 0; y < termSut.getHeight(); ++y) { 
		termSut.setPixel( 0, y, sisu::TTYCMap::randomTTYC( ) );
	}
	for (int x = 0; x < termSut.getWidth(); ++x) { 
		termSut.setPixel( x, 0, sisu::TTYCMap::randomTTYC( ) );
	}

}

TEST(TTYScreen_UT, TestScanLine)
{
	sisu::TerminalScreen termSut;

	sisu::Screen::buffer b = termSut.scanLine( 0 );

        sisu::eTTYColor c = sisu::eTTYCNone;
        sisu::eTTYModifier m = sisu::eModNone;
	char a = 'a'-1;

        static TTYC const expectedTTYC[] = { TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                           	     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m ) 
	                           	     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m ) 
	                           	     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m ) 
	                           	     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m ) 
	                           	     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m ) 
	                           	     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m ) 
	                           	     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m ) 
	                           	     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m )
	                                     , TTYCTransform::color2ttyc( ++a, ++c, ++c, ++m ) 
					 };

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

TEST(TTYScreen_UT, TestRefresh)
{
	sisu::TerminalScreen termSut;
	termSut.refresh( );
}

TEST(TTYScreen_UT, TestFill)
{
	sisu::TerminalScreen termSut;

        sisu::eTTYColor c = sisu::eTTYCNone;
        sisu::eTTYModifier m = sisu::eModNone;
	char ascii = 'a'-1;

	static sisu::eTTYColor const sColors [] = 
	{
	        sisu::eTTYCBlack,
	        sisu::eTTYCRed,
	        sisu::eTTYCGreen,
	        sisu::eTTYCYellow,
	        sisu::eTTYCBlue,
	        sisu::eTTYCMagenta ,
	        sisu::eTTYCCyan,
	        sisu::eTTYCWhite,
	};

	static sisu::eTTYModifier const sModifiers[] = 
	{
	        sisu::eModBold,
	        sisu::eModDim,
	        sisu::eModUnderline,
	        sisu::eModBlink,
	        sisu::eModReverse,
	};

	TTYC const firstColor = TTYCTransform::color2ttyc( ++ascii, sColors[0], sColors[1], sModifiers[0] );
	TTYC const secondColor = TTYCTransform::color2ttyc( ++ascii, sColors[2], sColors[3], sModifiers[0] );

	termSut.fill( firstColor );
	termSut.refresh( );

	termSut.fill( secondColor );
	termSut.refresh( );

}

TEST(TTYScreen_UT, TestSetPixel)
{
	sisu::TerminalScreen termSut;

	uint8_t const w = termSut.getWidth( ), h = termSut.getHeight( );

	static uint8_t const sFrames = 3;

	for ( uint8_t i = 0; i < sFrames; ++i )
	{
		for ( uint8_t x = 0; x < w; ++x )
		{
			for ( uint8_t y = 0; y < h; ++y )
			{
				termSut.setPixel( x, y, sisu::TTYCMap::randomTTYC( ) );
			}
		}

		termSut.refresh();
	}
}

TEST(TTYScreen_UT, TestSetPixelGradient)
{
	sisu::TerminalScreen termSut;


	uint8_t const w = termSut.getWidth( ), h = termSut.getHeight( );

	static uint8_t const sFrames = 3;
	char ascii = 'a'-1;

	for ( uint8_t i = 0; i < sFrames; ++i )
	{
		for ( uint8_t x = 0; x < w; ++x )
		{
			for ( uint8_t y = 0; y < h; ++y )
			{
			        static sisu::eTTYColor c = sisu::eTTYCNone;
			        static sisu::eTTYModifier m = sisu::eModNone;
				char ascii = 'a'-1;

	
				static sisu::eTTYModifier const sModifiers[] = 
				{
				        sisu::eModBold,
				        sisu::eModDim,
				        sisu::eModUnderline,
				        sisu::eModBlink,
				        sisu::eModReverse,
				};


				TTYC const color = TTYCTransform::color2ttyc( ++ascii
									     , ++c == sisu::eTTYCMax ? (c = sisu::eTTYCBlack) : c
									     , ++c == sisu::eTTYCMax ? (c = sisu::eTTYCBlack) : c
									     , ++m == sisu::eModMax  ? (m = sisu::eModBold) : m );

                                termSut.setPixel( x, y, color);
			}
		}
		termSut.refresh();
	}
}


#if 0
TEST(TTYScreen_UT, TestSetPixelHack)
{

	std::cout << "Hi again, bitch." << std::endl;
        HANDLE stdOut = CreateConsoleScreenBuffer( GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );
	if (stdOut == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to create handle." << std::endl;
		exit(-1);
	}

        BOOL const result1 = SetConsoleActiveScreenBuffer( stdOut );

	if (result1 == FALSE) {
		std::cout << "SetConsoleActiveScreenBuffer failed." << std::endl;
		exit(-1);
	}


	DWORD out = 0;
	for (SHORT i = 0; i < 3000; ++i) { 
		BOOL const result2 = FillConsoleOutputCharacter( stdOut, 'X', 50, { 0, i }, &out );
		if (result2 == FALSE) { 
			std::cout << "FillConsoleOutputCharacter failed" << std::endl;
			exit(-1);
		}
	}

	CloseHandle(stdOut);
}
#endif
#endif
