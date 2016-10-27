#ifndef PNG_IMAGE_3BABDBF0B4DE4E7988A0C00E7A17CA13_HPP_
#define PNG_IMAGE_3BABDBF0B4DE4E7988A0C00E7A17CA13_HPP_

#include <IL/il.h>

#ifdef OPENGLES
#include <GLES2/gl2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#include "AndroidLogWrapper.hpp"

namespace sisu
{
	class DevILImage
	{
		uint32_t mWidth, mHeight;
		ILuint mImgID;

		public:
			DevILImage( const char * xPath )
				: mWidth( 0 )
				, mHeight( 0 )
				, mImgID( 0 )
			{
				ilGenImages( 1, &mImgID );
				ilBindImage( mImgID );
				ilLoadImage( xPath );

				mWidth  = ilGetInteger( IL_IMAGE_WIDTH  );
				mHeight = ilGetInteger( IL_IMAGE_HEIGHT );

				ilBindImage( 0 );
			}
			DevILImage( uint8_t * xData, size_t const xSize )
				: mWidth( 0 )
				, mHeight( 0 )
				, mImgID( 0 )
			{
				ilGenImages( 1, &mImgID );
				ilBindImage( mImgID );
				//ilLoadImage( xPath );
				// assume png for now, parameterize later
				ilLoadL( IL_PNG, (char*)xData, xSize );

				mWidth  = ilGetInteger( IL_IMAGE_WIDTH  );
				mHeight = ilGetInteger( IL_IMAGE_HEIGHT );

				std::cout << "mWidth  = " << mWidth  << std::endl;
				std::cout << "mHeight = " << mHeight << std::endl;

				ilBindImage( 0 );
			}

			~DevILImage( )
			{
				if ( mImgID != 0 )
				{
					ilBindImage( 0 );
					ilDeleteImage( mImgID );
				}
			}

			uint32_t getWidth( )  const { return mWidth;  }
			uint32_t getHeight( ) const { return mHeight; }

			// https://gist.github.com/mortennobel/5299151
			GLubyte * toGLTextureBuffer( )
			{
				ilBindImage( mImgID );

				if (! ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) )
				{
					SISULOG("ilConvertImage( .. ) failed." );
					exit( -1 );
				}

				GLubyte * data = (GLubyte*)ilGetData( );

				ilBindImage( 0 );

				return data;
			}

	};
} // namespace sisu
#endif // PNG_IMAGE_3BABDBF0B4DE4E7988A0C00E7A17CA13_HPP_
