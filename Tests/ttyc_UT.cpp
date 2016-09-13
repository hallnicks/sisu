#include "test.hpp"
#include "ttyc.hpp"

using namespace sisu;

namespace
{
	class TTYCUT : public context
	{
		public:
			TTYCUT( ) : context( ) { }
			~TTYCUT( ) { }
			void Up( ) { }
			void Down( ) { }
	};

} // namespace

TEST(TTYCUT, getModifier)
	{ for ( eTTYModifier e = eModNone; e < eModMax; ++e )
		{ int const termModifier = TTYCMap::getModifier( e ); } }

TEST(TTYCUT, getColor)
	{ for ( eTTYColor e = eTTYCNone; e < eTTYCMax; ++e )
		{ int const termColor = TTYCMap::getColor( e ); } }

TEST(TTYCUT, randomCharacter)
	{ uint8_t const actual = TTYCMap::randomCharacter( ); }

TEST(TTYCUT, randomColor)
	{ eTTYColor const actual = TTYCMap::randomColor( ); }

TEST(TTYCUT, randomModifier)
	{ eTTYModifier const actual = TTYCMap::randomModifier( ); }

TEST(TTYCUT, randomTTYC)
	{ TTYC const actual = TTYCMap::randomTTYC( ); }

TEST(TTYCUT, incrementColor)
{	eTTYColor c = eTTYCNone;
#define EXPECT(xNextColor) MUSTEQ(xNextColor, (c = TTYCMap::incrementColor( c )))
	EXPECT(eTTYCBlack);
	EXPECT(eTTYCRed);
	EXPECT(eTTYCGreen);
	EXPECT(eTTYCYellow);
	EXPECT(eTTYCBlue);
	EXPECT(eTTYCMagenta);
	EXPECT(eTTYCCyan);
	EXPECT(eTTYCWhite);
	EXPECT(eTTYCMax);
	EXPECT(eTTYCNone);
#undef EXPECT
}

TEST(TTYCUT, operatorIncrementColor)
{	eTTYColor e = eTTYCNone;
#define EXPECT(xNextColor) MUSTEQ(xNextColor, ++e)
	EXPECT(eTTYCBlack);
	EXPECT(eTTYCRed);
	EXPECT(eTTYCGreen);
	EXPECT(eTTYCYellow);
	EXPECT(eTTYCBlue);
	EXPECT(eTTYCMagenta);
	EXPECT(eTTYCCyan);
	EXPECT(eTTYCWhite);
	EXPECT(eTTYCMax);
	EXPECT(eTTYCNone);
#undef EXPECT
}

TEST(TTYCUT, incrementModifier)
{	eTTYModifier m = eModNone;
#define EXPECT(xNextColor) MUSTEQ(xNextColor, (m = TTYCMap::incrementModifier( m )))
#if 0
	EXPECT(eModNone);
	EXPECT(eModBold);
	EXPECT(eModDim);
	EXPECT(eModUnderline);
	EXPECT(eModBlink);
	EXPECT(eModReverse);
	EXPECT(eModMax);
	EXPECT(eModNone);
#endif
#undef EXPECT
}

TEST(TTYCUT, operatorIncrementModifier)
{
	eTTYModifier m = eModNone;
#define EXPECT(xNextColor) MUSTEQ(xNextColor, ++m)
#if 0
	EXPECT(eModNone);
	EXPECT(eModBold);
	EXPECT(eModDim);
	EXPECT(eModUnderline);
	EXPECT(eModBlink);
	EXPECT(eModReverse);
	EXPECT(eModMax);
	EXPECT(eModNone);
#endif
#undef EXPECT
}

TEST(TTYCUT, ctor_dtor)
	{ TTYCTransform const color( TTYCMap::randomTTYC( ) ); }

TEST(TTYCUT, getRaw)
{	TTYC const expectedTTYC = TTYCMap::randomTTYC( );

	TTYCTransform const color( expectedTTYC );

	MUSTEQ(expectedTTYC, color.getRaw( ));
}

#if 0
TEST(TTYCUT, TTYCTransform)
{
	TTYC const expectedGetTTYC = TTYCMap::randomTTYC( );

	uint8_t eGetCharU = 0
		, eGetFGU = 0
		, eGetBGU = 0
		, eGetModU = 0;

	TTYCTransform::ttyc2uint( expectedGetTTYC
				, &eGetCharU
				, &eGetFGU
				, &eGetBGU
				, &eGetModU );

	char eGetChar = '\0';
	eTTYColor eGetFG = eTTYCNone, eGetBG = eTTYCNone;
	eTTYModifier eGetMod = eModNone;

	TTYCTransform::ttyc2color( expectedGetTTYC
				, eGetChar
				, eGetFG
				, eGetBG
				, eGetMod );

	TTYCTransform actual( expectedGetTTYC );

	MUSTEQ(eGetCharU, actual.getCharU( ));
	MUSTEQ(eGetFGU,	  actual.getFGU( ));
	MUSTEQ(eGetBGU,	  actual.getBGU( ));
	MUSTEQ(eGetModU,  actual.getModifierU( ));
	MUSTEQ(eGetChar,  actual.getChar( ));
	MUSTEQ(eGetFG, 	  actual.getFG( ));
	MUSTEQ(eGetBG, 	  actual.getBG( ));
	MUSTEQ(eGetMod,	  actual.getModifier( ));

	TTYC const expectedSetTTYC = TTYCMap::randomTTYC( );

	uint8_t eSetCharU = 0
		, eSetFGU = 0
		, eSetBGU = 0
		, eSetModU = 0;

	TTYCTransform::ttyc2uint( expectedSetTTYC
				, &eSetCharU
				, &eSetFGU
				, &eSetBGU
				, &eSetModU );

	actual.setCharU( eSetCharU );
	actual.setFGU( eSetFGU );
	actual.setBGU( eSetBGU );
	actual.setModifierU( eSetModU );

        MUSTEQ(eSetCharU, actual.getCharU( ));
        MUSTEQ(eSetFGU,   actual.getFGU( ));
        MUSTEQ(eSetBGU,   actual.getBGU( ));
        MUSTEQ(eSetModU,  actual.getModifierU( ));

	char eSetChar = '\0';
	eTTYColor eSetFG = eTTYCNone, eSetBG = eTTYCNone;
	eTTYModifier eSetMod = eModNone;

	TTYCTransform::ttyc2color( TTYCMap::randomTTYC( ), eSetChar, eSetFG, eSetBG, eSetMod );

	actual.setChar( eSetChar );
	actual.setFG( eSetFG );
	actual.setBG( eSetBG );
	actual.setModifier( eSetMod );

        MUSTEQ(eSetChar, actual.getChar( ));
        MUSTEQ(eSetFG,   actual.getFG( ));
        MUSTEQ(eSetBG,   actual.getBG( ));
        MUSTEQ(eSetMod,  actual.getModifier( ));
}

#endif
