#ifndef BEEP_50075EE7_DFA8_4DFB_A709_2E6ACF4F2AF5_HPP_
#define BEEP_50075EE7_DFA8_4DFB_A709_2E6ACF4F2AF5_HPP_

#ifdef __linux__

#include <linux/kd.h>
#include <sys/ioctl.h>
#include <unistd.h>

#else

#include <Windows.h>

#endif

namespace sisu {

enum eFreq
{
	eFreqCA = 523,
	eFreqD  = 587,
	eFreqE  = 659,
	eFreqF  = 698,
	eFreqG  = 784,
	eFreqA  = 880,
	eFreqB  = 998,
	eFreqCO = 1046,
};

class noise
{
	public:
		static void note( eFreq xFreq );
		static void rest( );
		static void octave( );
};

} // namespace sisu
#endif // BEEP_50075EE7_DFA8_4DFB_A709_2E6ACF4F2AF5_HPP_
