#ifdef MEMBLOCK_UT
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
#define BLOCK_SIZE 304
	uint8_t * block = new uint8_t[ BLOCK_SIZE ];

	unsigned int c;

	for ( unsigned int i = 0; i < BLOCK_SIZE; ++i )
	{
		c = std::clock( );

		block[ i ] = rand( ) % 0xFF;
	}

	std::cout << memblock( block, BLOCK_SIZE ) << std::endl;

	delete[] block;
}
#endif // MEMBLOCK_UT
