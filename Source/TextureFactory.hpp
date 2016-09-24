#ifndef TEXTURE_FACTORY_BE8B7F3B5AD74F5A866A048E58B08332_HPP_
#define TEXTURE_FACTORY_BE8B7F3B5AD74F5A866A048E58B08332_HPP_

#if (defined(BYTE_ORDER) && BYTE_ORDER==BIG_ENDIAN) || \
        (defined(__BYTE_ORDER) && __BYTE_ORDER==__BIG_ENDIAN) || \
        defined(__BIG_ENDIAN__)
#define SISU_BIG_ENDIAN
#define sENDIAN "Big Endian"
#else
#define SISU_LITTLE_ENDIAN
#define sENDIAN "Little Endian"
#endif

#include "SDLTest.hpp"

namespace sisu {

inline bool isPower2(int x)
{
           return ( (x > 0) && ((x & (x - 1)) == 0) );
}

class TextureBase
{
	void _free( )
	{
		if ( mCreated && mTextureID != 0 )
		{
			std::cout << "Free textures." << std::endl;
			glDeleteTextures( 1, &mTextureID );
		}
	}

	protected:
		bool mCreated;

		GLuint mTextureID;

		uint32_t mWidth, mHeight;

		int32_t mMode;

		GLboolean mHasMultiTexture
			, mHasFBO
			, mHasRectTexture
			, mOldRectTextureExt
			, mOldRectTextureArb
			, mOldRectTextureNv;

	public:
		TextureBase( )
			: mCreated( false )
			, mTextureID( 0 )
			, mMode( 0 )
			, mHasMultiTexture(   glewGetExtension( "GL_ARB_multitexture" 		  ) )
			, mHasRectTexture(    glewGetExtension( "GL_ARB_texture_non_power_of_two" ) )
			, mHasFBO( 	      glewGetExtension( "GL_EXT_framebuffer_object" 	  ) )
			, mOldRectTextureExt( glewGetExtension( "GL_EXT_texture_rectangle" 	  ) )
			, mOldRectTextureArb( glewGetExtension( "GL_ARB_texture_rectangle" 	  ) )
			, mOldRectTextureNv(  glewGetExtension( "GL_ARB_texture_rectangle"        ) )
		{
			;
		}

		~TextureBase( )
		{
			_free( );
		}


		void bind( int32_t const xTextureUnit )
		{
			if ( mHasMultiTexture )
			{
				glActiveTextureARB( GL_TEXTURE0_ARB + xTextureUnit );
			}

			glBindTexture( GL_TEXTURE_2D, mTextureID );
		}

		void create( uint32_t const xWidth
			   , uint32_t const xHeight
			   , GLint    const xComponents
			   , GLshort  const xFormat
			   , GLshort  const xDataType
			   , void * xData)
		{
			_free( );

			std::cout << __LINE__ << std::endl;
			_checkForGLError( );

			glGenTextures( 1, &mTextureID );

			std::cout << __LINE__ << std::endl;
			_checkForGLError( );


			glBindTexture( GL_TEXTURE_2D, mTextureID );

			std::cout << __LINE__ << std::endl;
			_checkForGLError( );

			glTexImage2D( GL_TEXTURE_2D
				    , 0
				    , xComponents
				    , xWidth
				    , xHeight
				    , 0
				    , xFormat
				    , GL_UNSIGNED_BYTE
				    , xData );

			std::cout << __LINE__ << std::endl;
			_checkForGLError( );


			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

			std::cout << __LINE__ << std::endl;
			_checkForGLError( );

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			std::cout << __LINE__ << std::endl;
			_checkForGLError( );


			mCreated = true;
		}
};

class GLTexture : public TextureBase
{

};

class TextureFactory
{
	public:
		static TextureBase * createTextureFromRandomPixels( )
		{
			TextureBase * ret = new GLTexture( );

			// TODO: Grab these values from SDL.
			size_t const size = 1920*1080*4;

			uint8_t * textureData = (uint8_t*)malloc( size );

			for ( uint32_t ii = 0; ii < size; ++ii )
			{
				textureData[ ii ] = rand( ) % 255;
			}

			ret->create( 1920, 1080, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, textureData );

			return ret;
		}
};

class RectTextureExt : public TextureBase { };
class RectTextureArb : public TextureBase { };
class RectTextureNv  : public TextureBase { };

class RenderTarget : public TextureBase
{
	GLuint mFB
	     , mColorTexture
	     , mDepthRB;

	uint32_t mWidth, mHeight;

	int32_t mMode;
};


} // namespace sisu

#endif // TEXTURE_FACTORY_BE8B7F3B5AD74F5A866A048E58B08332_HPP_

