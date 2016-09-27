#ifndef TEXTURE_2D_085ACBB7992040A29AD529CB44554BA3_HPP_
#define TEXTURE_2D_085ACBB7992040A29AD529CB44554BA3_HPP_

#include <functional>

namespace sisu {

class Texture2D
{
	GLuint mID;

	GLuint mWidth
	     , mHeight
	     , mInternalFormat
	     , mImageFormat
	     , mWrapS
	     , mWrapT
	     , mFilterMin
	     , mFilterMax;

	bool mInitialized;

	uint8_t * mData;

	public:
		Texture2D( )
			: mID( 0 )
			, mWidth( 0 )
			, mHeight( 0 )
			, mInternalFormat( GL_RGBA )
			, mImageFormat( GL_RGBA )
			, mWrapS( GL_REPEAT )
			, mWrapT( GL_REPEAT )
			, mFilterMin( GL_LINEAR )
			, mFilterMax( GL_LINEAR )
			, mInitialized( false )
			, mData( NULL )
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
			     , uint8_t * const xData )
		{
			mWidth  = xWidth;
			mHeight = xHeight;
			mData = xData;

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

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mWrapS );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mWrapT );

				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mFilterMin );
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mFilterMax );
			} );

		}

		class Texture2DRow
		{
			uint8_t * mRow;

			Texture2DRow( uint8_t * xRow ) : mRow( xRow ) { }

			friend class Texture2D;

			public:
				uint8_t* operator [ ] ( size_t const xX ) { return &mRow[ xX ]; }
		};

		Texture2DRow operator [ ] ( size_t const xY ) { return Texture2DRow( mData + ( mWidth * xY ) ); }

		void operator( )( std::function<void(void)> xLambda )
		{
			glBindTexture( GL_TEXTURE_2D, mID );
			xLambda( );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}

		GLuint getWidth( )  const { return mWidth;  }
		GLuint getHeight( ) const { return mHeight; }
};

} // namespace sisu


#endif // TEXTURE_2D_085ACBB7992040A29AD529CB44554BA3_HPP_

