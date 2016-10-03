// This file is part of sisu.

// sisu is free software: you can redistribute it and/or modify // it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or // (at your option) any later version.

// sisu is distributed in the hope that it will be useful, // but WITHOUT ANY WARRANTY; without even the implied warranty of //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the // GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.
#include "test.hpp"

#include "threadgears.hpp"
#include "stacktrace.hpp"
#include "ttyc.hpp"
#include "ttycolor.hpp"
#include "memblock.hpp"

#include <concurrentqueue.h>

using namespace sisu;

namespace
{

class tsq_UT : public context
{
	public:
		tsq_UT( ) : context( ) { }
		void Up() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
		void Down() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

template< uint64_t XSize >
class Packet
{
	protected:
		uint8_t * mHeapBuffer;
		uint64_t  mOffset;

	public:
		Packet( )
			: mHeapBuffer( new uint8_t[ XSize ] )
			, mOffset( 0 )
		{
			;
		}

		~Packet( )
		{
			delete[] mHeapBuffer;
		}

		uint8_t * getBuffer( ) const { return mHeapBuffer; }

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

		void operator( )( std::function<void(uint8_t*,uint64_t)> xLambda )
		{
			xLambda( mHeapBuffer + mOffset, XSize - mOffset );
		}
};

} // namespace

typedef Packet<666> AudioPacket;

TEST(tsq_UT, ThreadSafeQueueTrivial)
{
	moodycamel::ConcurrentQueue< AudioPacket * > q;
	event quit;

	tooth< Packet< 666 > , uint64_t> t;

	gear< uint32_t, uint32_t > producerThread( [ & ] (  uint32_t xThreadID )
	{
		std::cout << "In thread: " << ++t << std::endl;

		do
		{
			sleep::ms( rand( ) % 1000 );

			AudioPacket * p = new AudioPacket( );

			// randomize p's data.
			(*p)( [ ] ( uint8_t * xData, uint64_t xSize )
				{
					for ( uint64_t ii = 0; ii < xSize; ++ii )
					{
						xData[ ii ] = rand( ) % 255;
					}
				}
			 );


			std::cout << ccolor( eTTYCBlack, eTTYCRed, eModNone )
				  << "Server " << xThreadID << std::endl;

			q.enqueue( p );

		} while ( !quit.isSet( ) );

		return 0;
	} );

	gear< uint32_t, uint32_t > consumerThread( [ & ] (  uint32_t xThreadID )
	{

		std::cout << "In thread: " << ++t << std::endl;

		do
		{
			//sleep::ms( rand( ) % 1000 );

			AudioPacket * b = NULL;

			if ( !q.try_dequeue( b ) || b == NULL )
			{
				continue;
			}

			std::cout << ccolor( eTTYCBlack, eTTYCYellow, eModNone )
				  << "Client " << xThreadID
				  << std::endl;

			delete b;

		} while ( !quit.isSet( ) );

		return 0;
	} );


	for (int ii = 0; ii < 15; ++ii )
	{
		if ( ii % 2 == 0 )
		{
			std::cout << "Launching new thread." << std::endl;
			consumerThread( ii );
		}
		else
		{
			std::cout << "Launching new thread." << std::endl;
			producerThread( ii );
		}
	}


	sleep::ms( 3000 );
	quit.set( );
}
