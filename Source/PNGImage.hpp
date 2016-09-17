#ifndef PNG_IMAGE_3BABDBF0B4DE4E7988A0C00E7A17CA13_HPP_
#define PNG_IMAGE_3BABDBF0B4DE4E7988A0C00E7A17CA13_HPP_

#include <png.h>

// C style IO is necessary to talk to libpng. This could be seen as a wrapper between stl-style c++ and libpng
#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <fstream>
#include <math.h>

namespace sisu
{
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
		
		png_byte mColorType, mBitDepth;

		png_bytep * mRGBBuffer;

		char const * const mFilename;

		png_structp mPNGRead, mPNGWrite;
		
		png_infop mInfoRead, mInfoWrite;

		_memEncode * mState;

		void _blitToOutputBuffer( );

		protected:
			void _writeDataToStream( std::ofstream & xOfs );

		public:
			PNGImage( const char * xPath );		
			~PNGImage( );

			PNGImage & operator( ) ( std::function< void( PNGImage & )> xLambda );

			void perPixel( std::function< void( PNGPixel ) > xPixel );
		
			uint32_t    		getWidth( )     const;
			uint32_t    		getHeight( )    const;
			png_byte    		getColorType( ) const; 
			png_byte    		getBitDepth( )  const;
			png_bytep * 		getRGBBuffer( ) const;
			char const * const 	getFilename( )  const;
			size_t 			getRowsSize( )  const;
			
			friend std::ofstream & operator<<( std::ofstream& xOfs, PNGImage & xImage );
	};

std::ofstream & operator<<( std::ofstream& xOfs, PNGImage & xImage);

} // namespace sisu
#endif // PNG_IMAGE_3BABDBF0B4DE4E7988A0C00E7A17CA13_HPP_
