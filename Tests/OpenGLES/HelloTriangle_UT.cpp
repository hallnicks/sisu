#ifdef OPENGLES_HELLOTRIANGLE_UT
#ifdef OPENGLES
#include "test.hpp"

#include "SDLTestShaderWindow.hpp"
#include "Stopwatch.hpp"

#include <iostream>

using namespace sisu;

namespace
{

class HelloTriangle_UT : public context
{
	public:
		HelloTriangle_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

class HelloTriangle : public SDLTestShaderWindow
{
	protected:
		virtual void render( )
		{
			static GLfloat const vVertices[] = {  0.0f,  0.5f, 0.0f,
			       	                             -0.5f, -0.5f, 0.0f,
			                                      0.5f, -0.5f, 0.0f
			                                   };
			glViewport( 0, 0, mW, mH );

			glClear( GL_COLOR_BUFFER_BIT );

			mShader([&](){
				glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
				glEnableVertexAttribArray( 0 );
				glDrawArrays( GL_TRIANGLES, 0, 3 );
			});
		}

	public:
		HelloTriangle( )
			: SDLTestShaderWindow( ShaderSourcePair("#version 300 es                         \n"
							        "layout(location = 0) in vec4 vPosition; \n"
							        "void main()                             \n"
							        "{                                       \n"
							        "   gl_Position = vPosition;             \n"
							        "}                                       \n"
							      , "#version 300 es                              \n"
							        "precision mediump float;                     \n"
							        "out vec4 fragColor;                          \n"
							        "void main()                                  \n"
							        "{                                            \n"
							        "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
							        "}                                            \n") )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestShaderWindow::initialize( xAttributes );
			glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		}

		virtual void run( )
		{
			Stopwatch t;

			double accum = 0.0;

			do
			{
				t.startMs( );

				render( );

				SDL_GL_SwapWindow( mMainWindow );

				_checkForGLError( );

				SDL_PumpEvents( );

				if ( ( accum += t.stop( ) ) >= 3000.0 )
				{
					break;
				}

			} while ( 1 );

			_hide( );
		}
};

} // namespace

TEST(HelloTriangle_UT, HelloTriangle)
{
	HelloTriangle * window = new HelloTriangle;
	window->initialize( { 3, 0, 1, false, false } );
	window->run( );
	delete window;
}
#endif // OPENGLES
#endif // OPENGLES_HELLOTRIANGLE_UT
