#ifdef OPENGLES_HELLOTEXTURE_UT
#ifdef OPENGLES
#include "test.hpp"

#include "SDLTestShaderWindow.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"

#include <iostream>

using namespace sisu;

namespace
{

class HelloTexture_UT : public context
{
	public:
		HelloTexture_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};


class HelloTexture : public SDLTestShaderWindow
{
	Texture2D mTexture;
	GLint mSamplerLoc;

	protected:
		virtual void render( )
		{
			static GLfloat const vVertices[] = { -0.5f,  0.5f, 0.0f,  // Position 0
		                            		      0.0f,  0.0f,        // TexCoord 0
	                        			     -0.5f, -0.5f, 0.0f,  // Position 1
	  		                                      0.0f,  1.0f,        // TexCoord 1
                     		  	         	      0.5f, -0.5f, 0.0f,  // Position 2
		                                              1.0f,  1.0f,        // TexCoord 2
		                                              0.5f,  0.5f, 0.0f,  // Position 3
                     			                      1.0f,  0.0f         // TexCoord 3
		                                           };

			static GLushort const indices[] = { 0, 1, 2, 0, 2, 3 };

			// Set the viewport
			glViewport( 0, 0, mW, mH );

			// Clear the color buffer
			glClear ( GL_COLOR_BUFFER_BIT );

			mShader([&](){
				// Load the vertex position
				glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), vVertices );

   				// Load the texture coordinate
				glVertexAttribPointer ( 1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof ( GLfloat ), &vVertices[3] );

				glEnableVertexAttribArray ( 0 );
				glEnableVertexAttribArray ( 1 );

				mTexture([&](){
					// Set the sampler texture unit to 0
   					glUniform1i ( mSamplerLoc, 0 );
   					glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
				});
			});
		}

	public:
		HelloTexture( )
			: SDLTestShaderWindow( ShaderSourcePair("#version 300 es                            \n"
      								"layout(location = 0) in vec4 a_position;   \n"
							        "layout(location = 1) in vec2 a_texCoord;   \n"
							        "out vec2 v_texCoord;                       \n"
							        "void main()                                \n"
							        "{                                          \n"
							        "   gl_Position = a_position;               \n"
							        "   v_texCoord = a_texCoord;                \n"
							        "}                                          \n"
							      ,"#version 300 es                                     \n"
   	  		  			               "precision mediump float;                            \n"
							       "in vec2 v_texCoord;                                 \n"
							       "layout(location = 0) out vec4 outColor;             \n"
							       "uniform sampler2D s_texture;                        \n"
							       "void main()                                         \n"
							       "{                                                   \n"
							       "  outColor = texture( s_texture, v_texCoord );      \n"
							       "}                                                   \n") )
			, mTexture( )
			, mSamplerLoc( -1 )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestShaderWindow::initialize( xAttributes );

			// Using stack data creates a dangling pointer in our impl
			GLubyte pixels[4 * 3] =
			{
			      255,   0,   0, // Red
			      0, 255,     0, // Green
			      0,   0,   255, // Blue
			      255, 255,   0  // Yellow
			};

			mSamplerLoc = mShader.getUniforms( )["s_texture"];

			mTexture.initialize( 2, 2, pixels );
		}

		virtual void run( )
		{
			Stopwatch t;

			double accum = 0.0;

			do
			{
				t.startMs( );

				render( );
				_checkForGLError( );
				SDL_PumpEvents( );
				SDL_GL_SwapWindow( mMainWindow );

				if ( ( accum += t.stop( ) ) >= 3000.0 )
				{
					break;
				}

			} while ( 1 );

			_hide( );
		}
};

} // namespace

TEST(HelloTexture_UT, HelloTexture)
{
	HelloTexture * window = new HelloTexture;
	window->initialize( { 3, 0, 1, false, false } );
	window->run( );
	delete window;
}
#endif // OPENGLES
#endif // OPENGLES_HELLOTRIANGLE_UT
