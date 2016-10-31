#ifdef OPENGLES
#ifndef HELLO_TEXTURE_D8C137657A3A4989AE28F3640F7AA4C3_HPP_
#define HELLO_TEXTURE_D8C137657A3A4989AE28F3640F7AA4C3_HPP_

#include "SDLTest.hpp"
#include "SDLShader.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"
//#include "PNGImage.hpp"
#include "DevILImage.hpp"
#include "sisumath.hpp"
#include "ioassist.hpp"

#include <vector>

#include <GLES3/gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "AndroidLogWrapper.hpp"

namespace sisu {

class HelloTexture : public SDLTestWindow
{
	Texture2D mTexture;
	DevILImage mDevILImage;
	SDLShader mShader;
	int32_t mScaleW, mScaleH;
	bool mReverseScale;

	GLuint mQuadVBO, mQuadVAO;

        void _drawTexture( Texture2D & xTexture
                         , glm::vec2 const xPosition
                         , glm::vec2 const xSize
                         , GLfloat xRotate
                         , glm::vec3 const xColor )
        {
                mShader( [ & ] ( )
                {
                        glm::mat4 model;

                        model = glm::translate( model, glm::vec3( xPosition, 0.0f ) );

                        model = glm::translate( model, glm::vec3( 0.5f * xSize.x, 0.5f * xSize.y, 0.0f ) );

                        model = glm::rotate(    model, xRotate, glm::vec3( 0.0f, 0.0f, 1.0f ) );

                        model = glm::translate( model, glm::vec3( -0.5f * xSize.x, -0.5f * xSize.y, 0.0f ) );

                        model = glm::scale(     model, glm::vec3( xSize, 1.0f ) );

                        mShader.getUniforms( ).setUniformMatrix4fv( "model", model );

                        mShader.getUniforms( ).setUniformVector3f( "spriteColor", xColor );

                        glActiveTexture( GL_TEXTURE0 );

                        xTexture( [ & ]( )
                        {
                                glBindVertexArray( mQuadVAO );

                                glDrawArrays( GL_TRIANGLES, 0, 6 );

                                glBindVertexArray( 0 );
                        } );
                } );
        }


	protected:
		virtual void render( )
		{
			// Set the viewport
			glViewport( 0, 0, mW, mH );

			// Clear the color buffer
			glClear ( GL_COLOR_BUFFER_BIT );

			static GLfloat rotate = 0.0f;

			if ( ( rotate += 0.10 ) >= 360.0f )
			{
				rotate = 0.0f;
			}

			static uint32_t const sStep = 25;

			if ( !mReverseScale )
			{
				mScaleW -= sStep;
				mScaleH -= sStep;
			}
			else
			{
			 	mScaleW += sStep;
				mScaleH += sStep;
			}

			if ( mScaleW <= 0  || mScaleH <= 0  )
			{
				mReverseScale = true;
				mScaleW = mScaleH = 0;
			}
			else if ( mScaleW >= mTexture.getWidth( ) || mScaleH >= mTexture.getHeight( ) )
			{
				mReverseScale = false;
			}

			_drawTexture( mTexture
				    , glm::vec2( 0, 0 )
				    , glm::vec2( mScaleW, mScaleH )
				    , rotate
				    , glm::vec3( dRand( 0.0f, 1.0f )
					       , dRand( 0.0f, 1.0f )
                                               , dRand( 0.0f, 1.0f ) ) );
		}

	public:
		HelloTexture( )
			: SDLTestWindow( )
			, mShader( ShaderSourcePair("#version 300 es                            			  \n"
      						    "layout(location = 0) in vec4 vertex;       			  \n"
						    "out vec2 TexCoords;                        			  \n"
						    "uniform mat4 model;                        			  \n"
						    "uniform mat4 projection;                   			  \n"
						    "void main()                               			  \n"
						    "{                                          			  \n"
						    "   TexCoords = vertex.zw;                  			  \n"
						    "   gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0); \n"
						    "}                                          			  \n"
						   ,"#version 300 es                                                \n"
						    "precision mediump float;				       \n"
   	  		  			    "in vec2 TexCoords;				               \n"
   	  		  			    "out vec4 color;				               \n"
						    "uniform sampler2D image; 				       \n"
						    "uniform vec3 spriteColor;                                     \n"
						    "void main()                                                   \n"
						    "{                                                             \n"
						    "  color = vec4(spriteColor, 1.0) * texture(image, TexCoords); \n"
						    "}                                                             \n") )
			, mTexture( )
			, mDevILImage( "resources/testinput/testinput01.png" )
			, mQuadVAO( 0 )
			, mQuadVBO( 0 )
			, mScaleW( 0 )
			, mScaleH( 0 )
			, mReverseScale( false )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			mShader.initialize( );


                        glm::mat4 projection = glm::ortho(  0.0f
                                                         , static_cast<GLfloat>( mW )
                                                         , static_cast<GLfloat>( mH )
                                                         ,  0.0f
                                                         , -1.0f
                                                         ,  1.0f );

                        mShader( [ & ]( )
                        {
                                mShader.getUniforms( ).setUniform1i( "image", 0 );
                                mShader.getUniforms( ).setUniformMatrix4fv( "projection", projection );
                        });

                        static GLfloat const vertices[] = {
                                                            // Pos      // Tex
                                                            0.0f, 1.0f, 0.0f, 1.0f,
                                                            1.0f, 0.0f, 1.0f, 0.0f,
                                                            0.0f, 0.0f, 0.0f, 0.0f,

                                                            0.0f, 1.0f, 0.0f, 1.0f,
                                                            1.0f, 1.0f, 1.0f, 1.0f,
                                                            1.0f, 0.0f, 1.0f, 0.0f
                                                          };

                        glGenVertexArrays( 1, &mQuadVAO );

                        glGenBuffers( 1, &mQuadVBO );

                        glBindBuffer( GL_ARRAY_BUFFER, mQuadVBO );

                        glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

                        glBindVertexArray( mQuadVAO );

                        glEnableVertexAttribArray( 0 );

                        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), ( GLvoid* )0 );

                        glBindBuffer( GL_ARRAY_BUFFER, 0 );

                        glBindVertexArray( 0 );

			mTexture.initialize( ( mScaleW = mDevILImage.getWidth( )  )
					   , ( mScaleH = mDevILImage.getHeight( ) )
					   , mDevILImage.toGLTextureBuffer( ) );
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

				if ( ( accum += t.stop( ) ) >= 30000.0 )
				{
					break;
				}

			} while ( 1 );

			_hide( );
		}
};

} // namespace sisu

#endif // HELLO_TEXTURE_D8C137657A3A4989AE28F3640F7AA4C3_HPP_
#endif // OPENGLES
