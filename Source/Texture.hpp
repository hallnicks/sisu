#ifndef TEXTURE_567B3347DC6E4C04BBA74CB142A12D99_HPP_
#define TEXTURE_567B3347DC6E4C04BBA74CB142A12D99_HPP_
#include "ioassist.hpp"
#include "TexturedVertex2D.hpp"
#include "Rect.hpp"

#include <string>
#include <iostream>
#include <functional>

#ifndef __linux__
#include <Windows.h>
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.H>
#include <GL/glu.h>
#include <stdio.h>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sisu {

class Texture
{
	struct _VertexPos2D
	{
		GLfloat x, y;
	};

	struct _TexCoord
	{
		GLfloat s, t;
	};

	struct _TexturedVertex2D
	{
		_VertexPos2D 	position;
		_TexCoord	texCoord;
	};

	void _freeVBO( )
	{
		if ( mVBOID != 0 )
		{
			glDeleteBuffers( 1, &mVBOID );
			glDeleteBuffers( 1, &mIBOID );
			mVBOID = 0;
		}
	}

	void _initVBO( )
	{
		if ( mTextureID != 0 && mVBOID == 0 ) // Texture loaded VBO not created
		{
			_TexturedVertex2D vData[ 4 ];
			GLuint iData[ 4 ];

			iData[ 0 ] = 0;
			iData[ 1 ] = 1;
			iData[ 2 ] = 2;
			iData[ 3 ] = 3;

			glGenBuffers( 1, &mVBOID );
			glBindBuffer( GL_ARRAY_BUFFER, mVBOID );
			glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(_TexturedVertex2D), vData, GL_DYNAMIC_DRAW );

			glGenBuffers( 1, &mIBOID );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBOID );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof( GLuint ), iData, GL_DYNAMIC_DRAW );

			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}
	}

	inline static GLuint _powerOfTwo( GLuint num )
	{
		if ( num != 0 )
		{
			num--;
			num |= ( num >> 1  );
			num |= ( num >> 2  );
			num |= ( num >> 4  );
			num |= ( num >> 8  );
			num |= ( num >> 16 );
			num++;
		}
		return num;
	}

	protected:
		GLuint mVBOID
		     , mIBOID
		     , mImageWidth
		     , mImageHeight
		     , mTextureWidth
		     , mTextureHeight
		     , mPixelFormat
		     , mTextureID;

		GLuint * mPixels32;
		GLubyte * mPixels8;

		void _freeTexture( )
		{
			if ( mTextureID != 0 )
			{
				glDeleteTextures( 1, &mTextureID );
				mTextureID = 0;
			}

			if ( mPixels32 != NULL )
			{
				delete[] mPixels32;
				mPixels32 = NULL;
			}

			if ( mPixels8 != NULL )
			{
				delete[] mPixels8;
				mPixels8 = NULL;
			}

			mPixelFormat   =
			mImageWidth    =
		 	mImageHeight   =
			mTextureWidth  =
			mTextureHeight = 0;
		}

		void _initializePixels8( GLuint const xImageWidth, GLuint const xImageHeight )
		{
			_freeTexture( );

			GLuint const size = xImageWidth * xImageHeight;

			mPixels8 = new GLubyte [ size ];

			mTextureHeight = mImageHeight = xImageHeight;
			mTextureWidth  = mImageWidth  = xImageWidth;

			mPixelFormat = GL_RED;
		}
	public:
		Texture( )
			: mVBOID( 0 )
			, mIBOID( 0 )
			, mImageWidth( 0 ) 
			, mImageHeight( 0 )
			, mTextureWidth( 0 )
			, mTextureHeight( 0 )
			, mPixelFormat( 0 )
			, mTextureID( 0 )
			, mPixels32( NULL )
			, mPixels8( NULL )
		{
			;
		}

		~Texture( )
		{
			_freeTexture( );
			_freeVBO( );
		}


		template <typename XImage>
		void fromImage( XImage & xImage )
		{
			GLuint const w = xImage.getWidth( );
			GLuint const h = xImage.getHeight( );

			GLuint const texW = _powerOfTwo( w );
			GLuint const texH = _powerOfTwo( h );

			std::cerr << __PRETTY_FUNCTION__ << ": implement me " << std::endl;
			exit( -1 ); 
		}

		template< typename XBufferType>
		void blit( XBufferType * xDestinationPixels, XBufferType * xSourcePixels, GLuint const xX, GLuint const xY,  Texture & xDestination )
		{
			XBufferType * d = xDestinationPixels;
			XBufferType * s = xSourcePixels;

			for ( int32_t ii = 0; ii < mImageHeight; ++ii )
			{
				memcpy( &d[ ( ii + xY ) * xDestination.mTextureWidth + xX ], &s[ ii * mTextureWidth ], mImageWidth * sizeof(XBufferType) );
			}

		}

		void blit32( GLuint const xX, GLuint const xY, Texture & xDestination )
		{
			if ( mPixels32 == NULL )
			{
				std::cout << __PRETTY_FUNCTION__ << ": source buffer was null." << std::endl;
				exit( -1 );
			}

			if ( xDestination.mPixels32 == NULL )
			{
				std::cout << __PRETTY_FUNCTION__ << ": destination buffer was null." << std::endl;
				exit( -1 );
			}

			blit<GLuint>( xDestination.mPixels32, mPixels32, xX, xY, xDestination );
		}

		void blitPixels8( GLuint const xX, GLuint const xY, Texture & xDestination )
		{
			if ( mPixels8 == NULL )
			{
				std::cout << __PRETTY_FUNCTION__ << ": source buffer was null." << std::endl;
				exit( -1 );
			}

			if ( xDestination.mPixels8 == NULL )
			{
				std::cout << __PRETTY_FUNCTION__ << ": destination buffer was null." << std::endl;
				exit( -1 );
			}

			blit<GLubyte>( xDestination.mPixels8, mPixels8, xX, xY, xDestination );
		}

		void loadTextureFromPixels32( )
		{
			if ( mTextureID != 0 )
			{
				std::cerr << __PRETTY_FUNCTION__ << ": texture already loaded." << std::endl;
				exit( -1 );
			}

			if ( mPixels32 == NULL )
			{
				std::cerr << __PRETTY_FUNCTION__ << ": no data in pixels32 buffer" << std::endl;
				exit( -1 );
			}

			glGenTextures( 1, &mTextureID );

			std::cerr << __PRETTY_FUNCTION__ << ": implement me " << std::endl;

			exit( -1 );
		}

		void loadTextureFromPixels8( )
		{
			if ( mTextureID != 0 )
			{
				std::cerr << __PRETTY_FUNCTION__ << ": texture already loaded." << std::endl;
				exit( -1 );
			}

			if ( mPixels8 == NULL )
			{
				std::cerr << __PRETTY_FUNCTION__ << ": no data in pixels8 buffer" << std::endl;
				exit( -1 );
			}


			glGenTextures( 1, &mTextureID );

			glBindTexture( GL_TEXTURE_2D, mTextureID );

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, mTextureWidth, mTextureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, mPixels8 );

		        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

		        glBindTexture( GL_TEXTURE_2D, 0 );


			delete[] mPixels8;
			mPixels8 = NULL;

			_initVBO( );

			mPixelFormat = GL_RED;
		}

		void fromFileRGBA32( std::string const & xPath )
		{
			std::cout << __PRETTY_FUNCTION__ << " not implemented." << std::endl;
			exit( -1 );
		}

		void fromFileRGBA8( std::string const & xPath )
		{
			std::cout << __PRETTY_FUNCTION__ << " not implemented." << std::endl;
			exit( -1 );
		}

		void fromMemoryRGBA8( GLubyte * xPixels
				     , GLuint xImageWidth
				     , GLuint xImageHeight
				     , GLuint xTextureWidth
				     , GLuint xTextureHeight )
		{
			std::cout << __PRETTY_FUNCTION__ << " not implemented." << std::endl;
			exit( -1 );	
		}

		void copyPixels8( GLubyte * xPixels
  			        , GLuint xImageWidth
				, GLuint xImageHeight )
		{
			_initializePixels8( xImageWidth, xImageHeight );

			memcpy( mPixels8, xPixels, xImageWidth * xImageHeight );
		}

		void copyPixels32( GLuint * xPixels
				 , GLuint xImageWidth
				 , GLuint xImageHeight )
		{
			std::cout << __PRETTY_FUNCTION__ << " not implemented." << std::endl;
			exit( -1 );	
		}

		void padPixels8( )
		{
			if ( mPixels8 == NULL )
			{
				std::cout << __PRETTY_FUNCTION__ << "8-bit pixel buffer was not initialized." << std::endl;
				exit( -1 );
			}

			GLuint  oldTextureWidth  = mTextureWidth;

			mTextureWidth  = _powerOfTwo( mImageWidth );
			mTextureHeight = _powerOfTwo( mImageHeight );

			if ( mTextureWidth != mImageWidth || mTextureHeight != mImageHeight )
			{
				GLuint const size    = mTextureWidth * mTextureHeight;
				GLubyte *    pixels  = new GLubyte[ size ];

				for( int ii = 0; ii < mImageHeight; ++ii )
				{
			                memcpy( &pixels[ ii * mTextureWidth ], &mPixels8[ ii * oldTextureWidth ], mImageWidth );
				}

				delete[] mPixels8;
				mPixels8 = pixels;
			}
		}

		void createPixels8( GLuint const xWidth, GLuint const xHeight )
		{
			_initializePixels8( xWidth, xHeight );
			memset( mPixels8, 0, xWidth * xHeight );
		}

		void fromMemoryRGBA32( GLuint * xPixels
				     , GLuint xImageWidth
				     , GLuint xImageHeight
				     , GLuint xTextureWidth
				     , GLuint xTextureHeight )
		{
			_freeTexture( );

			mImageWidth    = xImageWidth;
			mImageHeight   = xImageHeight;
			mTextureWidth  = xTextureWidth;
			mTextureHeight = xTextureHeight;

			glGenTextures( 1, &mTextureID );

			glBindTexture( GL_TEXTURE_2D, mTextureID );

			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, xPixels );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

			glBindTexture( GL_TEXTURE_2D, 0 );

			GLenum const error = glGetError( );

			if ( error != GL_NO_ERROR )
			{
				std::cerr << "OpenGL: Error code " << error << " while initializing texture." << std::endl;
				exit( -1 );
			}

			std::cout << "OpengL: Initialized texture #" << mTextureID << std::endl;

			_initVBO( );

			mPixelFormat = GL_RGBA;
		}

		template< typename XRenderer > 
		void render( GLfloat const xX, GLfloat const xY, Rect * xClip, XRenderer * const xShaderProgram )
		{
			if ( mTextureID == 0 )
			{
				std::cerr << "Cannot render uninitialized texture." << std::endl;
				exit( -1 );
			}

			GLfloat texTop    = 0.0f;
			GLfloat texBottom = (GLfloat)mImageHeight / (GLfloat)mTextureHeight;
			GLfloat texLeft   = 0.0f;
			GLfloat texRight  = (GLfloat)mImageWidth / (GLfloat)mTextureWidth;

			GLfloat quadWidth  = mImageWidth;
			GLfloat quadHeight = mImageHeight;

			if ( xClip != NULL )
			{
				texLeft   =   xClip->x 		    / mTextureWidth;
				texRight  = ( xClip->x + xClip->w ) / mTextureWidth;
				texTop    =   xClip->y 		    / mTextureHeight;
				texBottom = ( xClip->y + xClip->h ) / mTextureHeight;

				quadWidth  = xClip->w;
				quadHeight = xClip->h;
			}

			xShaderProgram->leftMultModelView( glm::translate<float>( glm::mat4( ), glm::vec3( xX, xY, 0.0f ) ) );
			xShaderProgram->updateModelView( );

			TexturedVertex2D vData[ 4 ];

		        //Texture coordinates
		        vData[ 0 ].texCoord.s =  texLeft; vData[ 0 ].texCoord.t =    texTop;
		        vData[ 1 ].texCoord.s = texRight; vData[ 1 ].texCoord.t =    texTop;
		        vData[ 2 ].texCoord.s = texRight; vData[ 2 ].texCoord.t = texBottom;
		        vData[ 3 ].texCoord.s =  texLeft; vData[ 3 ].texCoord.t = texBottom;

		        //Vertex positions
		        vData[ 0 ].position.x =       0.f; vData[ 0 ].position.y =        0.f;
		        vData[ 1 ].position.x = quadWidth; vData[ 1 ].position.y =        0.f;
		        vData[ 2 ].position.x = quadWidth; vData[ 2 ].position.y = quadHeight;
		        vData[ 3 ].position.x =       0.f; vData[ 3 ].position.y = quadHeight;

			glBindTexture( GL_TEXTURE_2D, mTextureID );

			glBufferSubData( GL_ARRAY_BUFFER, 0, 4 * sizeof(TexturedVertex2D), vData );

			xShaderProgram->setTexCoordPointer( sizeof(TexturedVertex2D), (GLvoid*)offsetof( TexturedVertex2D, texCoord ) );
			xShaderProgram->setVertexPointer( sizeof(TexturedVertex2D), (GLvoid*)offsetof( TexturedVertex2D, position ) );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBOID );
			glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

			xShaderProgram->disableVertexPointer( );
			xShaderProgram->disableTexCoordPointer( );
		}

		void setPixel8( GLuint const xX, GLuint const xY, GLubyte const xPixel )
		{
			mPixels8[ xY * mTextureWidth + xX ] = xPixel;
		}

		void setPixel32( GLuint const xX, GLuint const xY, GLuint const xPixel )
		{
			mPixels32[ xY * mTextureWidth + xX ] = xPixel;
		}

		GLubyte getPixel8( GLuint const xX, GLuint const xY )
		{
			return mPixels8[ xY * mTextureWidth + xX ];   
		}

		GLubyte getPixel32( GLuint const xX, GLuint const xY )
		{
			return mPixels32[ xY * mTextureWidth + xX ];   
		}

		GLuint  * getPixelsData32( )  { return mPixels32;      }
		GLubyte * getPixelsData8( )   { return mPixels8;       }
		GLuint    getTextureID( )     { return mTextureID;     } 
		GLuint    getTextureWidth( )  { return mTextureWidth;  }
		GLuint    getTextureHeight( ) { return mTextureHeight; } 
		GLuint    getImageWidth( )    { return mImageWidth;    }
		GLuint    getImageHeight( )   { return mImageHeight;   }

};

} // namespace sisu

#endif // TEXTURE_567B3347DC6E4C04BBA74CB142A12D99_HPP_
