#include "test.hpp"
#include "ttyc.hpp"

using sisu::TTYC;
using sisu::TTYCMap;
using sisu::TTYCTransform;

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

} // namespace sisu

TEST(TTYCUT, getColorIndex)
{
}

TEST(TTYCUT, getModifierIndex)
{

}

TEST(TTYCUT, randomCharacter)
{

}

TEST(TTYCUT, randomColor)
{

}

TEST(TTYCUT, randomModifier)
{

}

TEST(TTYCUT, randomTTYC)
{

}

TEST(TTYCUT, incrementColor)
{

}

TEST(TTYCUT, operatorIncrementColor)
{

}

TEST(TTYCUT, incrementModifier)
{

}

TEST(TTYCUT, operatorIncrementModifier)
{

}

TEST(TTYCUT, ctor_dtor)
{
	TTYCTransform const color( TTYCMap::randomTTYC( ) );
}

TEST(TTYCUT, getRaw)
{
	TTYC const expectedTTYC = TTYCMap::randomTTYC( );

	TTYCTransform const color( expectedTTYC );

	MUSTEQ(expectedTTYC, color.getRaw( ));
}

TEST(TTYCUT, getCharU)
{

}
