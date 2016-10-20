#ifdef OPENGLES_HELLOINSTANCING_UT
#ifdef OPENGLES
#include "test.hpp"

#include "SDLTest.hpp"
#include "SDLShader.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"
#include "PNGImage.hpp"
#include "sisumath.hpp"
#include "GLESBook/esShapes.hpp"

#include <GLES3/gl3.h>
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

static PFNGLBINDVERTEXARRAYOESPROC    glBindVertexArrayOES;
static PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
static PFNGLGENVERTEXARRAYSOESPROC    glGenVertexArraysOES;
static PFNGLISVERTEXARRAYOESPROC      glIsVertexArrayOES;

#ifndef GLBINDVERTEXARRAYOES
 #define GLBINDVERTEXARRAYOES "glBindVertexArray"
#endif

#ifndef GLDELETEVERTEXARRAYSOES
 #define GLDELETEVERTEXARRAYSOES "glDeleteVertexArrays"
#endif

#ifndef GLGENVERTEXARRAYSOES
 #define GLGENVERTEXARRAYSOES "glGenVertexArrays"
#endif

#ifndef GLISVERTEXARRAYOES
 #define GLISVERTEXARRAYOES "glIsVertexArray"
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

namespace {

class HelloInstancing_UT : public context
{
	public:
		HelloInstancing_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

class HelloInstancing : public SDLTestWindow
{
    	static GLfloat const sVertices[5*6*6];
	static glm::vec3 const sCubePositions[];


	Texture2D mTexture, mSecondTexture, mThirdTexture;
	PNGImage mPNGImage, mSecondPNGImage;
	SDLShader mSpriteShader, m3DCameraShader;

	// 2D Render Data
	int32_t mScaleW, mScaleH;
	bool mReverseScale;

	GLuint mQuadVBO, mQuadVAO;

	GLuint mCubeVBO, mCubeVAO;


	// 3D Render data
	// Camera
	glm::vec3 mCameraPos
		, mCameraFront
		, mCameraUp;

	GLfloat mYaw
	      , mPitch
	      , mLastX
	      , mLastY
	      , mFov;

	bool mKeys[1024];

	// Deltatime
	GLfloat mDeltaTime, mLastFrame;

        void _drawTexture( Texture2D & xTexture
                         , glm::vec2 const xPosition
                         , glm::vec2 const xSize
                         , GLfloat xRotate
                         , glm::vec3 const xColor )
        {
                mSpriteShader( [ & ] ( )
                {
                        glm::mat4 model;

                        model = glm::translate( model, glm::vec3( xPosition, 0.0f ) );

                        model = glm::translate( model, glm::vec3( 0.5f * xSize.x, 0.5f * xSize.y, 0.0f ) );

                        model = glm::rotate(    model, xRotate, glm::vec3( 0.0f, 0.0f, 1.0f ) );

                        model = glm::translate( model, glm::vec3( -0.5f * xSize.x, -0.5f * xSize.y, 0.0f ) );

                        model = glm::scale(     model, glm::vec3( xSize, 1.0f ) );

                        mSpriteShader.getUniforms( ).setUniformMatrix4fv( "model", model );

                        mSpriteShader.getUniforms( ).setUniformVector3f( "spriteColor", xColor );

                        glActiveTexture( GL_TEXTURE0 );

                        xTexture( [ & ]( )
                        {
                                glBindVertexArrayOES( mQuadVAO );

                                glDrawArrays( GL_TRIANGLES, 0, 6 );

                                glBindVertexArrayOES( 0 );
                        } );
                } );
        }

	void _initialize2DOverlay( )
	{
		glm::mat4 projection = glm::ortho(  0.0f
                                                  , static_cast<GLfloat>( mW )
                                                  , static_cast<GLfloat>( mH )
                                                  ,  0.0f
                                                  , -1.0f
                                                  ,  1.0f );

                mSpriteShader( [ & ]( )
                {
			mSpriteShader.getUniforms( ).setUniform1i( "image", 0 );
			mSpriteShader.getUniforms( ).setUniformMatrix4fv( "projection", projection );
		});

		static GLfloat const vertices[] = { // Pos      // Tex
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

		mTexture.initialize( ( mScaleW = mPNGImage.getWidth( ) )
				   , ( mScaleH = mPNGImage.getHeight( ) )
				   , mPNGImage.toGLTextureBuffer( ) );

	}

	void _render2DOverlay( )
	{
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
	protected:
		virtual void render( )
		{
			// Set the viewport
			glViewport( 0, 0, mW, mH );

			// Clear the color buffer
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

			m3DCameraShader([&]( ) {

				glm::mat4 view        = glm::lookAt( mCameraPos, mCameraPos + mCameraFront, mCameraUp );

				glm::mat4 projection  = glm::perspective( mFov, (GLfloat) mW / (GLfloat) mH, 0.1f, 100.0f );

				m3DCameraShader.getUniforms( ).setUniformMatrix4fv("view"       , view );

				m3DCameraShader.getUniforms( ).setUniformMatrix4fv("projection", projection );

				_checkForGLError( "After set view and rpojection" );

				glBindVertexArray( mCubeVAO );

				GLint const modelLoc = m3DCameraShader.getUniforms( )["model"];

				for ( GLuint i = 0; i < 10; i++ )
				{
					glm::mat4 model;

					model = glm::translate( model, sCubePositions[ i ] );

					GLfloat angle = 20.0f * i;

					model = glm::rotate( model, angle, glm::vec3( 1.0f, 0.3f, 0.5f ) );

					m3DCameraShader.getUniforms( ).setUniformMatrix4fv( "model", model );

					_checkForGLError( "After set model" );

					glDrawArrays( GL_TRIANGLES, 0, 6 * 6 );
				}

				glBindVertexArray( 0 );

				_checkForGLError( "After render 3d" );

			} );

			_render2DOverlay( );

			_checkForGLError( "After render 2d" );
		}

	public:
		HelloInstancing( )
			: SDLTestWindow( )
			, mSpriteShader( ShaderSourcePair("#version 300 es                            			    \n"
      				   			  "layout(location = 0) in vec4 vertex;       			    \n"
	 	 				          "out vec2 TexCoords;                        			    \n"
						          "uniform mat4 model;                        			    \n"
						    	  "uniform mat4 projection;                   			    \n"
						    	  "void main()                               			    \n"
						    	  "{                                          			    \n"
						    	  "   TexCoords = vertex.zw;                  			    \n"
						    	  "   gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0); \n"
						    	  "}                                          			    \n"
						   	 ,"#version 300 es                                                  \n"
						    	  "precision mediump float;				            \n"
   	  		  			    	  "in vec2 TexCoords;				                    \n"
   	  		  			    	  "out vec4 color;				                    \n"
						    	  "uniform sampler2D image; 				            \n"
						    	  "uniform vec3 spriteColor;                                        \n"
						    	  "void main()                                                      \n"
						    	  "{                                                                \n"
						    	  "  color = vec4(spriteColor, 1.0) * texture(image, TexCoords);    \n"
						    	  "}                                                                \n") )
			, m3DCameraShader( ShaderSourcePair("#version 300 es                                                      \n"
		 					    "layout (location = 0) in vec3 position;                              \n"
							    "layout (location = 2) in vec2 texCoord;                              \n"
				   	  		    "out vec2 TexCoord;                                                   \n"
			 		   		    "uniform mat4 model;                                                  \n"
					   		    "uniform mat4 view;                                                   \n"
					   		    "uniform mat4 projection;                                             \n"
					   		    "void main()                                                          \n"
					    		    "{                                                                    \n"
					 		    "    gl_Position = projection * view * model * vec4(position, 1.0f);  \n"
							    "    TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);                   \n"
			 				    "}                                                                    \n"
		  	   	 			  , "#version 300 es                                                                       \n"
					    	            "precision mediump float;				                                   \n"
			 				    "in vec2 TexCoord;                                                                     \n"
							    "out vec4 color;                                                                       \n"
					  		    "uniform sampler2D ourTexture1;                                                        \n"
					  		    "uniform sampler2D ourTexture2;                                                        \n"
					  		    "void main()                                                                           \n"
							    "{                                                                                     \n"
					    		    "    color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2); \n"
					 		    "}                                                                                     \n" ) )
			, mTexture( )
			, mSecondTexture( )
			, mPNGImage( "resources/testinput/testinput01.png" )
			, mSecondPNGImage( "resources/testinput/testinput02.png" )
			// 2D Render Data
			, mQuadVAO( 0 )
			, mQuadVBO( 0 )
			, mScaleW( 0 )
			, mScaleH( 0 )
			, mReverseScale( false )
			// 3d Render Data
			, mCameraPos( glm::vec3(0.0f, 0.0f,  3.0f) )
			, mCameraFront( glm::vec3(0.0f, 0.0f, -1.0f) )
			, mCameraUp( glm::vec3(0.0f, 1.0f,  0.0f) )
			, mYaw(  -90.0f ) // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
			, mPitch( 0.0f )
			, mLastX( 0 )
			, mLastY( 0 )
			, mFov( 45.0f )
			, mKeys( )
			, mDeltaTime( 0.0f )
			, mLastFrame( 0.0f )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			mSpriteShader.initialize( );

			m3DCameraShader.initialize( );

			_initialize2DOverlay( );

			mLastX = mW / 2.0;
			mLastY = mH / 2.0;

    			glGenVertexArrays(1, &mCubeVAO);
    			glGenBuffers(1, &mCubeVBO);

    			glBindVertexArray(mCubeVAO);

    			glBindBuffer(GL_ARRAY_BUFFER, mCubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof( sVertices), sVertices, GL_STATIC_DRAW);

    			// Position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    			glEnableVertexAttribArray(0);
    			// TexCoord attribute
    			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    			glEnableVertexAttribArray(2);

    			glBindVertexArray(0); // Unbind VAO

			_checkForGLError( "After set vertices" );

			if ( !mSecondPNGImage.getIsValid( ) )
			{
				std::cerr << "Unit test resource testinput02.png not found or is corrupt." << std::endl;
				exit( -1 );
			}

			mSecondTexture.initialize( mSecondPNGImage.getWidth( )
					         , mSecondPNGImage.getHeight( )
					         , mSecondPNGImage.toGLTextureBuffer( ) );

			_checkForGLError( "After initialize texture" );

			m3DCameraShader([&](){
				auto __initializeTextureUnit = [&]( GLenum const xTextureUnit, Texture2D & xTexture, const char * xName, GLint const xValue )
				{
					glActiveTexture( xTextureUnit );
					_checkForGLError( "glActiveTexture" );

					xTexture([&]( ) {
						m3DCameraShader.getUniforms( ).setUniform1i( xName, xValue );
					} );
				};

				__initializeTextureUnit( GL_TEXTURE0, mTexture      , "ourTexture1", 0);

				__initializeTextureUnit( GL_TEXTURE1, mSecondTexture, "ourTexture2", 1);
			});
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

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat const HelloInstancing::sVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    glm::vec3 const HelloInstancing::sCubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };


} // namespace

TEST(HelloInstancing_UT, HelloInstancing)
{
	HelloInstancing * window = new HelloInstancing;
	window->initialize( { 3, 0, 1, false, false } );
	window->run( );
	delete window;
}
#endif // OPENGLES
#endif // OPENGLES_HELLOINSTANCING_UT
