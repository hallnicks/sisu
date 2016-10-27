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
#include <SDL.h>

#include <android/log.h>

#include "SDLColorSwitcher.hpp"
#include "HelloTriangle.hpp"
#include "HelloTexture.hpp"

using namespace sisu;

int main( int xArgc, char * xArgv[] )
{
	const char * buff[] = { " Measles make you bumpy\n"
			      , " And mumps'll make you lumpy\n"
			      , " And chicken pox'll make you jump and twitch\n"
			      , " A common cold'll fool ya\n"
			      , " And whooping cough'll cool ya\n"
			      , "But poison ivy, Lord'll make you itch!!\n"

			      , "You're gonna need an ocean of calamine lotion\n"
			      , "You'll be scratchin' like a hound\n"
			      , "The minute you start to mess around\n" };

	for ( int32_t ii = 0; ii < sizeof(buff)/sizeof(const char*); ++ii)
	{
                 __android_log_print( ANDROID_LOG_VERBOSE, "sisu", "%s\n", buff[ ii ] );
	}

	/*
        SDLColorSwitcher test;

        test.initialize( { 3, 0, 1, false, false } );

        test.run();
	*/

	/*
        HelloTriangle * window = new HelloTriangle;
        window->initialize( { 3, 0, 1, false, false } );
        window->run( );
        delete window;
	*/

        HelloTexture * window = new HelloTexture;
        window->initialize( { 3, 0, 1, false, false } );
        window->run( );
        delete window;

	return 0;
}
