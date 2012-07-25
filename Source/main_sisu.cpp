#include "sisu.hpp"

#include <iostream>

namespace
{
	class SisuTDD : public ::sisu::context
	{
		public:
			SisuTDD( ) : ::sisu::context( ) { }
			void Up() { }
			void Down() { }
	};

} // namespace

DECL_UT(SisuTDD, HelloWorld, {

	std::cout << "Hello, World!" << std::endl;
});

int main(void)
{
	sisu::ExecuteTests( );
	return 0;
}
