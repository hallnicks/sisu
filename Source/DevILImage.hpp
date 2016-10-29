#ifndef DEVIL_IMAGE_B210FA201E65486D986606D8219DAB8A_HPP_
#define DEVIL_IMAGE_B210FA201E65486D986606D8219DAB8A_HPP_

#include <IL/il.h>
#ifdef OPENGLES
#include <GLES2/gl2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#include "ioassist.hpp"
#include "AndroidLogWrapper.hpp"

namespace sisu
{
	class DevILImage
	{
		uint32_t mWidth, mHeight;
		ILuint mImgID;

		void _loadImageFromMemory( uint8_t * xData, size_t const xSize )
		{
			ilGenImages( 1, &mImgID );
			ilBindImage( mImgID );

			// Since we have only built libpng, assume png for now, parameterize later
			ilLoadL( IL_PNG, (char*)xData, xSize );

			mWidth  = ilGetInteger( IL_IMAGE_WIDTH  );
			mHeight = ilGetInteger( IL_IMAGE_HEIGHT );

			ilBindImage( 0 );
		}

		public:
			DevILImage( const char * xPath )
				: mWidth( 0 )
				, mHeight( 0 )
				, mImgID( 0 )
			{
#if 1
				std::vector<uint8_t> buff = fileToMemory<uint8_t>( xPath );

				_loadImageFromMemory( buff.data( ), buff.size( ) );
#else
				ilGenImages( 1, &mImgID );
				ilBindImage( mImgID );
				ilLoadImage( xPath );

				mWidth  = ilGetInteger( IL_IMAGE_WIDTH  );
				mHeight = ilGetInteger( IL_IMAGE_HEIGHT );

				ilBindImage( 0 );
#endif

			}
			DevILImage( uint8_t * xData, size_t const xSize )
				: mWidth( 0 )
				, mHeight( 0 )
				, mImgID( 0 )
			{
				_loadImageFromMemory( xData, xSize );
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
#endif // DEVIL_IMAGE_B210FA201E65486D986606D8219DAB8A_HPP_
