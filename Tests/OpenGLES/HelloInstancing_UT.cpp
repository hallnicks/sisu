#ifdef OPENGLES_HELLOINSTANCING_UT
#ifdef OPENGLES
#include "test.hpp"

#include "SDLTest.hpp"
#include "SDLShader.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"
#include "PNGImage.hpp"
#include "sisumath.hpp"
#include "mouse.hpp"
#include "keyboard.hpp"

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


class Overlay2D
{
	SDLShader mSpriteShader;
	GLuint mQuadVBO, mQuadVAO;

	public:
		Overlay2D( )
			: mSpriteShader( ShaderSourcePair("#version 300 es                            			    \n"
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
						    	  "uniform vec4 spriteColor;                                        \n"
						    	  "void main()                                                      \n"
						    	  "{                                                                \n"
						    	  "  color = spriteColor * texture(image, TexCoords);    \n"
						    	  "}                                                                \n") )
			, mQuadVBO( 0 )
			, mQuadVAO( 0 )

		{
			;
		}

		void initialize( uint32_t const xW, uint32_t const xH )
		{
			mSpriteShader.initialize( );

			glm::mat4 projection = glm::ortho(  0.0f
	                                                  , static_cast<GLfloat>( xW )
	                                                  , static_cast<GLfloat>( xH )
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
		}

	        void drawTexture( Texture2D & xTexture
	                        , glm::vec2 const xPosition
	                        , glm::vec2 const xSize
	                        , GLfloat xRotate
	                        , glm::vec4 const xColor )
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

        	                mSpriteShader.getUniforms( ).setUniformVector4f( "spriteColor", xColor );

	                        glActiveTexture( GL_TEXTURE0 );

	                        xTexture( [ & ]( )
        	                {
	                                glBindVertexArrayOES( mQuadVAO );

                	                glDrawArrays( GL_TRIANGLES, 0, 6 );

	                                glBindVertexArrayOES( 0 );
	                        } );
	                } );
	        }
};

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

	Mouse mMouse;
	Keyboard mKeyboard;

	Overlay2D mOverlay2D;

	Texture2D mTexture, mSecondTexture, mThirdTexture;
	PNGImage mPNGImage, mSecondPNGImage, mThirdPNGImage;

	SDLShader m3DCameraShader;
	// 2D Render Data
	int32_t mScaleW, mScaleH;
	bool mReverseScale;

	GLuint mCubeVBO, mCubeVAO;

	// 3D Render data
	// Camera
	glm::vec3 mCameraPos
		, mCameraFront
		, mCameraUp;

	enum eMovementDirection
	{
		eMovementDirection_None,
		eMovementDirection_W,
		eMovementDirection_A,
		eMovementDirection_S,
		eMovementDirection_D,
	} mMovementDirection;

	enum eHorizontalCameraRotation
	{
		eHorizontalCameraRotation_None,
		eHorizontalCameraRotation_Left,
		eHorizontalCameraRotation_Right,
	} mHorizontalCameraRotation;

	enum eVerticalCameraRotation
	{
		eVerticalCameraRotation_None,
		eVerticalCameraRotation_Up,
		eVerticalCameraRotation_Down,
	} mVerticalCameraRotation;

	GLfloat mYaw
	      , mPitch
	      , mLastX
	      , mLastY
	      , mFov
	      , mCameraSpeed;

	bool mFirstMouse;

	// Deltatime
	GLfloat mDeltaTime, mLastFrame;

	Stopwatch mDeltaWatch;

	void _initialize2DOverlay( )
	{
		mOverlay2D.initialize( mW, mH );

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

		mOverlay2D.drawTexture( mTexture
	 			      , glm::vec2( 0, 0 )
 	  		 	      , glm::vec2( mScaleW, mScaleH )
				      , rotate
			    	      , glm::vec4( dRand( 0.0f, 1.0f )
				       		 , dRand( 0.0f, 1.0f )
                                       		 , dRand( 0.0f, 1.0f )
				       		 , 0.1f ) );
	}


	void _render3DScene( )
	{
		m3DCameraShader([&]( ) {
			glm::mat4 view        = glm::lookAt( mCameraPos, mCameraPos + mCameraFront, mCameraUp );

			glm::mat4 projection  = glm::perspective( mFov, (GLfloat) mW / (GLfloat) mH, 0.1f, 100.0f );

			m3DCameraShader.getUniforms( ).setUniformMatrix4fv("view"       , view );

			m3DCameraShader.getUniforms( ).setUniformMatrix4fv("projection", projection );

			_checkForGLError( "After set view and rpojection" );

			glActiveTexture( GL_TEXTURE1 );

			mThirdTexture([&]( )
			{
				glActiveTexture( GL_TEXTURE2 );

				mSecondTexture([&]( )
				{
					glBindVertexArray( mCubeVAO );

					GLint const modelLoc = m3DCameraShader.getUniforms( )["model"];

					for ( GLuint i = 0; i < 10; i++ )
					{
						glm::mat4 model;

						model = glm::translate( model, sCubePositions[ i ] );

						GLfloat angle = 20.0f * i;

						model = glm::rotate( model, angle, glm::vec3( 1.0f, 0.3f, 0.5f ) );

		                	        model = glm::scale( model, glm::vec3( dRand( 1.0f, 2.0f )
										    , dRand( 1.0f, 2.0f )
										    , dRand( 1.0f, 2.0f ) ) );

						m3DCameraShader.getUniforms( ).setUniformMatrix4fv( "model", model );

						_checkForGLError( "After set model" );

						glDrawArrays( GL_TRIANGLES, 0, 6 * 6 );
					}

					glBindVertexArray( 0 );
				} );
			} );

			_checkForGLError( "After render 3d" );
		} );

	}

	void _initialize3DScene( )
	{
		m3DCameraShader.initialize( );

		glEnable( GL_BLEND );

		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glEnable( GL_DEPTH_TEST );

		glDepthFunc( GL_LEQUAL );

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

		if ( !mThirdPNGImage.getIsValid( ) )
		{
			std::cerr << "Unit test resource testinput03.png not found or is corrupt." << std::endl;
			exit( -1 );
		}

		mThirdTexture.initialize( mThirdPNGImage.getWidth( )
				         , mThirdPNGImage.getHeight( )
				         , mThirdPNGImage.toGLTextureBuffer( ) );

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

			__initializeTextureUnit( GL_TEXTURE1, mSecondTexture, "ourTexture1", 1);

			__initializeTextureUnit( GL_TEXTURE2, mThirdTexture, "ourTexture2", 2);
		});
	}

	void _rotateCamera( GLfloat xXOffset, GLfloat xYOffset )
	{
		GLfloat sensitivity = 0.05;	// Change this value to your liking // TODO: Make this a member var configurable from settings screen
		xXOffset *= sensitivity;
		xYOffset *= sensitivity;

		mYaw   += xXOffset;
		mPitch += xYOffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (mPitch > 89.0f)
		{
		    mPitch = 89.0f;
		}

		if (mPitch < -89.0f)
		{
		    mPitch = -89.0f;
		}

		glm::vec3 front;
		front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		front.y = sin(glm::radians(mPitch));
		front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		mCameraFront = glm::normalize(front);
	}

	void _moveCamera( )
	{
		switch ( mMovementDirection )
		{
			case eMovementDirection_W:
		    	{
				mCameraPos += mCameraSpeed * mCameraFront;
				break;
			}

			case eMovementDirection_S:
			{
				mCameraPos -= mCameraSpeed * mCameraFront;
				break;
			}

			case eMovementDirection_A:
	        	{
				mCameraPos -= glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed;
				break;
			}

			case eMovementDirection_D:
	        	{
				mCameraPos += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed;
				break;
			}

			default:
				break;
		}

	}

	void _handleCameraRotation( )
	{
		static GLfloat const sMovementSpeed = 10.0f;

		switch ( mHorizontalCameraRotation )
		{
			case eHorizontalCameraRotation_Left:
			{
				_rotateCamera( -sMovementSpeed, 0 );
				break;
			}

			case eHorizontalCameraRotation_Right:
			{
				_rotateCamera( sMovementSpeed, 0 );
				break;
			}

			case eHorizontalCameraRotation_None:
			default:
				break;

		}

		switch ( mVerticalCameraRotation )
		{
			case eVerticalCameraRotation_Up:
			{
				_rotateCamera( 0, -sMovementSpeed );
				break;
			}

			case eVerticalCameraRotation_Down:
			{
				_rotateCamera( 0, sMovementSpeed );
				break;
			}

			case eVerticalCameraRotation_None:
			default:
				break;
		}
	}

	void _onMouseEvent( MouseEventInfo const & xEvent )
	{
		if( mFirstMouse )
		{
			mLastX = xEvent.x;
			mLastY = xEvent.y;
			mFirstMouse = false;
		}

		GLfloat xoffset = 0.0f;

		xoffset = xEvent.x - mLastX;

		if ( xEvent.x <= 0 ) // Cursor is on the left side of the screen
		{
			mHorizontalCameraRotation = eHorizontalCameraRotation_Left;
		}
		else if( xEvent.x >= ( mW - 1 ) ) // Cursor is on the right of the screen
		{
			mHorizontalCameraRotation = eHorizontalCameraRotation_Right;
		}
		else
		{
			mHorizontalCameraRotation = eHorizontalCameraRotation_None;
		}

		if ( xEvent.y <= 0 ) // Cursor is at the top of the screen
		{
			mVerticalCameraRotation = eVerticalCameraRotation_Down;
		}
		else if( xEvent.y >= ( mH - 1 ) ) // Cursor is at the right of the screen
		{
			mVerticalCameraRotation = eVerticalCameraRotation_Up;
		}
		else
		{
			mVerticalCameraRotation = eVerticalCameraRotation_None;
		}

		GLfloat yoffset = mLastY - xEvent.y; // Reversed since y-coordinates go from bottom to left

		mLastX = xEvent.x;
		mLastY = xEvent.y;

		_rotateCamera( xoffset, yoffset );

		// TODO: Fix. Event has no 'offset' given, just whether the wheel has moved up or down.
		float const scrollOffset = xEvent.wheelHasMovedUp( ) ? xEvent.y : ( xEvent.wheelHasMovedDown( ) ? -xEvent.y : 0.0f );

		if (mFov >= 1.0f && mFov <= 45.0f)
		{
		    mFov -= scrollOffset;
		}

		if (mFov <= 1.0f)
		{
		    mFov = 1.0f;
		}

		if (mFov >= 45.0f)
		{
		    mFov = 45.0f;
		}
	}

	protected:
		virtual void render( )
		{
			// Set the viewport
			glViewport( 0, 0, mW, mH );

			// Clear the color buffer
			glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

			_render3DScene( );

			_render2DOverlay( );

			_checkForGLError( "After render 2d" );
		}

	public:
		HelloInstancing( )
			: SDLTestWindow( )
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
					    		    "    color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.5); \n"
					 		    "}                                                                                     \n" ) )
			, mMouse( )
			, mTexture( )
			, mSecondTexture( )
			, mPNGImage( "resources/testinput/testinput16.png" )
			, mSecondPNGImage( "resources/testinput/testinput14.png" )
			, mThirdPNGImage( "resources/testinput/testinput15.png" )
			// 2D Render Data
			, mScaleW( 0 )
			, mScaleH( 0 )
			, mReverseScale( false )
			// 3d Render Data
			, mCameraPos( glm::vec3(0.0f, 0.0f,  3.0f) )
			, mCameraFront( glm::vec3(0.0f, 0.0f, -1.0f) )
			, mCameraUp( glm::vec3(0.0f, 1.0f,  0.0f) )
			, mMovementDirection( eMovementDirection_None )
			, mYaw(  -90.0f ) // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
			, mPitch( 0.0f )
			, mLastX( 0 )
			, mLastY( 0 )
			, mFov( 45.0f )
			, mDeltaTime( 0.0f )
			, mLastFrame( 0.0f )
			, mDeltaWatch( )
			, mFirstMouse( true )
			, mHorizontalCameraRotation( eHorizontalCameraRotation_None )
			, mVerticalCameraRotation( eVerticalCameraRotation_None )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			_initialize2DOverlay( );

			_initialize3DScene( );

			mMouse.registerCallback([&](MouseEventInfo const & xEvent){
				_onMouseEvent( xEvent );
			});

			mMouse.listen( );

			mDeltaWatch.startS( );

			mKeyboard.registerCallback( [&]( KeyboardEvent const & xEvent )
			{
				// Camera controls
				if ( xEvent.getKeyUp( ) )
				{
      					mMovementDirection = eMovementDirection_None;
					return;
				}

				switch ( xEvent.getScanCode( ) )
				{
					case SDL_SCANCODE_W:
	        			{
						mMovementDirection = eMovementDirection_W;
						break;
					}

					case SDL_SCANCODE_S:
	        			{
						mMovementDirection = eMovementDirection_S;
						break;
					}

					case SDL_SCANCODE_A:
	        			{
						mMovementDirection = eMovementDirection_A;
						break;
					}

					case SDL_SCANCODE_D:
	        			{
						mMovementDirection = eMovementDirection_D;
						break;
					}

					default:
	        			{
						mMovementDirection = eMovementDirection_None;
						break;
					}
				}

			} );

			mKeyboard.listen( );
		}

		virtual void run( )
		{
			Stopwatch t;

			double accum = 0.0;

			do
			{
				t.startMs( );

				GLfloat const currentFrame = mDeltaWatch.stop( );

				mDeltaTime = currentFrame - mLastFrame;

				mLastFrame = currentFrame;

				mCameraSpeed = 1.0f * mDeltaTime;

				_moveCamera( );

				_handleCameraRotation( );

				render( );

				_checkForGLError( );

				SDL_PumpEvents( );

				SDL_GL_SwapWindow( mMainWindow );

				if ( ( accum += t.stop( ) ) >= 30000.0 )
				{
					break;
				}

			} while ( 1 );

			mMouse.stopListening( );
			mKeyboard.stopListening( );

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
