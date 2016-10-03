#include "test.hpp"

#include "beep.hpp"

using namespace sisu;

namespace {

class beep_UT : public context
{
	public:
		beep_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(beep_UT, testBeep)
{
	noise::octave( );
}
