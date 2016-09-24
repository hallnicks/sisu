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

		void operator( )( std::function<void(void)> xLambda )
		{
			glBindTexture( GL_TEXTURE_2D, mID );
			xLambda( );
			glBindTexture( GL_TEXTURE_2D, 0 );
		}
};

} // namespace sisu


#endif // TEXTURE_2D_085ACBB7992040A29AD529CB44554BA3_HPP_

