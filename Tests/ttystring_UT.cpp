#include "test.hpp"
#include "ttyc.hpp"
#include "ttycolor.hpp"
#include "ttystring.hpp"
#include "bits.hpp"

using sisu::bits;
using sisu::TTYC;
using sisu::TTYCMap;
using sisu::TTYString;
using sisu::TTYCTransform;

namespace
{
	class TTYString_UT : public context
	{
		public:
			TTYString_UT( ) : context( ) { }
			~TTYString_UT( ) { }
			void Up( ) { }
			void Down( ) { }
	};

} // namespace

TEST(TTYString_UT, BuildBasicString)
{
	TTYC const expected = TTYCMap::randomTTYC( );

	static char const string[] = "1234";

	TTYString<5> actual( string, expected );

	MUSTEQ(sizeofarray(string), 5);
	for ( size_t ii = 0; ii < sizeofarray( string ); ++ii )
	{
		TTYCTransform t( actual [ ii ] );
		std::cout << "actual[" << ii << "] = " << bits<TTYC>( actual[ ii ] ) << std::endl;
		std::cout << "string[" << ii << "].getChar() = " << bits<char>( string[ ii ] ) << std::endl;
		std::cout << "actual[" << ii << "].getChar() = " << bits<char>( t.getChar( ) ) << std::endl;
	}

}
