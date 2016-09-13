#include "test.hpp"
#include "bits.hpp"
#include "drawings.hpp"
#include "memblock.hpp"
#include "signal.hpp"
#include "ttyc.hpp"
#include "ttycolor.hpp"
#include "ttyscreen.hpp"

#include <iomanip>
#include <iostream>
#include <cstring>
#include <clocale>

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

} // namespace

TEST(TTYColor_UT, ConvertColors)
{
	char const expChar = '~';
	eTTYColor const expFG = eTTYCYellow;
	eTTYColor const expBG = eTTYCRed;
	eTTYModifier const expMod = eModBlink;
	TTYC const expTTYC = 0x7E080208;
	TTYC const actTTYC = TTYCTransform::color2ttyc( expChar, expFG, expBG, expMod );

	MUSTEQ(expTTYC, actTTYC);

	char actChar = 'x';
	eTTYColor actFG = eTTYCNone, actBG = eTTYCNone;
	eTTYModifier actMod = eModNone;

	TTYCTransform::ttyc2color( actTTYC, actChar, actFG, actBG, actMod );

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
		std::cout 	<< "i = "	<< std::hex << i << std::dec
				<< "#" 		<< i
				<< std::endl;

		for ( eTTYColor j = eTTYCBlack; j < eTTYCMax; ++j )
		{
			std::cout 	<< "    , j = "	<< std::hex << j << std::dec
					<< "#" 		<< j
					<< std::endl;

			for ( eTTYModifier k = eModBold; k < eModMax; ++k )
			{
				std::cout 	<< ccolor( i, j, k )
						<< "         , mod = " 	<< std::hex << k << std::dec
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
