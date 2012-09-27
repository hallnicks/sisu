#include "test.hpp"
#include "bits.hpp"

// TODO - Fix Deficiencies: Assumes little endian for now. Need to think of a good way to mock up endianness from the compiler side.
using sisu::bits;
using sisu::eUpperLower;
using sisu::eUL_Lower;
using sisu::eUL_Upper;

namespace
{

class bits_UT : public context
{
	public:
		bits_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(bits_UT, bits_std_cout)
{
        std::cout << bits<float>( 0xFFFF0000 )           << std::endl;
        std::cout << bits<double>( 0xFFFF0000FFFF0000 )  << std::endl;
        std::cout << bits<int32_t>( 0xFF00FF00 )         << std::endl;
        std::cout << bits<int32_t>( 0xF0F0F0F0 )         << std::endl;
        std::cout << bits<int32_t>( 0x0F0F0F0F )         << std::endl;
        std::cout << bits<uint32_t>( 0xFF00FF00 )        << std::endl;
        std::cout << bits<uint32_t>( 0xF0F0F0F0 )        << std::endl;
        std::cout << bits<uint32_t>( 0x0F0F0F0F )        << std::endl;
        std::cout << bits<int8_t>( 0xF0 )                << std::endl;
        std::cout << bits<uint8_t>( 0xF0 )               << std::endl;
        std::cout << bits<int16_t>( 0xF0F0 )             << std::endl;
        std::cout << bits<int32_t>( 0xFF00FF00 )         << std::endl;
        std::cout << bits<int64_t>( 0x00FF00FFFFFFFF00 ) << std::endl;
        std::cout << bits<uint8_t>( 0x0F )               << std::endl;
        std::cout << bits<uint16_t>( 0x0F00F )           << std::endl;
        std::cout << bits<uint32_t>( 0x00FF00FF )      	 << std::endl;
        std::cout << bits<uint64_t>( 0x00FF00FFFFFFFFFF )<< std::endl;
}

TEST(bits_UT, bitsSetGetNibbleUpperLower)
{
	bits<int32_t> bitSut( 0xFFFFFFFF );
	for ( size_t i = 0; i < bits<int32_t>::numberOfBytes( ); ++i )
	{
	        std::cout << ( bitSut [ i ] [ eUL_Lower ] = 0xF0 ) << std::endl;
	        std::cout << ( bitSut [ i ] [ eUL_Upper ] = 0x0F ) << std::endl;
	}
}

TEST(bits_UT, bitsSetGetByteAtIndex)
{
	bits<int32_t> bitSut( 0xFFFFFFFF );

	for ( size_t i = 0; i < bits<int32_t>::numberOfBytes( ); ++i )
		{ std::cout << ( bitSut [ i ] = 0xF0 ) << std::endl; }

	for ( size_t i = 0; i < bitSut.numberOfBytes( ); ++i )
		{ std::cout << ( bitSut [ i ] = 0x0F ) << std::endl; }
}
