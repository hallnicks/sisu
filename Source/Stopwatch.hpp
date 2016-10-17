#ifndef STOPWATCH_7DE304E10B734077A36B86CA99E5B0F2_HPP_
#define STOPWATCH_7DE304E10B734077A36B86CA99E5B0F2_HPP_

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>

namespace sisu {

class Stopwatch
{
#ifdef WIN32
	double mFrequency;

	__int64 mCounterStart;
#else
	timespec mStart;
	bool mNs;
#endif


#ifdef WIN32
	void _start( double const xDenominator )
	{
		LARGE_INTEGER li;

		if ( !QueryPerformanceFrequency( &li ) )
		{
			std::cerr << "QueryPerformanceFrequency( .. ) failed." << std::endl;
			exit( -1 );
		}

		mFrequency = double( li.QuadPart ) / xDenominator;

		QueryPerformanceCounter( &li );

		mCounterStart = li.QuadPart;

	}
#else
	void _start( bool const xNs )
	{
		clock_gettime( CLOCK_REALTIME, &mStart );

		mNs = xNs;
	}
#endif

	public:
		Stopwatch( )
#ifdef WIN32
			: mFrequency( 0.0 )
			, mCounterStart( 0 )
#else
			: mStart( { 0, 0 } )
			, mNs( false )
#endif
		{
			;
		}

#ifdef WIN32
		void startMs( ) { _start( 1000.0 ); 	  }

		void startNs( ) { _start( 1000000000.0 ); }
#else
		void startMs( ) { _start( false ); 	  }

		void startNs( ) { _start( true  );        }
#endif

		double stop( )
		{
#ifdef WIN32
			LARGE_INTEGER li;

			QueryPerformanceCounter( &li );

			return double( li.QuadPart - mCounterStart ) / mFrequency;
#else
			timespec end;

			clock_gettime( CLOCK_REALTIME, &end );

			time_t const dSec  = end.tv_sec  - mStart.tv_sec;
			long   const dNSec = end.tv_nsec - mStart.tv_nsec;

			return mNs ? double( dSec*1000000000.0 + dNSec ) :  double( dSec * 1000.0 + dNSec / 1000000.0 );
#endif
		}
};

} // namespace

#endif // STOPWATCH_7DE304E10B734077A36B86CA99E5B0F2_HPP_
