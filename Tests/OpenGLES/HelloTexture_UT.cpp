#ifdef OPENGLES_HELLOTEXTURE_UT
#ifdef OPENGLES
#include "test.hpp"

#include "SDLTestShaderWindow.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"
#include "PNGImage.hpp"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>

// begin GLESext header
#include <dlfcn.h>

//these ugly typenames are defined in GLES2/gl2ext.h
#ifndef DLSYM_LIBGLES
 #ifdef WIN32
  #define DLSYM_LIBGLES "libGLESv2.dll"
 #else
  #define DLSYM_LIBGLES "libGLESv2.so"
 #endif // WIN32
#endif // DLSYM_LIBGLES

namespace sisu {

PFNGLBINDVERTEXARRAYOESPROC    glBindVertexArrayOES;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
PFNGLGENVERTEXARRAYSOESPROC    glGenVertexArraysOES;
PFNGLISVERTEXARRAYOESPROC      glIsVertexArrayOES;

#ifndef GLBINDVERTEXARRAYOES
 #define GLBINDVERTEXARRAYOES    "glBindVertexArray"
#endif

#ifndef GLDELETEVERTEXARRAYSOES
 #define GLDELETEVERTEXARRAYSOES "glDeleteVertexArrays"
#endif

#ifndef GLGENVERTEXARRAYSOES
 #define GLGENVERTEXARRAYSOES    "glGenVertexArrays"
#endif

#ifndef GLISVERTEXARRAYOES
 #define GLISVERTEXARRAYOES      "glIsVertexArray"
#endif

static class OpenGLESExtensions
{

	void * mHandle;

	public:
		OpenGLESExtensions( )
			: mHandle( NULL )
		{
			if ( ( mHandle = dlopen( DLSYM_LIBGLES, RTLD_LAZY ) ) == NULL )
			{
				std::cerr << "Failed to load " DLSYM_LIBGLES ". Cannot load OpenGL ES 3.0 extensions!" << std::endl;
				exit( -1 );
			}

#define LOADSYM(xSym, xType, xName)\
	                if ( ( xSym = (xType) dlsym( mHandle, xName ) ) == NULL )\
        	        {\
	                        std::cerr << "Could not load symbol " << xName << " from " DLSYM_LIBGLES << std::endl;\
	                        exit( -1 );\
	                }

			LOADSYM( glBindVertexArrayOES    , PFNGLBINDVERTEXARRAYOESPROC   , GLBINDVERTEXARRAYOES    );
			LOADSYM( glDeleteVertexArraysOES , PFNGLDELETEVERTEXARRAYSOESPROC, GLDELETEVERTEXARRAYSOES );
			LOADSYM( glGenVertexArraysOES    , PFNGLGENVERTEXARRAYSOESPROC   , GLGENVERTEXARRAYSOES    );
			LOADSYM( glIsVertexArrayOES      , PFNGLISVERTEXARRAYOESPROC     , GLISVERTEXARRAYOES      );
#undef LOADSYM

		}

		~OpenGLESExtensions( )
		{
			if ( mHandle != NULL )
			{
				dlclose( mHandle );
			}
		}

} sOpenGLESExtensions;

} // namespace sisu;


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
	PNGImage mPNGImage;

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
                                glBindVertexArrayOES( mQuadVAO );

                                glDrawArrays( GL_TRIANGLES, 0, 6 );

                                glBindVertexArrayOES( 0 );
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

			_drawTexture( mTexture
				    , glm::vec2( 0, 0 )
				    , glm::vec2( mTexture.getWidth( ), mTexture.getHeight( ) )
				    , rotate
				    , glm::vec3( 1.0f, 1.0f, 1.0f ) );
		}

	public:
		HelloTexture( )
			: SDLTestShaderWindow( ShaderSourcePair("#version 300 es                            			  \n"
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
			, mPNGImage( "resources/testinput/testinput01.png" )
			, mQuadVAO( 0 )
			, mQuadVBO( 0 )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestShaderWindow::initialize( xAttributes );

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

                        glGenVertexArraysOES( 1, &mQuadVAO );

                        glGenBuffers( 1, &mQuadVBO );

                        glBindBuffer( GL_ARRAY_BUFFER, mQuadVBO );

                        glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

                        glBindVertexArrayOES( mQuadVAO );

                        glEnableVertexAttribArray( 0 );

                        glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), ( GLvoid* )0 );

                        glBindBuffer( GL_ARRAY_BUFFER, 0 );

                        glBindVertexArrayOES( 0 );

			if ( !mPNGImage.getIsValid( ) )
			{
				std::cerr << "Unit test resource testinput01.png not found or is corrupt." << std::endl;
				exit( -1 );
			}

			mTexture.initialize( mPNGImage.getWidth( ), mPNGImage.getHeight( ), mPNGImage.toGLTextureBuffer( ) );
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
