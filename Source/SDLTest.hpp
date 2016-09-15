#ifndef SDL_TEST_54E16EE8084C43819D8DBBA8CD37156F_HPP_
#define SDL_TEST_54E16EE8084C43819D8DBBA8CD37156F_HPP_

// This file is part of sisu.

// sisu is free software: you can redistribute it and/or modify // it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or // (at your option) any later version.

// sisu is distributed in the hope that it will be useful, // but WITHOUT ANY WARRANTY; without even the implied warranty of //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the // GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace sisu {
        class SDLTestWindow
        {
                void _setOpenGLAttributes( );

                static void _checkSDLError( );

		protected:
	                SDL_Window * mMainWindow;

	                SDL_GLContext mMainContext;

                public:
                        SDLTestWindow( );
                        ~SDLTestWindow( );

                        virutal void initialize( int const xSwapInterval );

        	        virtual void run() = 0;
	}; // class
}

#endif // SDL_TEST_54E16EE8084C43819D8DBBA8CD37156F_HPP_
