#if 0
#include "beep.hpp"


#include <iostream>
#include <linux/kd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace sisu
{

namespace sound
{
	void noise::note( eFreq xFreq, unsigned int xSleepUS )
	{
		if ( xFreq != 0 )
		{
			ioctl( STDOUT_FILENO, KIOCSOUND, 1193180 / static_cast<int32_t>( xFreq ) );

			usleep( xSleepUS );
		}
	}

	void noise::rest( )
	{
		ioctl( STDOUT_FILENO, KIOCSOUND, 0 );
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

	Beep::Beep( ) { }

	Beep::~Beep( ) { }

} // sound

} // namespace sisu

#endif
