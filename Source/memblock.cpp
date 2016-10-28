#include "memblock.hpp"


#include <iostream>
#include <iomanip>
#include <stdint.h>

namespace sisu
{

static void memout( uint8_t * xStart
			, unsigned int xLength
			, std::ostream & xOut = std::cout
			, unsigned int xColumnWidth = 8
			, bool xFlush = false )
{
        static uint8_t const zero = '0';
        static uint8_t const space = ' ';
        static uint8_t const period = '.';

        if ( xStart != NULL )
        {
                uint8_t print = 0;

                uint32_t base = 0;

                for ( uint32_t i = 0; i < xLength; ++i )
                {
                        if ( print == 0 )
                        {
                                xOut << static_cast< void * >( xStart + i ) << " : ";
                        }

                        ( xStart[ i ] < 10 ? xOut << zero << std::hex : xOut << std::hex )
                                << xStart[ i ]
                                        << space;

                        if ( ++print == xColumnWidth || i == xLength - 1 )
                        {
                                uint32_t xDP = xColumnWidth - print;

                                if ( xDP != 0 ) { do { xOut << ". "; } while( --xDP != 0 ); }

                                xOut << "  ";

                                for ( uint32_t k = base; k < ( base + xColumnWidth ); ++k )
                                {
                                        if ( k < xLength )
                                        {
                                                uint8_t a = xStart[ k ] == '\n' ? '_' : xStart[ k ]; // TODO: Better replacement for newline char, ideally uinicode NL 

                                                xOut    << space
                                                        << static_cast<uint8_t>( a > 0x20 && a < 0x7F ? a : period )
                                                        << space;
                                        }
                                        else
                                        {
                                                xOut << " ? ";
                                        }

                                }

                                base += xColumnWidth;

	                       	xOut << std::endl;

                                print = 0;
                        }
                }

                ( xFlush ? xOut << std::flush << std::dec : xOut << std::dec );

        }
        else
        {
                xOut << "0xNULLPOINTER" << std::endl;
	}
}

static inline void memout( void * xStart, unsigned int xLength, unsigned int xColumnWidth = 8 )
{
        memout( static_cast< uint8_t * >( xStart ), xLength, xColumnWidth );
}

static inline void memout( uint8_t * xAddress1, uint8_t * xAddress2 )
{
        xAddress2 >= xAddress1 ?
                memout( xAddress1, xAddress2 - xAddress1 + 1 )
        :
                memout( xAddress2, xAddress1 - xAddress2 + 1 );
}

memblock::memblock( uint8_t * const & xStart, unsigned int xLength )
	: mStart( xStart ), mLength( xLength ), mOut( std::cout )
{
	;
}

memblock::~memblock( ) { ; }

std::ostream & memblock::operator >> ( std::ostream & xStream ) const
{
	memout( mStart, mLength );
	return xStream;
}

memblock & memblock::operator << ( endl_t xStream )
{
	return (*this) << xStream;
}

} // namespace sisu
