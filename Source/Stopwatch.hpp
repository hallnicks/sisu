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
	enum eStopwatchMode
	{
		eStopwatchMode_Ns,
		eStopwatchMode_Ms,
		eStopwatchMode_S,
	} mStopwatchMode;
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
	void _start( eStopwatchMode const xMode )
	{
		clock_gettime( CLOCK_REALTIME, &mStart );

		mStopwatchMode = xMode;
	}
#endif

	public:
		Stopwatch( )
#ifdef WIN32
			: mFrequency( 0.0 )
			, mCounterStart( 0 )
#else
			: mStart( { 0, 0 } )
			, mStopwatchMode( eStopwatchMode_S )
#endif
		{
			;
		}

#ifdef WIN32
		void startS( )  { _start( 1.0 ); 	  }

		void startMs( ) { _start( 1000.0 ); 	  }

		void startNs( ) { _start( 1000000000.0 ); }
#else
		void startS( )  { _start( eStopwatchMode_S ); 	}

		void startMs( ) { _start( eStopwatchMode_Ms ); 	}

		void startNs( ) { _start( eStopwatchMode_Ns  ); }
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

			double ret = 0.0;

			switch ( mStopwatchMode )
			{
				case eStopwatchMode_Ns:
					ret = double( dSec*1000000000.0 + dNSec ); break;
				case eStopwatchMode_Ms:
					ret = double( dSec * 1000.0 + dNSec / 1000000.0 ); break;
				case eStopwatchMode_S:
					ret = double( dSec  + dNSec / 1000000000.0 ); break;
				default:
					break;
			}

			return ret;
#endif
		}
};

} // namespace

#endif // STOPWATCH_7DE304E10B734077A36B86CA99E5B0F2_HPP_
