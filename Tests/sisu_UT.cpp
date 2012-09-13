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
#include "test.hpp"
#include "signal.hpp"

#include <iostream>

namespace {

class SisuUT : public context
{
	int mPrivateMember;

	protected:
		int mProtectedMember;

	public:
		SisuUT( ) : context( ), mPrivateMember(3), mProtectedMember(-1) { }

		void Up()
		{
			std::cout << __PRETTY_FUNCTION__ <<  " : Private member is " << mPrivateMember << std::endl;
			mPrivateMember = 972;
		}

		void Down()
		{
			std::cout << __PRETTY_FUNCTION__ <<  " : Private member is " << mPrivateMember << std::endl;
		}
};

} // namespace

TEST(SisuUT, HelloWorld1)
{
	std::cout << __FUNCTION__ << " : Hello, World! Protected member is " << ++mProtectedMember << std::endl;
	mProtectedMember = 35;
}


TEST(SisuUT, HelloWorld2)
{
	std::cout << __FUNCTION__ << " : Hello, World! Protected member is " << ++mProtectedMember << std::endl;
}

TEST(SisuUT, Sisu_MUSTEQ)
{
	std::cout << __FUNCTION__ << " : Hello, World! Protected member is " << ++mProtectedMember << std::endl;
	MUSTEQ(1,1);
}

TEST(SisuUT, Sisu_MUSTNEQ)
{
	std::cout << __FUNCTION__ << std::endl;
	MUSTNEQ(1,0);
}

TEST(SisuUT, Sisu_Commas)
{
	int x = 1, y = 2, z = 3;
	std::cout << __FUNCTION__ << x << y << z << std::endl;
}
