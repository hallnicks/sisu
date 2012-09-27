#include "test.hpp"

#include "bits.hpp"
#include "memblock.hpp"

#include <stdint.h>
#include <iostream>
#include <limits>
#include <cstring>
#include <ctime>
#include <clocale>
#include <climits>

using namespace sisu;

namespace
{

class memblock_UT : public context
{
	public:
		memblock_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(memblock_UT, memout)
{
	uint8_t * block = new uint8_t[ 300 ];

	unsigned int c;

	for ( unsigned int i = 0; i < 300; ++i )
	{
		c = std::clock( );

		block[ i ] = rand( ) % 0xFF;
	}

	std::cout << memblock( block, 300 ) << std::endl;

	delete[] block;
}
