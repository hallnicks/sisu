#include "test.hpp"
#include "threadgears.hpp"
#include "Stopwatch.hpp"

using namespace sisu;

namespace {

class stopwatch_UT : public context
{
	public:
		stopwatch_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

} // namespace

TEST(stopwatch_UT, measure1000ms)
{
	Stopwatch sw;

	sw.startMs( );

	sleep::ms( 50 );

	double const elapsed = sw.stop( );

	std::cerr << "elapsed ms = " << elapsed << std::endl;
}

TEST(stopwatch_UT, measure1000msInNS)
{
	Stopwatch sw;

	sw.startNs( );

	sleep::ms( 50 );

	double const elapsed = sw.stop( );

	std::cerr << "elapsed ns = " << elapsed << std::endl;
}
