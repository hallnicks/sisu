#ifdef OPENGLES
#include "test.hpp"

#include "SDLShaderTest.hpp"

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

			mShader([&]() {
				glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
				glEnableVertexAttribArray( 0 );
				glDrawArrays( GL_TRIANGLES, 0, 3 );
			});
		}

	public:
		HelloTriangle( )
			: SDLTestShaderWindow( ShaderSourcePair("#version 300 es                          \n"
							        "layout(location = 0) in vec4 vPosition;  \n"
							        "void main()                              \n"
							        "{                                        \n"
							        "   gl_Position = vPosition;              \n"
							        "}                                        \n"
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
			do
			{
				render( );
				_checkForGLError( );
				SDL_PumpEvents( );
				SDL_GL_SwapWindow( mMainWindow );

			} while ( !mQuit.isSet( ) );

			_hide( );
		}
};

} // namespace

TEST(HelloTriangle_UT, HelloTriangle)
{
	HelloTriangle window;

	window.initialize( { 3, 0, 0, false, false } );

	window.run( );
}
#endif // OPENGLES
