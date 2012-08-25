#include "test.hpp"
#include "bits.hpp"

#include "drawings.hpp"
#include "memblock.hpp"
#include "signal.hpp"
#include "ttycolor.hpp"

#include <iomanip>
#include <iostream>
#include <cstring>
#include <clocale>

#include "ttyscreen.hpp"

#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <stdint.h>

using namespace sisu;

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
	TTYC const expTTYC = 0x7E080208;
	TTYC const actTTYC = color2ttyc( expChar, expFG, expBG, expMod );

	MUSTEQ(expTTYC, actTTYC);

	char actChar = 'x';
	eTTYColor actFG = eTTYCNone, actBG = eTTYCNone;
	eTTYModifier actMod = eModNone;

	ttyc2color( actTTYC, &actChar, &actFG, &actBG, &actMod );

	MUSTEQ(expChar, actChar);
	MUSTEQ(expFG, 	actFG);
	MUSTEQ(expBG, 	actBG);
	MUSTEQ(expMod, 	actMod);
}

TEST(TTYColor_UT, SimpleColor)
{
	char * locale = std::setlocale( LC_ALL, NULL );

	std::setlocale( LC_ALL, "en_US.UTF-8" );

	for ( eTTYColor i = eTTYCBlack; i < eTTYCMax; ++i )
	{
		std::cout 	<< "i = "	<< getColorStr( i )
				<< "#" 		<< i
				<< std::endl;

		for ( eTTYColor j = eTTYCBlack; j < eTTYCMax; ++j )
		{
			std::cout 	<< "    , j = "	<< getColorStr( j )
					<< "#" 		<< j
					<< std::endl;

			for ( eTTYModifier k = eModBold; k < eModMax; ++k )
			{
				std::cout 	<< ccolor( i, j, k )
						<< "         , mod = " 	<< getModifier( k )
						<< "#" << k
						<< std::endl;
			}
		}
	}

	std::setlocale( LC_ALL, locale );

}

TEST(TTYColor_UT, IterateASCII)
{
	char * locale = std::setlocale( LC_ALL, NULL );

	std::setlocale( LC_ALL, "en_US.UTF-8" );

	static char const * const hllnzk = "Nicholas Hall ";
	static int const n216666 = strlen( hllnzk );
	int _ = -1;

	for ( eTTYColor i = eTTYCBlack; i < eTTYCMax; ++i )
	{
		for ( eTTYColor j = eTTYCBlack; j < eTTYCMax; ++j )
		{
			for ( eTTYModifier k = eModBold; k < eModMax; ++k )
			{
				//std::cout << "Indexer is at " << _ << " this value." << std::endl;
				//std::cout << "hllnzk[ " << _  << "] = " << hllnzk[ (++_ >= n216666 ? _ = 0 : _ ) ] << std::endl;
				std::cout << ccolor( i, j, k ) << hllnzk[ (++_ >= n216666 ? _ = 0 : _ ) ];
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

        for ( eTTYColor i = eTTYCBlack; i < eTTYCMax; ++i )
                for ( eTTYColor j = eTTYCBlack; j < eTTYCMax; ++j )
                        for ( eTTYModifier k = eModBold; k < eModMax; ++k )
                                { std::cout << ccolor( i, j, k ) << "נצחחלל"; }

	std::cout << std::endl;

	std::setlocale( LC_ALL, locale );
}


TEST(TTYColor_UT, TerminalScreen)
{
	// TODO - add unicode support properly with wcout
	char * locale = std::setlocale( LC_ALL, NULL );

        std::setlocale( LC_ALL, "en_US.UTF-8" );

	TerminalScreen s;

	Drawings d( &s );

	unsigned int i = 0;

#if 0
	do
	{
		d.clear( color2ttyc( '!', eTTYCYellow, eTTYCBlack, eModBold ) );
		s.refresh( );

	} while( ++i < 30 );

	usleep( 50000 );

#endif

	do
	{
		d.clear( color2ttyc( '!', eTTYCGreen, eTTYCBlack, eModBold ) );
		d.mandlebrot( color2ttyc( '@', eTTYCYellow, eTTYCBlack, eModBlink ) );
		s.refresh( );

	} while( ++i < 30 );




	std::setlocale( LC_ALL, locale );
	for ( int i = 0; i < 30; ++i ) {
		std::cout << "Alakazam!" << std::endl;
	}
}
