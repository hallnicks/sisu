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
#include "threadgears.hpp"

#include <iostream>

using namespace threadgears;

namespace {

class tgUT : public context
{
	public:
		tgUT( ) : context( ) { }
		void Up() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
		void Down() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

} // namespace

TEST(tgUT, ThreadTrivial){
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	class TrivialThread
	{
		public:
			TrivialThread( ) { }
			void DoSomething( int ) { }
	};

	TrivialThread m;
	/*
	thread<void, TrivialThread, int> t( m, &TrivialThread::DoSomething );
	*/
}

TEST(tgUT, ThreadDerived){
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	/*
	class TrivialThread : public thread<int, TrivialThread, void>
	{
	public:
		TrivialThread( ) : thread<int, TrivialThread, void>( this, DoSomething ) { }

		void DoSomething( );
	} m;

	m( );

	m.join( );
	*/
}
