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
#include <SDL2/SDL.h>
#include <iostream>

int main( int xArgc, char * xArgv[] )
{
	const char * buff[] = { " Measles make you bumpy"
			      , " And mumps'll make you lumpy"
			      , " And chicken pox'll make you jump and twitch"
			      , " A common cold'll fool ya"
			      , " And whooping cough'll cool ya"
			      , "But poison ivy, Lord'll make you itch!!\n"

			      , "You're gonna need an ocean of calamine lotion"
			      , "You'll be scratchin' like a hound"
			      , "The minute you start to mess around" };

	for ( int32_t ii = 0; ii < sizeof(buff)/sizeof(const char*); ++ii)
	{
		std::cout << buff[ ii ] << std::endl;
	}

	char buffer[4096];

	char * out = NULL;

	if ( ( out = fgets( buffer, 4096, stdin ) ) == NULL )
	{
		return -1;
	}

	puts( out );

	return 0;
}
