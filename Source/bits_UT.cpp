#include "test.hpp"
#include "bits.hpp"

using sisu::bits;

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
        //std::cout << bits<float>( 0xFFFF0000 )          	<< std::endl;
        //std::cout << bits<double>( 0xFFFF0000FFFF0000 ) 	<< std::endl;
        std::cout << bits<int8_t>( 0xF0 )               	<< std::endl;
        std::cout << bits<int16_t>( 0xF0F0 )            	<< std::endl;
        std::cout << bits<int32_t>( 0xFF00FF00 )        	<< std::endl;
        std::cout << bits<int64_t>( 0x00FF00FFFFFFFF00 )        << std::endl;
        std::cout << bits<uint8_t>( 0x0F )              	<< std::endl;
        std::cout << bits<uint16_t>( 0x0F00F )          	<< std::endl;
        std::cout << bits<uint32_t>( 0x00FF00FF )       	<< std::endl;
        std::cout << bits<uint64_t>( 0x00FF00FFFFFFFFFF )  	<< std::endl;
}

