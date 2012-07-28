//    This file is part of sisu.

//    sisu is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    sisu is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.
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
			SisuTDD( ) : ::sisu::context( ), mPrivateMember(3), mProtectedMember(-1) { }
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
	std::cout << __FUNCTION__ << " : Hello, World! Protected member is " << ++mProtectedMember << std::endl;
	mProtectedMember = 35;
});

DECL_UT(SisuTDD, HelloWorld2, {
	std::cout << __FUNCTION__ << " : Hello, World! Protected member is " << ++mProtectedMember << std::endl;
});

int main(void)
{
	sisu::ExecuteTests( );
	return 0;
}

