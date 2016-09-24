#ifndef SDL_SHADER_TEST_DBB12D8670054D2E97E6A436C1F0EC51_HPP_
#define SDL_SHADER_TEST_DBB12D8670054D2E97E6A436C1F0EC51_HPP_

#include "SDLTest.hpp"
#include "ioassist.hpp"
#include "ColorRGBA.hpp"
#include "Texture.hpp"
#include "Rect.hpp"
#include "Font.hpp"
#include "VBM.hpp"
#include "threadgears.hpp"

#include <string>
#include <iostream>
#include <functional>
#include <limits>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.H>
#include <GL/glu.h>

#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>

#include "TextureFactory.hpp"
#include "Texture2D.hpp"

namespace sisu {

struct ShaderSourcePair
{
	ShaderSourcePair( const char * xV, const char* xF )
		: VertexSource( xV )
		, FragmentSource( xF )
	{
		;
	}

	char const * const VertexSource;
	char const * const FragmentSource;
};

struct ShaderPathPair
{
	ShaderPathPair( const char * xV, const char * xF )
		: VertexPath( xV )
		, FragmentPath( xF )
	{
		;
	}

	char const * const VertexPath;
	char const * const FragmentPath;
};

class SDLShader
{
	std::string mVertexShaderSource
		  , mFragmentShaderSource;

	protected:
		GLuint mProgramID;

	public:
		SDLShader( ShaderSourcePair const & xShaderSources );
		SDLShader( ShaderPathPair const & xShaderPaths );
		~SDLShader( );

		class Uniforms
		{
			GLuint const mProgramID;

			Uniforms( GLuint const xProgramID )
				: mProgramID( xProgramID )
			{
				;
			}

			friend class SDLShader;

			public:
				GLint operator [ ] ( const char * xName )
				{
					return glGetUniformLocation( mProgramID, xName );
				}

				void setUniform1i( const char * xName, GLint const xValue )
				{
					glUniform1i( (*this)[ xName ], xValue );
				}


				void setUniform1f( const char * xName, GLfloat const xValue )
				{
					glUniform1f( (*this)[ xName ], xValue );
				}

				void setUniformMatrix4fv( const char * xName, glm::mat4 const & xMatrix )
				{
					glUniformMatrix4fv( (*this)[ xName ], 1, GL_FALSE, glm::value_ptr( xMatrix ) );
				}

				void setUniformVector3f( const char * xName, glm::vec3 const & xValue )
				{
					glUniform3f( (*this)[ xName ], xValue.x, xValue.y, xValue.z );
				}
		};

		virtual void initialize( );

		virtual SDLShader & operator( )( std::function<void(void)> xLambda );

		Uniforms getUniforms() { return Uniforms( mProgramID ); }

		GLuint getProgramID( ) const { return mProgramID; }
};

class SDLTestShaderWindow : public SDLTestWindow
{
	protected:
		SDLShader mShader;

	public:
		SDLTestShaderWindow( ShaderSourcePair const & xShaderSources );
		SDLTestShaderWindow( ShaderPathPair const & xShaderPaths );
		~SDLTestShaderWindow( );

		virtual void render( );

		virtual void run( );

		virtual SDLTestShaderWindow & operator( )( std::function<void(void)> xLambda );

		virtual void initialize( OpenGLAttributes const & xOpenGLAttributes );
};

class SDLQuadShader : public SDLTestShaderWindow
{
	GLint mVertexPos2DLocation;

	GLuint mVBO, mIBO;

	protected:
		virtual void render( );
	public:
		SDLQuadShader( );

		virtual void initialize( OpenGLAttributes const & xAttributes );
};

class StackOverflowGuerillaShader : public SDLTestShaderWindow
{
	GLuint mTextureID;
	GLuint * mBuffer;
	public:
		StackOverflowGuerillaShader( )
			: SDLTestShaderWindow( ShaderSourcePair("#version 330 core\n"
								"layout(points) in;"
								"layout(triangle_strip, max_vertices = 4) out;"
								"out vec2 texcoord;"
								"void main() "
								"{"
								"    gl_Position = vec4( 1.0, 1.0, 0.5, 1.0 );"
								"    texcoord = vec2( 1.0, 1.0 );"
								"    EmitVertex();"
								"    gl_Position = vec4(-1.0, 1.0, 0.5, 1.0 );"
								"    texcoord = vec2( 0.0, 1.0 ); "
								"    EmitVertex();"
								"    gl_Position = vec4( 1.0,-1.0, 0.5, 1.0 );"
								"    texcoord = vec2( 1.0, 0.0 );"
								"    EmitVertex();"
								"    gl_Position = vec4(-1.0,-1.0, 0.5, 1.0 );"
								"    texcoord = vec2( 0.0, 0.0 ); "
								"    EmitVertex();"
								"    EndPrimitive(); "
							      , "#version 330 core\n"
								"void main(){}") )
		, mTextureID( 0 )
		, mBuffer( NULL )
		{
			;
		}

		~StackOverflowGuerillaShader( )
		{
			if ( mBuffer != NULL )
			{
				delete[] mBuffer;
			}
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestShaderWindow::initialize( xAttributes );

			mBuffer = new GLuint[ mH * mW ];

			for ( uint32_t ii = 0; ii <  (mH * mW); ++ii )
			{
				mBuffer[ii] = rand( );
			}

			glGenTextures( 1, &mTextureID );

			glBindTexture( GL_TEXTURE_2D, mTextureID );

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, mW, mH, 0, GL_RGBA, GL_UNSIGNED_BYTE, mBuffer );

                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
                        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

                        glBindTexture( GL_TEXTURE_2D, 0 );

			glClearColor(0.05f, 0.1f, 1.0f, 1.0f);

			// TODO: Should only do this on resize
			glViewport(0, 0, mW, mH );

			_checkForGLError( );
		}


		virtual void run( )
		{
			for( int ii = 0; ii < 50000; ++ii )
			{
				render( );
				SDL_GL_SwapWindow( mMainWindow );
			}

			_hide( );
		}

	protected:
		virtual void render( )
		{
			SDLTestShaderWindow::render( );

			glEnable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                        glBindTexture( GL_TEXTURE_2D, mTextureID );

			mShader([](){ glDrawArrays(GL_POINTS, 0, 1); });
		}


};

class SpriteShader : public SDLTestWindow
{
	SDLShader mSpriteShader;
	Texture2D mTexture;
	uint8_t * mRandomData;

	GLuint mVBO, mQuadVAO;

	event mQuit;

	void _drawSprite( Texture2D & xTexture
		        , glm::vec2 const xPosition
			, glm::vec2 const xSize
			, GLfloat xRotate
			, glm::vec3 const xColor )
	{
		mSpriteShader( [ & ] ( ) {

			glm::mat4 model;

			model = glm::translate( model, glm::vec3( xPosition, 0.0f ) );

			model = glm::translate( model, glm::vec3( 0.5f * xSize.x, 0.5f * xSize.y, 0.0f ) );

			model = glm::rotate(    model, xRotate, glm::vec3( 0.0f, 0.0f, 1.0f ) );

			model = glm::translate( model, glm::vec3( -0.5f * xSize.x, -0.5f * xSize.y, 0.0f ) );

			model = glm::scale(     model, glm::vec3( xSize, 1.0f ) );

			mSpriteShader.getUniforms( ).setUniformMatrix4fv( "model", model );

			mSpriteShader.getUniforms( ).setUniformVector3f( "spriteColor", xColor );

			glActiveTexture( GL_TEXTURE0 );

			xTexture( [ & ]( ) {

				glBindVertexArray( mQuadVAO );

				glDrawArrays( GL_TRIANGLES, 0, 6 );

				glBindVertexArray( 0 );
			} );
		} );

		_fillRandomData( );

		xTexture( [ & ]( )
		{
			glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, mW, mH, GL_RGBA, GL_UNSIGNED_BYTE, mRandomData );
		});

	}

	void _fillRandomData( )
	{
		for ( uint64_t ii = 0; ii < mW * mH * 4; ++ii )
		{
			mRandomData[ ii ] = rand( ) % 255;
		}
	}

	protected:
		virtual void render( )
		{
			_drawSprite( mTexture, glm::vec2( 0, 0 ), glm::vec2( mW, mH ), 0.0f, glm::vec3( 1.0f, 1.0f, 1.0f ) );
		}

	public:
		SpriteShader( )
			: SDLTestWindow( )
			, mSpriteShader( ShaderPathPair( "resources/sprite.vs.txt", "resources/sprite.fs.txt" ) )
			, mTexture( )
			, mRandomData( NULL )
			, mVBO( 0 )
			, mQuadVAO( 0 )
			, mQuit( )
		{
			;
		}

		~SpriteShader( )
		{
			if ( mRandomData != NULL )
			{
				delete[] mRandomData;
			}

			// TODO: delete quad vao and vbo (!)
		}


		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			if ( mRandomData != NULL )
			{
				std::cout << "RGBA data was already initialized. Calling initialze( .. ) twice is undefined." << std::endl;
				exit( -1 );
			}

			uint64_t const sSize = mW * mH * 4; // Assuming 32 bit color. TODO: Fix or not care

			mRandomData = new uint8_t[ sSize ];

			_fillRandomData( );

			mTexture.initialize( mW, mH, mRandomData );

			mSpriteShader.initialize( );

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


			// TODO: Support more than one sprite!
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

			glGenBuffers( 1, &mVBO );

			glBindBuffer( GL_ARRAY_BUFFER, mVBO );

			glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

			glBindVertexArray( mQuadVAO );

			glEnableVertexAttribArray( 0 );

			glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GLfloat ), (GLvoid*)0 );

			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindVertexArray( 0 );
		}

		virtual void run( )
		{
			while ( !mQuit.isSet( ) )
			{
				render( );
				SDL_GL_SwapWindow( mMainWindow );
				std::cout << "Render." << std::endl;
			}

			std::cout << "Quit set, main thread is out." << std::endl;

			_hide( );
		}

		void stop ( ) { mQuit.set( ); }
};

#define DEPTH_TEXTURE_SIZE 512
#define _M_PI 3.14159265358979323846

#define INSTANCE_COUNT 100

#define FRUSTUM_DEPTH       800.0f
#define DEPTH_TEXTURE_SIZE  512


class RedbookCh04Shader : public SDLTestWindow
{
	SDLShader mShadowShader, mSceneShader;

	float mAspect;

	struct _RenderLightUniforms
	{
		_RenderLightUniforms( )
			: modelViewProjectionMatrix( -1 )
		{
			;
		}

		GLint modelViewProjectionMatrix;

	} mRenderLightUniforms;

	struct _RenderSceneUniforms
	{

		_RenderSceneUniforms( )
			: viewMatrix( -1 )
			, projectionMatrix( -1 )
			, shadowMatrix( -1 )
			, lightPosition( -1 )
			, materialAmbient( -1 )
			, materialDiffuse( -1 )
			, materialSpecular( -1 )
			, materialSpecularPower( -1 )
		{
			;
		}

		GLint modelMatrix
		    , viewMatrix
		    , projectionMatrix
		    , shadowMatrix
		    , lightPosition
		    , materialAmbient
		    , materialDiffuse
		    , materialSpecular
		    , materialSpecularPower;

	} mRenderSceneUniforms;

	GLuint mDepthFBO
	     , mDepthTexture
	     , mGroundVBO
	     , mGroundVAO;

	VBMObject mObject;

	void _drawScene( bool const xDepthOnly )
	{
		if ( !xDepthOnly )
    		{
        		glUniform3fv( mRenderSceneUniforms.materialAmbient,  1, glm::value_ptr(glm::vec3(0.1f, 0.0f, 0.2f)));
        		glUniform3fv( mRenderSceneUniforms.materialDiffuse,  1, glm::value_ptr(glm::vec3(0.3f, 0.2f, 0.8f)));
        		glUniform3fv( mRenderSceneUniforms.materialSpecular, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

        		glUniform1f( mRenderSceneUniforms.materialSpecularPower, 25.0f);
 		}

    		mObject.render( 0, 0 );

    		if ( !xDepthOnly )
    		{
        		glUniform3fv(mRenderSceneUniforms.materialAmbient,  1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));
        		glUniform3fv(mRenderSceneUniforms.materialDiffuse,  1, glm::value_ptr(glm::vec3(0.1f, 0.5f, 0.1f)));
        		glUniform3fv(mRenderSceneUniforms.materialSpecular, 1, glm::value_ptr(glm::vec3(0.1f, 0.1f, 0.1f)));

        		glUniform1f( mRenderSceneUniforms.materialSpecularPower, 3.0f );
		}

    		glBindVertexArray( mGroundVAO );

    		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    		glBindVertexArray(0);
	}

	protected:
		virtual void render( )
		{
			float t = float(SDL_GetTicks() & 0xFFFF) / float(0xFFFF);

			static float q = 0.0f;
			static const glm::vec3 X( 1.0f, 0.0f, 0.0f );
			static const glm::vec3 Y( 0.0f, 1.0f, 0.0f );
			static const glm::vec3 Z( 0.0f, 0.0f, 1.0f );

			glm::vec3 light_position = glm::vec3( sinf( t * 6.0f * 3.141592f ) * 300.0f
							    , 200.0f
							    , cosf(t * 4.0f * 3.141592f) * 100.0f + 250.0f );

			glEnable( GL_CULL_FACE );
			glEnable( GL_DEPTH_TEST );
			glDepthFunc( GL_LEQUAL );

			glm::mat4 scene_model_matrix 	  = glm::rotate(glm::mat4(1.0f),t * float(_M_PI*4), Y);
			glm::mat4 scene_view_matrix 	  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -300.0f));
			glm::mat4 scene_projection_matrix = glm::frustum(-1.0f, 1.0f, -mAspect, mAspect, 1.0f, FRUSTUM_DEPTH);

			const glm::mat4 scale_bias_matrix = glm::mat4(glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
                        			                      glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
                                                  		      glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
   			                                              glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    			// Matrices used when rendering from the light's position
    			glm::mat4 light_view_matrix = glm::lookAt(light_position, glm::vec3(0.0f), Y);
    			glm::mat4 light_projection_matrix(glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, FRUSTUM_DEPTH));

    			// Now we render from the light's position into the depth buffer.
    			// Select the appropriate program
			mShadowShader([&](){

				glUniformMatrix4fv( mRenderLightUniforms.modelViewProjectionMatrix
						  , 1
						  , GL_FALSE
						  , glm::value_ptr(light_projection_matrix * light_view_matrix * scene_model_matrix));

    				// Bind the 'depth only' FBO and set the viewport to the size of the depth texture
    				glBindFramebuffer(GL_FRAMEBUFFER, mDepthFBO);
    				glViewport(0, 0, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE);

				// Clear
	    			glClearDepth(1.0f);
	    			glClear(GL_DEPTH_BUFFER_BIT);

	    			// Enable polygon offset to resolve depth-fighting isuses
	    			glEnable(GL_POLYGON_OFFSET_FILL);
	    			glPolygonOffset(2.0f, 4.0f);

				// Draw from the light's point of view
	   			_drawScene(true);
	    			glDisable(GL_POLYGON_OFFSET_FILL);

	   			// Restore the default framebuffer and field of view
	   			glBindFramebuffer(GL_FRAMEBUFFER, 0);

	    			glViewport(0, 0, mW, mH);
			});

			mSceneShader([&]() {
	    			// Now render from the viewer's position    			glUseProgram(render_scene_prog);
	    			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	   			 // Setup all the matrices
	    			glUniformMatrix4fv(mRenderSceneUniforms.modelMatrix, 	  1, GL_FALSE, glm::value_ptr(scene_model_matrix));
	   			glUniformMatrix4fv(mRenderSceneUniforms.viewMatrix, 	  1, GL_FALSE, glm::value_ptr(scene_view_matrix));
	    			glUniformMatrix4fv(mRenderSceneUniforms.projectionMatrix, 1, GL_FALSE, glm::value_ptr(scene_projection_matrix));
	    			glUniformMatrix4fv(mRenderSceneUniforms.shadowMatrix, 	  1, GL_FALSE, glm::value_ptr(scale_bias_matrix * light_projection_matrix * light_view_matrix));

	    			glUniform3fv(mRenderSceneUniforms.lightPosition, 1, glm::value_ptr(light_position));

	   			// Bind the depth texture
    				glBindTexture(GL_TEXTURE_2D, mDepthTexture);
	    			glGenerateMipmap(GL_TEXTURE_2D);

	    			// Draw
	    			_drawScene(false);
			});
		}

	public:
		RedbookCh04Shader( )
			: SDLTestWindow( )
			, mShadowShader( ShaderPathPair("resources/shadowmap_shadow.vs.glsl", "resources/shadowmap_shadow.fs.glsl") )
			, mSceneShader( ShaderPathPair("resources/shadowmap_scene.vs.glsl", "resources/shadowmap_scene.fs.glsl") )
			, mAspect( 0 )
			, mRenderLightUniforms( )
			, mRenderSceneUniforms( )
			, mDepthFBO( 0 )
			, mDepthTexture( 0 )
			, mGroundVBO( 0 )
			, mGroundVAO( 0 )
			, mObject( VBMObjectFileDescriptor( "resources/armadillow_low.vbm", 0, 1, 2 ) )
		{
			;
		}

		~RedbookCh04Shader( )
		{
			if ( mGroundVBO != 0 )
			{
				glDeleteBuffers( 1, &mGroundVBO );
			}

			if ( mGroundVAO != 0 )
			{
				glDeleteVertexArrays( 1, &mGroundVAO );
			}
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			mAspect = mH / mW;

			mShadowShader.initialize( );
			mSceneShader.initialize( );

			mRenderSceneUniforms.modelMatrix 	   = mSceneShader.getUniforms( )[ "model_matrix" ];
			mRenderSceneUniforms.viewMatrix 	   = mSceneShader.getUniforms( )[ "view_matrix" ];
			mRenderSceneUniforms.projectionMatrix 	   = mSceneShader.getUniforms( )[ "projection_matrix" ];
			mRenderSceneUniforms.shadowMatrix 	   = mSceneShader.getUniforms( )[ "shadow_matrix" ];
			mRenderSceneUniforms.lightPosition 	   = mSceneShader.getUniforms( )[ "light_position" ];
			mRenderSceneUniforms.materialAmbient 	   = mSceneShader.getUniforms( )[ "material_ambient" ];
			mRenderSceneUniforms.materialDiffuse 	   = mSceneShader.getUniforms( )[ "material_diffuse" ];
			mRenderSceneUniforms.materialSpecular 	   = mSceneShader.getUniforms( )[ "material_specular" ];
			mRenderSceneUniforms.materialSpecularPower = mSceneShader.getUniforms( )[ "material_specular_power" ];

			mSceneShader([&](){

				glUniform1i( mSceneShader.getUniforms()[ "depth_texture" ], 0 );

				glGenTextures( 1, &mDepthTexture );
				glBindTexture( GL_TEXTURE_2D, mDepthTexture );
				glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DEPTH_TEXTURE_SIZE, DEPTH_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
				glBindTexture( GL_TEXTURE_2D, 0 );

				glGenFramebuffers( 1, &mDepthFBO );
				glBindFramebuffer( GL_FRAMEBUFFER, mDepthFBO );
				glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,mDepthTexture, 0 );
				glDrawBuffer( GL_NONE );

				GLuint const status = glCheckFramebufferStatus( GL_FRAMEBUFFER );

				if ( status != GL_FRAMEBUFFER_COMPLETE )
				{
					std::cerr << "Framebuffer error: " << std::hex << status << std::endl;
					exit( -1 );
				}

				glBindFramebuffer( GL_FRAMEBUFFER, 0 );

				static const float ground_vertices[] =
				{
				        -500.0f, -50.0f, -500.0f, 1.0f,
				        -500.0f, -50.0f,  500.0f, 1.0f,
				         500.0f, -50.0f,  500.0f, 1.0f,
				         500.0f, -50.0f, -500.0f, 1.0f,
				};

	 	 	 	static const float ground_normals[] =
				{
				        0.0f, 1.0f, 0.0f,
				        0.0f, 1.0f, 0.0f,
				        0.0f, 1.0f, 0.0f,
				        0.0f, 1.0f, 0.0f
				};

    				glGenVertexArrays(1, &mGroundVAO);
    				glGenBuffers(1, &mGroundVBO);;
				glBindVertexArray(mGroundVAO);
 				glBindBuffer(GL_ARRAY_BUFFER, mGroundVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices) + sizeof(ground_normals), NULL, GL_STATIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ground_vertices), ground_vertices);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(ground_vertices), sizeof(ground_normals), ground_normals);

				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(ground_vertices));
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);

			});

			mObject.initialize( );
		}

		virtual void run( )
		{
			while ( 1 )
			{
				render( );
				SDL_GL_SwapWindow( mMainWindow );
			}

			_hide( );
		}

};

class CheShader : public SDLTestShaderWindow
{
	TextureBase * mTexture;

	protected:
		virtual void render( )
		{
			SDLTestShaderWindow::render( );

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			if ( mTexture != NULL )
			{
				mTexture->bind( 0 );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			}

			mShader([&]() {
				mShader.getUniforms().setUniform1f( "myTexture", 0 );

				glBegin( GL_QUADS );
					glVertex3f(-1.0f, -1.0f,  0.0f);
        				glTexCoord2f(1.0, 0.0);
					glVertex3f( 1.0f, -1.0f,  0.0f);
					glTexCoord2f(1.0, 1.0);
					glVertex3f( 1.0f,  1.0f,  0.0f);
					glTexCoord2f(0.0, 1.0);
					glVertex3f(-1.0f,  1.0f,  0.0f);
					glTexCoord2f(0.0, 0.0);
				glEnd();
			});

			_checkForGLError( );
		}

	public:
		CheShader( )
			: SDLTestShaderWindow( ShaderPathPair( "resources/vertexshader.txt", "resources/fragmentshader.txt" ) )
			, mTexture( NULL )
		{
			;
		}

		~CheShader( )
		{
			if ( mTexture != NULL )
			{
				delete mTexture;

				mTexture = NULL;
			}
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			std::cout << "Before check glew ( )" << std::endl;

			_checkForGLError( );

			std::cout << "after check glew." << std::endl;

			if (!GL_ARB_bindless_texture)
			{
				std::cerr << "GL_ARB_bindless_texture extension not enabled." <<std::endl;
				exit( -1 );
			}


			SDLTestShaderWindow::initialize( xAttributes );


			_checkForGLError( );

			glClearColor( 0.5f, 0.5f, 1.0f, 1.0f );

			glShadeModel( GL_SMOOTH );

			glEnable( GL_DEPTH_TEST );

			glEnable( GL_TEXTURE_2D );

			mTexture = TextureFactory::createTextureFromRandomPixels( );

			_checkForGLError( );

			glViewport(0, 0, mW, mH );

			_checkForGLError( );
		}

		virtual void run( )
		{
			while ( 1 )
			{
				render( );
				SDL_GL_SwapWindow( mMainWindow );
			}

			_hide( );
		}

};

class RedbookCh03Shader : public SDLTestShaderWindow
{
	float mAspect;

	GLuint mVBO, mEBO, mVAO;

	GLint mRenderModelMatrixLoc
	    , mRenderProjectionMatrixLoc;

	protected:
		virtual void render( )
		{
			SDLTestShaderWindow::render( );

			glm::mat4 model_matrix;

			// Setup
			glEnable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Activate simple shading program
			mShader([&](){

				// Set up the model and projection matrix
				glm::mat4 projection_matrix(glm::frustum(-1.0f, 1.0f, -mAspect, mAspect, 1.0f, 500.0f));
				glUniformMatrix4fv(mRenderProjectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

				// Set up for a glDrawElements call
				glBindVertexArray(mVAO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

				// Draw Arrays...
				model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(-3.0f, 0.0f, -5.0f));
				glUniformMatrix4fv(mRenderModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
				glDrawArrays(GL_TRIANGLES, 0, 3);

				// DrawElements
				model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(-1.0f, 0.0f, -5.0f));
				glUniformMatrix4fv(mRenderModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

				// DrawElementsBaseVertex
				model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(1.0f, 0.0f, -5.0f));
				glUniformMatrix4fv(mRenderModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
				glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

				// DrawArraysInstanced
				model_matrix = glm::translate(glm::mat4(1.0f),glm::vec3(3.0f, 0.0f, -5.0f));
				glUniformMatrix4fv(mRenderModelMatrixLoc, 1, GL_FALSE, glm::value_ptr(model_matrix));
				glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);
			});
		}
	public:
		RedbookCh03Shader( )
			: SDLTestShaderWindow( ShaderPathPair("resources/primitive_restart.vs.glsl", "resources/primitive_restart.fs.glsl") )
			, mAspect( 0.0f )
			, mVBO( 0 )
			, mVAO( 0 )
			, mEBO( 0 )
			, mRenderModelMatrixLoc( -1 )
			, mRenderProjectionMatrixLoc( -1 )
		{
			;
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestShaderWindow::initialize( xAttributes );

			mShader([&](){

				mRenderModelMatrixLoc = glGetUniformLocation( mShader.getProgramID( ), "model_matrix" );

				mRenderProjectionMatrixLoc = glGetUniformLocation( mShader.getProgramID( ), "projection_matrix" );

				// A single triangle
				static const GLfloat vertex_positions[] = {
									      -1.0f, -1.0f,  0.0f, 1.0f,
									      1.0f, -1.0f,  0.0f, 1.0f,
									      -1.0f,  1.0f,  0.0f, 1.0f,
									      -1.0f, -1.0f,  0.0f, 1.0f,
				  };

				// Color for each vertex
				static const GLfloat vertex_colors[] = {
									      1.0f, 1.0f, 1.0f, 1.0f,
									      1.0f, 1.0f, 0.0f, 1.0f,
									      1.0f, 0.0f, 1.0f, 1.0f,
									      0.0f, 1.0f, 1.0f, 1.0f
				  };

				// Indices for the triangle strips
				static const GLushort vertex_indices[] = { 0, 1, 2 };

				// Set up the element array buffer
				glGenBuffers(1, &mEBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex_indices), vertex_indices, GL_STATIC_DRAW);

				// Set up the vertex attributes
				glGenVertexArrays(1, &mVAO);
				glBindVertexArray(mVAO);

				glGenBuffers(1, &mVBO);
				glBindBuffer(GL_ARRAY_BUFFER, mVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions) + sizeof(vertex_colors), NULL, GL_STATIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_positions), vertex_positions);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertex_positions), sizeof(vertex_colors), vertex_colors);

				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
				glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertex_positions));
				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);

				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

				// TODO: Should only do this on resize
				glViewport(0, 0, mW, mH );
				mAspect = float(mH)/float(mW);
			});
		}

		virtual void run( )
		{
			for( int ii = 0; ii < 50000; ++ii )
			{
				render( );
				SDL_GL_SwapWindow( mMainWindow );
			}

			_hide( );
		}

};

#define IMPLEMENT_ME std::cerr << __PRETTY_FUNCTION__ << " is not implemented." << std::endl; exit( -1 );

class ModelViewProjectionShader : public SDLShader
{
	GLint mVertexPos2DLocation
            , mTexCoordLocation
	    , mTextureUnitLocation
	    , mProjectionMatrixLocation
	    , mModelViewMatrixLocation;

	glm::mat4 mProjectionMatrix, mModelViewMatrix;

	protected:
		void _setAttribute( GLint & xVar , const char * xAttribute )
	        {
	        	if ( ( xVar = glGetAttribLocation( mProgramID, xAttribute ) ) == -1 )
		        {
		        	std::cerr << "OpenGL: Failed to set location for " << xAttribute << std::endl;
		                exit( -1 );
		        }
		}

		void _setUniform( GLint & xVar , const char * xAttribute )
	        {
	        	if ( ( xVar = glGetUniformLocation( mProgramID, xAttribute ) ) == -1 )
		        {
		        	std::cerr << "OpenGL: Failed to set location for " << xAttribute << std::endl;
		                exit( -1 );
		        }
		}

	public:
		ModelViewProjectionShader( ShaderPathPair const & xPaths ) 
			: SDLShader( xPaths )
			, mVertexPos2DLocation( 0 )
    		        , mTexCoordLocation( 0 )
		        , mTextureUnitLocation( 0 )
			, mProjectionMatrixLocation( 0 )
			, mModelViewMatrixLocation( 0 )
			, mProjectionMatrix( )
			, mModelViewMatrix( )
		{
			;
		}

		// Initialize everything but color location (which we will unify soon ).. TODO
		virtual void initialize( )
		{
			SDLShader::initialize( );

			_setAttribute( mVertexPos2DLocation,    "LVertexPos2D"      );
			_setAttribute( mTexCoordLocation,       "LTexCoord"         );

			_setUniform( mTextureUnitLocation,      "LTextureUnit"      );
			_setUniform( mProjectionMatrixLocation, "LProjectionMatrix" );
			_setUniform( mModelViewMatrixLocation,  "LModelViewMatrix"  );
		}

		void setVertexPointer( GLsizei const xStride, const GLvoid* xData )
		{
			glVertexAttribPointer( mVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, xStride, xData );
		}

		void setTexCoordPointer( GLsizei const xStride, const GLvoid* xData )
		{
			glVertexAttribPointer( mTexCoordLocation, 2, GL_FLOAT, GL_FALSE, xStride, xData );
		}

		void enableVertexPointer( )
		{
			glEnableVertexAttribArray( mVertexPos2DLocation );
		}

		void disableVertexPointer( )
		{
			glDisableVertexAttribArray( mVertexPos2DLocation );
		}

		void enableTexCoordPointer( )
		{
			glEnableVertexAttribArray( mTexCoordLocation );
		}

		void disableTexCoordPointer( )
		{
			glDisableVertexAttribArray( mTexCoordLocation );
		}

		void setProjection( glm::mat4 const & xMatrix )
		{
			mProjectionMatrix = xMatrix;
		}

		void setModelView( glm::mat4 const & xMatrix )
		{
			mModelViewMatrix = xMatrix;
		}

		void leftMultProjection( glm::mat4 const & xMatrix )
		{
			mProjectionMatrix = mProjectionMatrix * xMatrix;
		}

		void leftMultModelView( glm::mat4 const & xMatrix )
		{
			mModelViewMatrix = mModelViewMatrix * xMatrix;
		}

		void updateProjection( )
		{
			glUniformMatrix4fv( mProjectionMatrixLocation, 1, GL_FALSE, glm::value_ptr( mProjectionMatrix ) );
		}

		void updateModelView( )
		{
			glUniformMatrix4fv( mModelViewMatrixLocation, 1, GL_FALSE, glm::value_ptr( mModelViewMatrix ) );
		}

		void setTextureUnit( GLuint const xUnit )
		{
			glUniform1i( mTextureUnitLocation, xUnit );
		}
};


class SDLTexturedPolygonShader : public ModelViewProjectionShader
{
	GLint mColorLocation;

	public:
		SDLTexturedPolygonShader( )
			: ModelViewProjectionShader( ShaderPathPair( "resources/shaders/LTexturedPolygonProgram2D.glvs"
                                                                   , "resources/shaders/LTexturedPolygonProgram2D.glfs" ) )
			, mColorLocation( 0 )
		{
			;
		}

		virtual void initialize( )
		{
			ModelViewProjectionShader::initialize( );
			_setUniform( mColorLocation, "LTextureColor" ); // TODO: unify into LColor
		}

		void setTextureColor( ColorRGBA const xColor )
		{
			glUniform4f( mColorLocation, xColor.r, xColor.g, xColor.b, xColor.a );
		}
};

class SDLFontShader : public ModelViewProjectionShader
{
	GLint mColorLocation;

	public:
		SDLFontShader( )
			: ModelViewProjectionShader( ShaderPathPair( "resources/shaders/LFontProgram2D.glvs"
                                                                   , "resources/shaders/LFontProgram2D.glfs" ) )
			, mColorLocation( 0 )
		{
			;
		}

		virtual void initialize(  )
		{
			ModelViewProjectionShader::initialize( );
			_setUniform( mColorLocation, "LTextColor" );
		}

		void setTextColor( ColorRGBA const xColor )
		{
			glUniform4f( mColorLocation, xColor.r, xColor.g, xColor.b, xColor.a );
		}
};


// TODO: Make this class scalable for N shaders in a pipeline for scenes, lighting, etc!
class SDLTexturedFontShader : public SDLTestWindow
{
	SDLTexturedPolygonShader mTexturedQuadProgram;
	SDLFontShader mFontProgram;

	ColorRGBA mImageColor, mTextColor;

	Rect mScreenArea;

	Texture mOpenGLTexture;

	GLuint * mPixels;

	Font mFont;

	void _freePixels( )
	{
		if ( mPixels != NULL )
		{
			delete[] mPixels;
		}
	}

	protected:
		virtual void render( )
		{
			glClear( GL_COLOR_BUFFER_BIT );

			mTexturedQuadProgram([&]( )
			{
				mTexturedQuadProgram.setModelView( glm::mat4( ) );
				mTexturedQuadProgram.setTextureColor( mImageColor );
				mOpenGLTexture.render( ( mW - mOpenGLTexture.getImageWidth( ) ) / 2.0f
							, ( mH - mOpenGLTexture.getImageHeight( ) ) / 2.0f
							, NULL
							, &mTexturedQuadProgram  );

				_checkForGLError( );

			});

			mFontProgram([&]( )
			{
				mFontProgram.setModelView( glm::mat4( ) );
				mFontProgram.setTextColor( mTextColor );

				const char * message = "Godzilla";

				mFont.render( 0.0f, 0.0f, message, &mFontProgram, &mScreenArea, eFontTextAlignment_Centered_H | eFontTextAlignment_Centered_V );

				_checkForGLError( );
			});
		}

	public:
		SDLTexturedFontShader( )
			: SDLTestWindow( )
			, mTexturedQuadProgram( )
			, mFontProgram( )
			, mImageColor( { 0.5f, 0.5f, 0.5f, 1.0f } )
			, mTextColor( { 1.0f, 0.5f, 0.5f, 1.0f } )
			, mScreenArea( )
			, mFont ( )
			, mPixels( NULL )
		{
			;
		}

		~SDLTexturedFontShader( )
		{
			if ( mPixels != NULL )
			{
				delete[] mPixels;
			}
		}

		virtual void initialize( OpenGLAttributes const & xAttributes )
		{
			SDLTestWindow::initialize( xAttributes );

			mScreenArea = { 0.0f, 0.0f, (GLfloat)mW, (GLfloat)mH };

			_freePixels( );

			GLuint const size = mW * mH;

			mPixels = new GLuint[ size ];
			memset( mPixels, 0, size * sizeof(GLuint) );

			for (int ii = 0; ii < size; ++ii )
			{
				mPixels[ ii ] = rand( ) % std::numeric_limits<GLuint>::max( );
			}

			mOpenGLTexture.fromMemoryRGBA32( mPixels, mW, mH, mW, mH );

			mFont.loadFreeType( "resources/lazy.ttf", 60 );

			glViewport( 0.0f, 0.0f, mW, mH );

			glClearColor( 0.0f, 1.0f, 1.0f, 1.0f );

			glEnable( GL_TEXTURE_2D );

			glEnable( GL_BLEND );
			glDisable( GL_DEPTH_TEST );
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			_checkForGLError( );

			mTexturedQuadProgram.initialize( );

			mFontProgram.initialize( );

			_checkForGLError( );

			auto initializeOrtho = [&]( ModelViewProjectionShader * const xShader )
			{
				xShader->setProjection( glm::ortho<GLfloat>( 0.0, mW, mH, 0.0, 1.0, -1.0 ) );
				xShader->updateProjection( );

				xShader->setModelView( glm::mat4( ) );
				xShader->updateModelView( );

				xShader->setTextureUnit( 0 );

				_checkForGLError( );

			};

			mTexturedQuadProgram([&]( )
			{
				initializeOrtho( static_cast<ModelViewProjectionShader*>( &mTexturedQuadProgram ) );
			});

			mFontProgram([&]( )
			{
				initializeOrtho( static_cast<ModelViewProjectionShader*>( &mFontProgram ) );
			});
		}

		virtual void run( )
		{

			for( int ii = 0; ii < 50000; ++ii )
			{
				render( );
				SDL_GL_SwapWindow( mMainWindow );
			}
		}
};

} // namespace sisu

#endif // SDL_SHADER_TEST_DBB12D8670054D2E97E6A436C1F0EC51_HPP_
