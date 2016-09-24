// This file is part of sisu.

// sisu is free software: you can redistribute it and/or modify // it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or // (at your option) any later version.

// sisu is distributed in the hope that it will be useful, // but WITHOUT ANY WARRANTY; without even the implied warranty of //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the // GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.
#include "test.hpp"

#include "threadgears.hpp"
#include "stacktrace.hpp"
#include "ttyc.hpp"
#include "ttycolor.hpp"

using namespace sisu;

namespace
{

class tsq_UT : public context
{
	public:
		tsq_UT( ) : context( ) { }
		void Up() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
		void Down() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};


template< typename XType >
class LocklessRingBuffer
{

};


} // namespace

TEST(tsq_UT, ThreadSafeQueueTrivial)
{

}
