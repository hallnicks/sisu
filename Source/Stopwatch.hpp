#ifndef STOPWATCH_7DE304E10B734077A36B86CA99E5B0F2_HPP_
#define STOPWATCH_7DE304E10B734077A36B86CA99E5B0F2_HPP_

#include <windows.h>
#include <iostream>

namespace sisu {

class Stopwatch
{
	double mFrequency;

	__int64 mCounterStart;

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

	public:
		Stopwatch( )
			: mFrequency( 0.0 )
			, mCounterStart( 0 )
		{
			;
		}

		void startMs( ) { _start( 1000.0 ); 	  }

		void startNs( ) { _start( 1000000000.0 ); }

		double stop( )
		{
			LARGE_INTEGER li;

			QueryPerformanceCounter( &li );

			return double( li.QuadPart - mCounterStart ) / mFrequency;
		}
};

} // namespace

#endif // STOPWATCH_7DE304E10B734077A36B86CA99E5B0F2_HPP_
