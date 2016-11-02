#ifndef TEXTURE_2D_085ACBB7992040A29AD529CB44554BA3_HPP_
#define TEXTURE_2D_085ACBB7992040A29AD529CB44554BA3_HPP_

#include <functional>
#include <string>
#include <assimp/types.h>

#include "AndroidLogWrapper.hpp"

namespace sisu {

enum eTexture2DType
{
	eTexture2DType_Diffuse,
	eTexture2DType_Specular,
	eTexture2DType_Normal,
	eTexture2DType_Height,
};

class Texture2D
{
	GLuint mID
	     , mWidth
	     , mHeight
	     , mInternalFormat
	     , mImageFormat
	     , mWrapS
	     , mWrapT
	     , mFilterMin
	     , mFilterMax;

	bool mInitialized;

	uint8_t * mData;

	aiString mIdentifier;

	eTexture2DType mTextureType;

	public:
		Texture2D( eTexture2DType const xTextureType    = eTexture2DType_Diffuse
			 , GLenum 	  const xInternalFormat = GL_RGBA
			 , GLenum 	  const xFilter 	= GL_LINEAR
			 , GLenum 	  const xWrap 		= GL_REPEAT )
			: mID( 0 )
			, mWidth( 0 )
			, mHeight( 0 )
			, mInternalFormat( xInternalFormat )
			, mImageFormat( xInternalFormat )
			, mWrapS( xWrap )
			, mWrapT( xWrap )
			, mFilterMin( xFilter )
			, mFilterMax( xFilter )
			, mInitialized( false )
			, mData( NULL )
			, mTextureType( xTextureType )
			, mIdentifier( )
		{
			;
		}

		~Texture2D( )
		{
			if ( mInitialized )
			{
				glDeleteTextures( 1, &mID );
			}
		}

		void initialize( GLuint const xWidth
			       , GLuint const xHeight
			       , uint8_t * const xData
			       , aiString const & xIdentifier = aiString( ) )
		{
			mWidth  = xWidth;
			mHeight = xHeight;
			mData = xData;

#ifdef OPENGLES
			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
#endif

			glGenTextures( 1, &mID );

			( *this )( [ & ] ( ) {

				glTexImage2D( GL_TEXTURE_2D
					     , 0
					     , mInternalFormat
					     , mWidth
					     , mHeight
					     , 0
					     , mImageFormat
					     , GL_UNSIGNED_BYTE
					     , xData );

// TODO: Make this configurable by parameter object ..
#if 1
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrapS );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrapT );
#else
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
#endif

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mFilterMin );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mFilterMax );

				glGenerateMipmap( GL_TEXTURE_2D );
			} );

			mIdentifier = xIdentifier;
		}

		class Texture2DRow
		{
			uint8_t * mRow;

			Texture2DRow( uint8_t * xRow ) : mRow( xRow ) { }

			friend class Texture2D;

			public:
				uint8_t* operator [ ] ( size_t const xX ) { return &mRow[ xX ]; }
		};

		// TODO: Add Update texture using PBOs (optionally) and glTexSubImage.
		Texture2DRow operator [ ] ( size_t const xY ) { return Texture2DRow( mData + ( mWidth * xY ) ); }

		void operator( )( std::function<void(void)> xLambda )
		{
			bind( );
			xLambda( );
		}

		void bind( )
		{
			glBindTexture( GL_TEXTURE_2D, mID );
		}

		static void unbind( )
		{
			glBindTexture( GL_TEXTURE_2D, 0 );

		}

		GLuint 	         getWidth( )      const { return mWidth;       }
		GLuint 	         getHeight( )     const { return mHeight;      }
		eTexture2DType 	 getType( )       const { return mTextureType; }
		aiString const & getIdentifier( ) const { return mIdentifier;  }
};

} // namespace sisu


#endif // TEXTURE_2D_085ACBB7992040A29AD529CB44554BA3_HPP_

