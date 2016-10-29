#ifndef CUBE_RENDERER_F5C942BC83B946389CD32C39AAB7D2F1_HPP_
#define CUBE_RENDERER_F5C942BC83B946389CD32C39AAB7D2F1_HPP_

#include "SDLShader.hpp"
#include "Stopwatch.hpp"
#include "Texture2D.hpp"
#include "PNGImage.hpp"
#include "sisumath.hpp"
#include "Quad.hpp"
#include "Oscillator.hpp"

#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

namespace sisu {

class CubeRenderer
{
	static GLfloat const constexpr sFovStep = 0.05f;

    	static GLfloat const sVertices[5*6*6];
	static glm::vec3 const sCubePositions[10];

	Texture2D mSecondTexture
		, mThirdTexture
		, mFourthTexture
		, mFifthTexture
		, mSixthTexture;

	PNGImage mSecondPNGImage
	       , mThirdPNGImage
	       , mFourthPNGImage
	       , mFifthPNGImage
	       , mSixthPNGImage;

	std::vector< Oscillator< GLfloat > > mOscillators;
	GLfloat  mLastX, mLastY;

	SDLShader m3DCameraShader;
	GLuint mCubeVBO, mCubeVAO;

	Quad mPlane;

	bool mFirstMouse;

	// Deltatime
	GLfloat mDeltaTime, mLastFrame;

	Stopwatch mDeltaWatch;

	uint32_t mW, mH;

	Camera * mCamera;

	void _checkCamera( ) const
	{
		if ( mCamera == NULL )
		{
			std::cerr << "Null camera in " <<  __PRETTY_FUNCTION__ << std::endl;
			exit( -1 );
		}
	}

	void _render( std::function<void(void)> xLambda )
	{
		_checkCamera( );

		m3DCameraShader([&]( ) {
			glm::mat4 projection  = glm::perspective( mCamera->getFOV( ), (GLfloat) mW / (GLfloat) mH, 0.1f, 100.0f );

			m3DCameraShader.getUniforms( ).setUniformMatrix4fv("view"       , mCamera->getViewMatrix( ) );

			m3DCameraShader.getUniforms( ).setUniformMatrix4fv("projection", projection );

			xLambda( );

			_checkForGLError( "After render 3d" );
		} );
	}

	void _setModelMatrix( glm::vec3 const & xPosition
			    , GLfloat const xRotateAngle
			    , glm::vec3 const & xScale )
	{
		glm::mat4 model;

		model = glm::translate( model, xPosition );

		model = glm::rotate( model, xRotateAngle, glm::vec3( 1.0f, 0.3f, 0.5f ) );

        	model = glm::scale( model, xScale );

		m3DCameraShader.getUniforms( ).setUniformMatrix4fv( "model", model );
	}

	void _drawCubes( GLfloat const xAngleOffset )
	{
		for ( GLuint ii = 0; ii <  sizeof( sCubePositions ) / sizeof( glm::vec3 ); ii++ )
		{
			GLfloat const angle = xAngleOffset * ii;

			GLfloat const scale = ++mOscillators[ ii ];

			_setModelMatrix( sCubePositions[ ii ]
				       , angle
				       , glm::vec3( scale
					          , scale
					          , scale ) );

			glBindVertexArray( mCubeVAO );

			glDrawArrays( GL_TRIANGLES, 0, 6 * 6 );

			glBindVertexArray( 0 );
		}

	}

	void _drawCube( glm::vec3 const & xPosition
  		      , GLfloat const xAngle
		      , glm::vec3 const & xScale )
	{
		_setModelMatrix( xPosition, xAngle, xScale );

		glBindVertexArray( mCubeVAO );

		glDrawArrays( GL_TRIANGLES, 0, 6 * 6 );

		glBindVertexArray( 0 );

	}


	public:
		CubeRenderer( )
			: m3DCameraShader( ShaderSourcePair("#version 300 es                                                      \n"
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
			, mSecondTexture( )
			, mThirdTexture( )
			, mFourthTexture( )
			, mFifthTexture( )
			, mSixthTexture( )
			, mSecondPNGImage( "resources/testinput/testinput14.png" ) // TODO:  Make textures dynamic! Add PBOs, etc
			, mThirdPNGImage(  "resources/testinput/testinput15.png" )
			, mFourthPNGImage( "resources/testinput/testinput17.png" )
			, mFifthPNGImage(  "resources/testinput/testinput18.png" )
			, mSixthPNGImage(  "resources/testinput/testinput19.png" )
			, mOscillators( sizeof( sCubePositions ) / sizeof( glm::vec3 ), Oscillator<GLfloat>( 1.0f, 1.0f, 2.0f, 0.025f ) )
			, mLastX( 0 )
			, mLastY( 0 )
			, mDeltaTime( 0.0f )
			, mLastFrame( 0.0f )
			, mDeltaWatch( )
			, mFirstMouse( true )
			, mW( 0 )
			, mH( 0 )
			, mCamera( NULL )
		{
			;
		}

		void initialize( uint32_t const xW, uint32_t const xH, Camera * const xCamera )
		{
			mW = xW;
			mH = xH;

			mCamera = xCamera;

			_checkCamera( );

			m3DCameraShader.initialize( );

			mPlane.initialize( );

			glEnable(GL_BLEND);
			glEnable( GL_DEPTH_TEST );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			glDepthFunc( GL_LESS );

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

			auto __initializeTex = [&]( PNGImage & xImage, Texture2D & xTex )
			{
				if ( !xImage.getIsValid( ) )
				{
					std::cerr << "Failed to load PNG image resource." << std::endl;
					exit( -1 );
				}

				xTex.initialize( xImage.getWidth( )
					       , xImage.getHeight( )
					       , xImage.toGLTextureBuffer( ) );
			};

			__initializeTex( mSecondPNGImage, mSecondTexture );
			__initializeTex( mThirdPNGImage , mThirdTexture  );
			__initializeTex( mFourthPNGImage, mFourthTexture );
			__initializeTex( mFifthPNGImage , mFifthTexture  );
			__initializeTex( mSixthPNGImage , mSixthTexture  );

			_checkForGLError( "After initialize texture" );

			m3DCameraShader([&](){
				m3DCameraShader.getUniforms( ).setUniform1i( "ourTexture1", 1 );
				m3DCameraShader.getUniforms( ).setUniform1i( "ourTexture2", 2 );
			});

			mDeltaWatch.startS( );
		}

		void onKeyboardEvent( KeyboardEvent const & xEvent )
		{
			_checkCamera( );

			// Camera controls
			if ( xEvent.getKeyUp( ) )
			{
 				mCamera->setMovementDirection( eMovementDirection_None );
				return;
			}

			switch ( xEvent.getScanCode( ) )
			{
				case SDL_SCANCODE_W:
	       			{
					mCamera->setMovementDirection( eMovementDirection_W );
					break;
				}

				case SDL_SCANCODE_S:
	       			{
					mCamera->setMovementDirection( eMovementDirection_S );
					break;
				}

				case SDL_SCANCODE_A:
        			{
					mCamera->setMovementDirection( eMovementDirection_A );
					break;
				}

				case SDL_SCANCODE_D:
        			{
					mCamera->setMovementDirection( eMovementDirection_D );
					break;
				}

				default:
        			{
					mCamera->setMovementDirection( eMovementDirection_None );
					break;
				}
			}
		}

		void onMouseEvent( MouseEventInfo const & xEvent )
		{
			_checkCamera( );

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
				mCamera->setHorizontalCameraRotation( eHorizontalCameraRotation_Left );
			}
			else if( xEvent.x >= ( mW - 1 ) ) // Cursor is on the right of the screen
			{
				mCamera->setHorizontalCameraRotation( eHorizontalCameraRotation_Right );
			}
			else
			{
				mCamera->setHorizontalCameraRotation( eHorizontalCameraRotation_None );
			}

			if ( xEvent.y <= 0 ) // Cursor is at the top of the screen
			{
				mCamera->setVerticalCameraRotation( eVerticalCameraRotation_Down );
			}
			else if( xEvent.y >= ( mH - 1 ) ) // Cursor is at the right of the screen
			{
				mCamera->setVerticalCameraRotation( eVerticalCameraRotation_Up );
			}
			else
			{
				mCamera->setVerticalCameraRotation( eVerticalCameraRotation_None );
			}

			GLfloat yoffset = mLastY - xEvent.y; // Reversed since y-coordinates go from bottom to left

			mLastX = xEvent.x;
			mLastY = xEvent.y;

			mCamera->rotateCamera( xoffset, yoffset );

			// TODO: Fix. Event has no 'offset' given, just whether the wheel has moved up or down.
			GLfloat const scrollOffset = xEvent.wheelHasMovedUp( ) ?
							     sFovStep : ( xEvent.wheelHasMovedDown( ) ?
								  	  -sFovStep : 0.0f );

			mCamera->setFov( scrollOffset );
		}

		void render( std::function<void(void)> xLambda
			   , glm::vec3 const & xPosition
			   , GLfloat const xRotate
			   , glm::vec3 const & xScale )
		{
			_render([&]() {

				_setModelMatrix( xPosition, xRotate, xScale );

				xLambda( );
			});
		}
		// TODO: Parameterize this so that translate, rotate, and scale are parameters.
		// We will draw a single cube instead (less efficient but more to our use case.)
		// Or specify a list of N cubes and their positions (better )
		void render3DScene( )
		{
			_render( [&]() {
				_setModelMatrix( glm::vec3( -10, -10, -10 )
					       , 90
					       , glm::vec3( 1000.0f
 						          , 1000.0f
						          , 1000.0f ) );

				mPlane.render( mFourthTexture, GL_TEXTURE1 );

				_checkForGLError( "After render plane." );

				glActiveTexture( GL_TEXTURE1 );

				mThirdTexture([&]( )
				{
					glActiveTexture( GL_TEXTURE2 );

					mSecondTexture([&]( )
					{
						_drawCubes( 20.0f );
					} );
				} );

				glActiveTexture( GL_TEXTURE1 );

				auto __drawCubeSeries = [&]( Texture2D & xTex, GLint const xPosition )
				{
					xTex([&]( )
					{
						_drawCube( glm::vec3( xPosition, -5, -5 )
							 , 0
							 , glm::vec3( 1.0f, 1.0f, 1.0f ) );
					} );
				};

				__drawCubeSeries( mSecondTexture, -1 );
				__drawCubeSeries( mThirdTexture , -2 );
				__drawCubeSeries( mFourthTexture, -3 );
				__drawCubeSeries( mFifthTexture , -4 );
				__drawCubeSeries( mSixthTexture , -5 );
			} );
		}

		void updateCameras( )
		{
			_checkCamera( );

			GLfloat const currentFrame = mDeltaWatch.stop( );

			mDeltaTime = currentFrame - mLastFrame;

			mLastFrame = currentFrame;

			mCamera->moveCamera( mDeltaTime );

			mCamera->handleCameraRotation( );

		}
};

    // Set up vertex data (and buffer(s)) and attribute pointers
GLfloat const CubeRenderer::sVertices[] = {
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

glm::vec3 const CubeRenderer::sCubePositions[] = {
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

} // namespace sisu
#endif // CUBE_RENDERER_F5C942BC83B946389CD32C39AAB7D2F1_HPP_
