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
TEST(stopwatch_UT, resetInLoop)
{
	Stopwatch sw;

	double accum = 0.0;

	uint8_t iterations = 0;

	for ( ;; )
	{
		sw.startMs( );

		sleep::ms( 300 );

		if ( ( accum += sw.stop( ) ) > 3000.0 )
		{
			std::cout << "reset accum because 3 seconds exceeded." << std::endl;
			accum = 0.0;
			if ( ++iterations > 5 )
			{
				std::cout << "Five iterations have succeeded." << std::endl;
				break;
			}
		}
	}
}

TEST(stopwatch_UT, reUseStopwatchWorks)
{
	Stopwatch sw;

	double accum = 0.0;

	for ( ;; )
	{
		sw.startMs( );

		sleep::ms( 300 );

		accum += sw.stop( );

		std::cout << "elapsed ms = " << accum << std::endl;

		if ( accum > 3000.0 )
		{
			break;
		}
	}

	std::cout << "elapsed ms = " << accum << std::endl;
}
