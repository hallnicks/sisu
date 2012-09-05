#include "test.hpp"
#include "ttystring.hpp"

namespace
{
	class TTYString_UT : public context
	{
		public:
			TTYString_UT( ) : context( ) { }
			~TTYString_UT( ) { }
			void Up( ) { }
			void Down( ) { }
	};

} // namespace

TEST(TTYString_UT, BuildBasicString)
{

}
