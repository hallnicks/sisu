#include "test.hpp"
#include "memblock.hpp"
#include "ttyc.hpp"
#include "ioassist.hpp"

using namespace sisu;

namespace
{
	class libavcodec_UT : public context
	{
		public:
			libavcodec_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};
};

// Ensures writes are stateless ( at least for one iteration ) ..
TEST(libavcodec_UT, writeAVCODECFile)
{

}
