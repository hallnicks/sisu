#if 0
#include "test.hpp"
#include "signal.hpp"

#include <iostream>
#include <cstring>
#include <clocale>

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>
#include <stdint.h>

namespace
{

class Signal_UT : public context
{
	public:
		Signal_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(Signal_UT, RaiseSignals)
{
	std::cout << "!!!! STUB:" << __PRETTY_FUNCTION__ << std::endl;
}
#endif