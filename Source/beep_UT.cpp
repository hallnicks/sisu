#include "test.hpp"
#include "beep.hpp"

namespace
{

class Beep_UT : public context
{
	public:
		Beep_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(Beep_UT, Octave)
{
	//sisu::sound::octave( );
}

