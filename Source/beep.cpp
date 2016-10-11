#include "beep.hpp"

#include <iostream>

#ifdef __linux__
#include <linux/kd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace sisu
{

void noise::note( eFreq xFreq )
{
	if ( xFreq != 0 )
	{
#ifdef __linux__
		ioctl( STDOUT_FILENO, KIOCSOUND, 1193180 / static_cast<int32_t>( xFreq ) );

		usleep( 50000 ); // TODO: Replace with a reasonable value for linux
#elif defined WIN32
		if ( !Beep( (DWORD)xFreq, 500 ) )
		{
			std::cerr << "Beep( .. ) failed." << std::endl;
			exit( -1 );
		}
#endif
	}
}

void noise::rest( )
{
#ifdef __linux__
	ioctl( STDOUT_FILENO, KIOCSOUND, 0 );
#endif
}

void noise::octave( )
{
	static eFreq octave[8] = { eFreqCA
				 , eFreqD
				 , eFreqE
				 , eFreqF
				 , eFreqG
				 , eFreqA
				 , eFreqB
				 , eFreqCO };

	for ( unsigned i = 0; i < 8; ++i )
	{
		note( octave[i] );
		rest( );
	}
}

} // namespace sisu
