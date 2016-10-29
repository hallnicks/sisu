#ifndef TGA_IMAGE_1EB0C7A9D45D4D029E0362F6435DD52A_HPP_
#define TGA_IMAGE_1EB0C7A9D45D4D029E0362F6435DD52A_HPP_

#include <cstdio>
#include <GLES3/gl3.h>

// This file is based strongly on Ginsburg's original c implementation. The original license
// is included below.

// The MIT License (MIT)
//
// Copyright (c) 2013 Dan Ginsburg, Budirijanto Purnomo
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

//
// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133
//

namespace sisu {

class TGAImage
{
	typedef struct
	{
	   uint8_t IdSize
		 , MapType
		 , ImageType;

	   uint16_t PaletteStart
		  , PaletteSize;

	   uint8_t  PaletteEntryDepth;
	   uint16_t X
		  , Y
	          , Width
	          , Height;

	   uint8_t ColorDepth
		 , Descriptor;

	} TGA_HEADER;

	int mW, mH;

	GLubyte * mData;

	static char * esLoadTGA ( const char *fileName, int *width, int *height )
	{
	   char        *buffer;
	   FILE       *fp;
	   TGA_HEADER   Header;
	   int          bytesRead;

	   // Open the file for reading
	   fp = fopen( fileName , "rb" );

	   if ( fp == NULL )
	   {
	      // Log error as 'error in opening the input file from apk'
	      std::cerr << "esLoadTGA FAILED to load :" << fileName  << std::endl;
	      exit( -1 );
	   }

	  // bytesRead = esFileRead ( fp, sizeof ( TGA_HEADER ), &Header );
	  bytesRead = fread( &Header, sizeof( TGA_HEADER), 1, fp );

	   *width = Header.Width;
	   *height = Header.Height;

	   if ( Header.ColorDepth == 8 ||
	         Header.ColorDepth == 24 || Header.ColorDepth == 32 )
	   {
	      int bytesToRead = sizeof ( char ) * ( *width ) * ( *height ) * Header.ColorDepth / 8;

	      // Allocate the image data buffer
	      buffer = ( char * ) malloc ( bytesToRead );

	      if ( buffer )
	      {
	         //bytesRead = esFileRead ( fp, bytesToRead, buffer );
	         bytesRead = fread( buffer, bytesToRead, 1, fp );
	         fclose ( fp );

	         return ( buffer );
	      }
	   }

	   return ( NULL );
	}


	public:
		TGAImage( const char * xPath )
		{
			mData = (GLubyte*)esLoadTGA( xPath, &mW, &mH );
		}

		~TGAImage( )
		{
			if ( mData != NULL )
			{
				free( mData );
			}
		}


		GLubyte * toGLTextureBuffer( ) const { return mData; }

		uint32_t getWidth( )  const { return (uint32_t)mW; }
		uint32_t getHeight( ) const { return (uint32_t)mH; }
};

} // namespace sis

#endif // TGA_IMAGE_1EB0C7A9D45D4D029E0362F6435DD52A_HPP_
