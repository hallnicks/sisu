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
#include "ttycolor.hpp"
#include <time.h>

using std::cout;
using std::endl;
using sisu::ccolor;
using sisu::eTTYCBlack;
using sisu::eTTYCYellow;
using sisu::eModBold;

int main( void )
{
	srand( time( NULL ) );

	// TODO - get and save original color. For now, screw you!
	ccolor const d( eTTYCBlack, eTTYCYellow, eModBold );
	ccolor const b( eTTYCBlack, eTTYCYellow, eModBold );

	cout << d;

	static const int ES = 7;


	std::cout << '`' << std::endl;
	unsigned i = 0;

	for ( ; i < ES; ++i)
		{ cout <<
			endl << "                          ... ☁ ▅▒░☼‿☼░▒▅ ☁  "
				<< endl; }

	int r = ::sisu::Execute();

	cout << b;

	for ( i = 0; i < ES; ++i )
		{ cout <<
			endl <<      "                          ... ☁ ▅▒░OoO░▒▅ ☁  "
				<< endl; }

	std::cout << std::endl << "                              \\m/ >_< \\m/ " << std::endl;

	return r;
}
