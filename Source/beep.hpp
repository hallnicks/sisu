#ifndef BEEP_50075EE7_DFA8_4DFB_A709_2E6ACF4F2AF5_HPP_
#define BEEP_50075EE7_DFA8_4DFB_A709_2E6ACF4F2AF5_HPP_

#include <linux/kd.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace sisu
{

namespace sound
{
	enum eFreq
	{
		eFreqCA = 523,
		eFreqD = 587,
		eFreqE = 659,
		eFreqF = 698,
		eFreqG = 784,
		eFreqA = 880,
		eFreqB = 998,
		eFreqCO = 1046,
	};

	static inline void note( eFreq xFreq, unsigned int sleep_us = 500000 )
	{
		if ( xFreq != 0 )
		{
			ioctl(STDOUT_FILENO, KIOCSOUND, 1193180/static_cast<int32_t>(xFreq));
			usleep(sleep_us);
		}
	}

	static inline void rest( ) { ioctl(STDOUT_FILENO, KIOCSOUND, 0); }

	static inline void octave( )
	{
		static eFreq octave[] = { eFreqCA
					 , eFreqD
					 , eFreqE
					 , eFreqF
					 , eFreqG
					 , eFreqA
					 , eFreqB
					 , eFreqCO
					};

		for ( unsigned i = 0; i < sizeof(octave); ++i )
		{
			note( octave[i] );
			rest( );
		}
	}

	class Beep
	{
		public:
			Beep( ) { }
			~Beep( ) { }
	};

} // sound

} // namespace sisu
#endif // BEEP_50075EE7_DFA8_4DFB_A709_2E6ACF4F2AF5_HPP_
