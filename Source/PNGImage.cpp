#include "PNGImage.hpp"

#include <stdio.h>

#include <iostream>

namespace sisu
{

_memEncode::_memEncode( ) 
	: buffer( NULL )
	, size( 0 ) 
{ 	
	;
}

_memEncode::~_memEncode( )
{
	if ( buffer != NULL ) 
	{
		free( buffer ); 
	}
}
	
// stub for debugging and api compatibility
void _png_flush_memory( png_structp xPngPointer ) 
{ 
	_memEncode * p = (_memEncode*)png_get_io_ptr( xPngPointer );
	delete p;
	p = new _memEncode( );
}

void _png_read_memory( png_structp xPngPointer, png_bytep xData, png_size_t xLength )
{
	_memEncode * p = (_memEncode*)png_get_io_ptr( xPngPointer );

	if ( p->buffer == NULL ) 
	{
		std::cerr << "_png_read_memory: Source buffer was NULL." << std::endl;
		exit( -1 ); 
	}

	// Copy data from parameterized buffer into xData?
	std::cout << "copy " << p->size << " bytes from " << std::hex << p->buffer 
					<< " to " << std::hex << xData << std::endl;

	if ( xData == NULL )
	{ 
		std::cerr << "Target buffer was NULL." << std::endl;
		exit( -1 );
	}

	memcpy( xData, p->buffer, p->size );
}

void _png_write_memory( png_structp xPngPointer, png_bytep xData, png_size_t xLength )
{
	_memEncode * p = (_memEncode*)png_get_io_ptr( xPngPointer );

	size_t nsize = p->size + xLength;

	if ( p->buffer != NULL )
	{
		p->buffer = ( char* )realloc( p->buffer, nsize );
	}
	else 
	{
		p->buffer = ( char* )malloc ( nsize ); 
	}
	
	if ( p->buffer == NULL ) 
	{
		std::cerr << "Memory allocation failure in _png_write_memory " << std::endl;
		exit( -1 ); 
	}
	
	memcpy( p->buffer + p->size, xData, xLength );
	
	p->size += xLength;
}

void PNGImage::_blitToOutputBuffer( )
{
	// Reset state of our output buffer
	if ( mWriteState != NULL )
	{
		delete mWriteState;
		mWriteState = new _memEncode( );
	}

	if ( mPNGWrite == NULL ) 
	{
		mPNGWrite = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL ); 
	}

	if ( mPNGWrite == NULL ) 
	{
		std::cerr << "png_create_write_struct failed." << std::endl;
		exit( -1 );
	}

	if ( mInfoWrite == NULL ) 
	{
		mInfoWrite = png_create_info_struct( mPNGWrite );
	}

	if ( mInfoWrite == NULL ) 
	{
		std::cerr << "png_create_info_struct failed." << std::endl;
		exit( -1 );
	}

	if ( setjmp( png_jmpbuf( mPNGWrite ) ) )
	{
		std::cerr << "setjmp( png_jmpbuf( .. ) ) failed." << std::endl;
		exit( -1 );
	}

	png_init_io( mPNGWrite, NULL );

	png_set_IHDR( mPNGWrite
			, mInfoWrite
			, mWidth
			, mHeight
			, 8
			, PNG_COLOR_TYPE_RGBA
			, PNG_INTERLACE_NONE
			, PNG_COMPRESSION_TYPE_DEFAULT
			, PNG_FILTER_TYPE_DEFAULT );

			
	png_set_write_fn( mPNGWrite, mWriteState, _png_write_memory, _png_flush_memory ); 

	png_write_info( mPNGWrite, mInfoWrite ); 

	png_write_image( mPNGWrite, mRGBBuffer ); // Here we may check if mRGBBuffer changed.
			
	png_write_end( mPNGWrite, NULL );

	if ( mPNGWrite != NULL ) 
	{
		png_destroy_write_struct( &mPNGWrite, (png_infopp)NULL );
		mPNGWrite = NULL; 
	}

}

void PNGImage::_writeDataToStream( std::ofstream & xOfs )
{
	_blitToOutputBuffer( ); 

	if ( mWriteState == NULL || mWriteState->buffer == NULL ) 
	{
		std::cerr << "Buffer was NULL, cannot write." << std::endl;
		exit( -1 );
	}

	xOfs.write( &mWriteState->buffer[0], mWriteState->size ); 
}

void PNGImage::_populateReadStructures( )
{
	mWidth      = png_get_image_width( mPNGRead, mInfoRead );

	mHeight     = png_get_image_height( mPNGRead, mInfoRead );

	if ( ( mBitDepth = png_get_bit_depth( mPNGRead, mInfoRead ) ) == 16 )
	{
		png_set_strip_16( mPNGRead ); 
	}

	mColorType  = png_get_color_type( mPNGRead, mInfoRead );

	if ( mColorType == PNG_COLOR_TYPE_PALETTE )
	{
		png_set_palette_to_rgb( mPNGRead );
	}
	else if ( mColorType == PNG_COLOR_TYPE_GRAY && mBitDepth < 8 )
	{
		png_set_expand_gray_1_2_4_to_8( mPNGRead );
	}

	if ( png_get_valid( mPNGRead, mInfoRead, PNG_INFO_tRNS ) )
	{
		png_set_tRNS_to_alpha( mPNGRead );
	}				
				
	if( mColorType == PNG_COLOR_TYPE_RGB  ||
	    mColorType == PNG_COLOR_TYPE_GRAY ||
     	    mColorType == PNG_COLOR_TYPE_PALETTE )
	{
		png_set_filler( mPNGRead, 0xFF, PNG_FILLER_AFTER );
	}

	if( mColorType == PNG_COLOR_TYPE_GRAY ||
	    mColorType == PNG_COLOR_TYPE_GRAY_ALPHA )
	{
		png_set_gray_to_rgb( mPNGRead );
	}
}

void PNGImage::_initializeReadStructures( )
{
	mPNGRead  = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL ); 

	if ( mPNGRead == NULL ) 
	{
		std::cerr << "png_create_read_struct failed." << std::endl;
		exit( -1 );
	}

	mInfoRead  = png_create_info_struct( mPNGRead );
			
	if ( mInfoRead == NULL ) 
	{
		std::cerr << "png_create_info_struct failed." << std::endl;
		exit( -1 );
	}

	if ( setjmp( png_jmpbuf( mPNGRead ) ) )
	{
		std::cerr << "setjmp( png_jmpbuf( .. ) ) failed." << std::endl;
		exit( -1 );
	}
}

void PNGImage::_initializeObject( FILE * xFile )
{	
	png_read_info( mPNGRead, mInfoRead );

	_populateReadStructures( );

	png_read_update_info( mPNGRead, mInfoRead );

	_allocateRGBBuffer( png_get_rowbytes( mPNGRead, mInfoRead ) );

	png_read_image( mPNGRead, mRGBBuffer );

	if ( xFile != NULL )
	{
		fclose( xFile );
	}
}

void PNGImage::_allocateRGBBuffer( png_uint_32 const xRowBytes )
{
	mRGBBuffer = ( png_bytep * ) malloc( getRowsSize( ) );

	for( uint32_t y = 0; y < mHeight; y++ ) 
	{
		mRGBBuffer[ y ] = ( png_byte* )malloc( xRowBytes );
	}
}

#define CTOR_ARGS(xPath)\
	: mWidth( 0 )\
	, mHeight( 0 )\
	, mColorType( 0 )\
	, mBitDepth( 0 )\
	, mRGBBuffer( NULL )\
	, mFilename( xPath )\
	, mPNGRead( NULL )\
	, mInfoRead( NULL )\
	, mPNGWrite( NULL )\
	, mInfoWrite( NULL )\
	, mWriteState( new _memEncode( ) )\
	, mReadState( new _memEncode( ) )\

PNGImage::PNGImage( _PNGImageDimensions const & xDimensions )
	CTOR_ARGS("")
{
	mWidth  = xDimensions.w; 
	mHeight = xDimensions.h;

	_allocateRGBBuffer( xDimensions.w * 4 );

	perPixel( [&]( PNGPixel xPixel )
        {
        	xPixel.data[0] = 
                xPixel.data[1] = 
                xPixel.data[2] =
                xPixel.data[3] = 0;
        });
}

PNGImage::PNGImage( const char * xPath ) 
	CTOR_ARGS(xPath)
{
	FILE * fp = fopen( xPath , "rb" );
	
	if ( fp == NULL ) 
	{
		std::cerr << "File " << xPath << " does not exist." << std::endl;
		exit( -1 );
	}

	_initializeReadStructures( );

	png_init_io( mPNGRead, fp );

	_initializeObject( fp ); 
}
	
PNGImage::~PNGImage( ) 
{
	if ( mRGBBuffer != NULL ) 
	{
		for ( uint32_t y = 0; y < mHeight; y++ )
		{
			free( mRGBBuffer[ y ] );
		}

		free( mRGBBuffer );
		
		mRGBBuffer = NULL;
	}
		
	// TODO: fix leak! :( 
	//png_destroy_info_struct( &mInfo, (png_infopp)NULL );
		
	// destroy structs as they are no longer needed
	if ( mPNGRead != NULL ) 
	{
		png_destroy_read_struct( &mPNGRead, (png_infopp)NULL, (png_infopp)NULL );
		mPNGRead = NULL; 
	}

	delete mWriteState;
	delete mReadState;
}

PNGImage & PNGImage::operator( ) ( std::function< void( PNGImage & )> xLambda )
{
	xLambda( *this );
	return *this;
}

void PNGImage::perPixel( std::function< void( PNGPixel ) > xPixel )
{
	for( uint32_t y = 0; y < mHeight; y++ ) 
	{
  		png_bytep row = mRGBBuffer[ y ];
					
		for( uint32_t x = 0; x < mWidth; x++ ) 
		{
			png_bytep px = &( row[ x * 4 ] );
			xPixel( { x, y, px } ); 
		}
	}
}
		
uint32_t    		PNGImage::getWidth( )     const { return mWidth;     } 
uint32_t    		PNGImage::getHeight( )    const { return mHeight;    }
png_byte    		PNGImage::getColorType( ) const { return mColorType; } 
png_byte    		PNGImage::getBitDepth( )  const { return mBitDepth;  }
png_bytep * 		PNGImage::getRGBBuffer( ) const { return &mRGBBuffer[0]; }
char const * const 	PNGImage::getFilename( )  const { return mFilename;  }
size_t 			PNGImage::getRowsSize( )  const { return sizeof( png_bytep ) * mHeight; }

std::ofstream & operator<<( std::ofstream& xOfs, PNGImage & xImage)
{
        xImage._writeDataToStream( xOfs );
        return xOfs;
}

} // namespace sisu
