#ifndef SDL_TEST_54E16EE8084C43819D8DBBA8CD37156F_HPP_
#define SDL_TEST_54E16EE8084C43819D8DBBA8CD37156F_HPP_

// This file is part of sisu.

// sisu is free software: you can redistribute it and/or modify // it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or // (at your option) any later version.

// sisu is distributed in the hope that it will be useful, // but WITHOUT ANY WARRANTY; without even the implied warranty of //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the // GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace sisu {

        inline GLfloat _randomGLfloat( )
        {
		return static_cast <GLfloat> ( rand( ) ) / static_cast <GLfloat> ( RAND_MAX );
        }

	void _printGLProgramLog( GLuint const xProgram );
	void _printGLShaderLog( GLuint const xProgram );

	struct OpenGLAttributes
	{
		uint8_t  const mMajorGLVersion;
		uint8_t  const mMinorGLVersion;
		uint32_t const mSwapInterval;
		bool 	 const mDoubleBufferingEnabled;
		bool 	 const mMultisampleBufferingEnabled;
	};

        class SDLTestWindow
        {
                void _setOpenGLAttributes( OpenGLAttributes const & xAttributes ); 

                static void _checkSDLError( );

		protected:
	                SDL_Window * mMainWindow;

	                SDL_GLContext mMainContext;
	
			void _stealContext( );

                public:
                        SDLTestWindow( );
                        ~SDLTestWindow( );

                        virtual void initialize( OpenGLAttributes const & xAttributes );

        	        virtual void run() = 0;
	}; // class
}

#endif // SDL_TEST_54E16EE8084C43819D8DBBA8CD37156F_HPP_
