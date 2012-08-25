#ifndef MEMBLOCK_HPP_
#define MEMBLOCK_HPP_

#include <iostream>
#include <iomanip>
#include <stdint.h>

namespace sisu
{

class memblock
{
	unsigned int mLength;

	uint8_t * const & mStart;

	memblock( memblock const & );

	memblock & operator=( memblock const & );

	std::ostream & mOut;

	public:
		memblock( uint8_t * const & xStart, unsigned int xLength );

		~memblock( );

		template< typename XType >
		memblock & operator << ( XType xStream )
		{
			mOut << xStream;
			return (*this);
		}

                std::ostream & operator >> ( std::ostream & xStream ) const;

	        typedef std::basic_ostream<char, std::char_traits<char> > cout_t;

                typedef cout_t & ( * endl_t )( cout_t & );

		memblock & operator << ( endl_t xStream );
};

inline std::ostream & operator << ( std::ostream & xStream, memblock const & xBlock )
{
        return xBlock >> xStream;
}

} // namespace sisu

#endif // MEMBLOCK_HPP_
