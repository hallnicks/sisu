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
#endif

	void _start( double const xDenominator )
	{
#ifdef WIN32
		LARGE_INTEGER li;

		if ( !QueryPerformanceFrequency( &li ) )
		{
			std::cerr << "QueryPerformanceFrequency( .. ) failed." << std::endl;
			exit( -1 );
		}

		mFrequency = double( li.QuadPart ) / xDenominator;

		QueryPerformanceCounter( &li );

		mCounterStart = li.QuadPart;
#else
		std::cerr << __PRETTY_FUNCTION__ << " is not implemented on this platform." << std::endl;
		exit( -1 );
#endif
	}

	public:
		Stopwatch( )
#ifdef WIN32
			: mFrequency( 0.0 )
			, mCounterStart( 0 )
#endif
		{
			;
		}

		void startMs( ) { _start( 1000.0 ); 	  }

		void startNs( ) { _start( 1000000000.0 ); }

		double stop( )
		{
#ifdef WIN32
			LARGE_INTEGER li;

			QueryPerformanceCounter( &li );

			return double( li.QuadPart - mCounterStart ) / mFrequency;
#else
		std::cerr << __PRETTY_FUNCTION__ << " is not implemented on this platform." << std::endl;
		exit( -1 );
		return 0.0;
#endif
		}
};

} // namespace

#endif // STOPWATCH_7DE304E10B734077A36B86CA99E5B0F2_HPP_
