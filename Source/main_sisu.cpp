#include "sisu.hpp"

#include <iostream>

namespace
{
	class SisuTDD : public ::sisu::context
	{
		private:
			int mPrivateMember;

		protected:
			int mProtectedMember;

		public:
			SisuTDD( ) : ::sisu::context( ), mPrivateMember(3), mProtectedMember(5) { }
			void Up()
			{
				std::cout << __FUNCTION__ <<  " : Private member is " << mPrivateMember << std::endl;
				mPrivateMember = 972;
			}
			void Down()
			{
				std::cout << __FUNCTION__ <<  " : Private member is " << mPrivateMember << std::endl;
			}
	};

} // namespace

DECL_UT(SisuTDD, HelloWorld, {
	std::cout << __FUNCTION__ << " : Hello, World! Protected member is " << mProtectedMember << std::endl;
});

int main(void)
{
	sisu::ExecuteTests( );
	return 0;
}

