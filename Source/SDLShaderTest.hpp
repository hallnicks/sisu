#ifndef SDL_SHADER_TEST_DBB12D8670054D2E97E6A436C1F0EC51_HPP_
#define SDL_SHADER_TEST_DBB12D8670054D2E97E6A436C1F0EC51_HPP_

#include "SDLTest.hpp"
#include "ioassist.hpp"
#include "ColorRGBA.hpp"
#include "Texture.hpp"
#include "Rect.hpp"
#include "Font.hpp"

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
	std::string mVertexShaderSource, mFragmentShaderSource;

	protected:
		GLuint mProgramID;

	public:
		SDLShader( ShaderSourcePair const & xShaderSources );
		SDLShader( ShaderPathPair const & xShaderPaths );
		~SDLShader( );

	virtual void initialize( );

	virtual SDLShader & operator( )( std::function<void(void)> xLambda );

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

// Renders a quad. Result of following LazyFoo's tut.
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
