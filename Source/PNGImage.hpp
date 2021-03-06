#ifndef PNG_IMAGE_3BABDBF0B4DE4E7988A0C00E7A17CA13_HPP_
#define PNG_IMAGE_3BABDBF0B4DE4E7988A0C00E7A17CA13_HPP_

#include <png.h>

// C style IO is necessary to talk to libpng. This could be seen as a wrapper between stl-style c++ and libpng
#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <fstream>
#include <math.h>

#ifdef OPENGLES
#include <GLES2/gl2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#ifdef __linux__
#include <cstring>
using std::memcpy;
#endif

#include "AndroidLogWrapper.hpp"

namespace sisu {
	struct _PNGImageDimensions
	{
		uint32_t w;
		uint32_t h;
	};

	class _PNGImageRow
	{
		png_bytep mRow;

		public:
			_PNGImageRow( png_bytep xRow )
				: mRow( xRow )
			{
				if ( mRow == NULL )
				{
					SISULOG( "NULL PNG image row detected." );
					exit( -1 );
				}
			}

			png_bytep operator[]( size_t const xIndex )
			{
				return &( mRow[ xIndex * 4 ] );
			}
	};

	struct _memEncode
	{
		_memEncode( );
		~_memEncode( );

		char * buffer;
		png_size_t size;
	};

	// stub for debugging and api compatibility
	void _png_flush_memory( png_structp xPngPointer );

	//unsigned char*, long long unsigned int
	void _png_write_memory( png_structp xPngPointer, png_bytep xData, png_size_t xLength );

	struct PNGPixel
	{
		uint32_t x, y;
		png_bytep data;
	};

	class PNGImage
	{
		uint32_t mWidth, mHeight;

		bool mIsValid;

		png_byte mColorType, mBitDepth;

		png_bytep * mRGBBuffer;

		char const * const mFilename;

		png_structp mPNGRead, mPNGWrite;

		png_infop mInfoRead, mInfoWrite;

		_memEncode * mWriteState, * mReadState;

		void _blitToOutputBuffer( );

		void _initializeReadStructures( );
		void _populateReadStructures( );
		void _allocateRGBBuffer( png_uint_32 const xRowBytes );
		void _initializeObject( FILE * xFile = NULL );

		void _checkWriteData( ) const
		{
			if ( mWriteState == NULL || mWriteState->buffer == NULL )
			{
				SISULOG( "Write data requested be fore blit data present" );
				exit( -1 );
			}
		}


		protected:
			void _writeDataToStream( std::ofstream & xOfs );

		public:
			PNGImage( _PNGImageDimensions const & xDimensions );
			PNGImage( const char * xPath );
			~PNGImage( );

			PNGImage & operator( ) ( std::function< void( PNGImage & )> xLambda );

			void perPixel( std::function< void( PNGPixel ) > xPixel );

			bool			getIsValid( )   const { return mIsValid; }
			uint32_t    		getWidth( )     const;
			uint32_t    		getHeight( )    const;
			png_byte    		getColorType( ) const;
			png_byte    		getBitDepth( )  const;
			png_bytep * 		getRGBBuffer( ) const;
			char const * const 	getFilename( )  const;
			size_t 			getRowsSize( )  const;

			// https://gist.github.com/mortennobel/5299151
			GLubyte * toGLTextureBuffer( )
			{
#if 0
				uint32_t const rowBytes = png_get_rowbytes( mPNGRead, mInfoRead );

				std::cout << "RowBytes = " << rowBytes << std::endl;

#endif
				size_t const rowBytes = mWidth * sizeof(uint8_t)*4;

				GLubyte* outData = (GLubyte* )malloc( rowBytes * mHeight );

				for ( uint32_t ii = 0; ii < mHeight; ii++ )
				{
					memcpy( outData + ( rowBytes * ii )
					      , mRGBBuffer[ ii ]
					      , rowBytes );
				}

				return outData; // must be 'FREED by the caller some time.
			}

			friend std::ofstream & operator<<( std::ofstream& xOfs, PNGImage & xImage );

			_PNGImageRow operator[]( size_t const xIndex )
			{
				if ( xIndex >= mHeight )
				{
					SISULOG( "Invalid index  passed to PNGimage::operator[] " );
					exit( -1 );
				}

		                png_bytep row = mRGBBuffer[ xIndex ];

				return _PNGImageRow( row );
			}


			void blit( ) { _blitToOutputBuffer( ); }

			uint8_t * getData( ) const
			{
				_checkWriteData( );
				return (uint8_t*)&mWriteState->buffer[0];
			}

			size_t const getDataSize( ) const
			{
				_checkWriteData( );
				return mWriteState->size;
			}

	};

std::ofstream & operator<<( std::ofstream& xOfs, PNGImage & xImage);

} // namespace sisu
#endif // PNG_IMAGE_3BABDBF0B4DE4E7988A0C00E7A17CA13_HPP_
