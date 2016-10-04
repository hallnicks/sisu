// This file is part of sisu.

// sisu is free software: you can redistribute it and/or modify // it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or // (at your option) any later version.

// sisu is distributed in the hope that it will be useful, // but WITHOUT ANY WARRANTY; without even the implied warranty of //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the // GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.
#ifndef PACKET_3F1C3B4EFD88490DA490A75A8E7AB058_HPP_
#define PACKET_3F1C3B4EFD88490DA490A75A8E7AB058_HPP_

#include "memblock.hpp"

namespace sisu {

template< uint64_t XSize >
class Packet
{
	protected:
		uint8_t * mHeapBuffer;
		uint64_t  mOffset;

	public:
		Packet( uint8_t * xBuffer = NULL, uint64_t const xData = 0 )
			: mHeapBuffer( xBuffer != NULL ? xBuffer : new uint8_t[ XSize ] )
			, mOffset( 0 )
		{

		}

		~Packet( )
		{
			delete[] mHeapBuffer;
		}

		uint8_t * getBuffer( ) const { return mHeapBuffer; }

		void operator+=( uint64_t const xAmount )
		{
			mOffset += xAmount;
		}

		void writeBuffer( uint8_t * const xData, uint64_t const xSize )
		{
			if ( ( mOffset + xSize ) >= XSize )
			{
				std::cerr << "Packet length " << XSize << " exceeded." << std::endl;
				exit( -1 );
			}

			memcpy( mHeapBuffer + mOffset, xData, xSize );

			mOffset += xSize;
		}

		uint64_t getOffset( ) const { return mOffset; }

		uint64_t getSize( ) const { return XSize; }

		void operator( )( std::function<void(uint8_t*,uint64_t)> xLambda )
		{
			xLambda( mHeapBuffer + mOffset, XSize - mOffset );
		}

		void print( )
		{
			std::cout << memblock( mHeapBuffer, mOffset ); 
		}
};

} // namespace sisu
#endif // PACKET_3F1C3B4EFD88490DA490A75A8E7AB058_HPP_
