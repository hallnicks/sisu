#ifndef FONT_EAF969A3C426403DA705D9614D79046D_HPP_
#define FONT_EAF969A3C426403DA705D9614D79046D_HPP_

// C style IO is necessary to talk to libpng. This could be seen as a wrapper between stl-style c++ and libpng
#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <algorithm>
#include <fstream>
#include <math.h>
#include "memblock.hpp"
#include "ttyc.hpp"
#include "ioassist.hpp"

#include "PNGImage.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include "word.hpp"

namespace sisu
{

enum eFontTextAligment
{
	eFontTextAlignment_Left 	= 1,
	eFontTextAlignment_Centered_H 	= 2,
	eFontTextAlignment_Right 	= 4,
	eFontTextAlignment_Top 		= 8,
	eFontTextAlignment_Centered_V   = 16,
	eFontTextAlignment_Bottom       = 32,
};

static struct _initializeFreeType
{
        FT_Library mLibrary;

        public:
                _initializeFreeType( )
                        : mLibrary( 0 )
                {
                        FT_Error const error = FT_Init_FreeType( &mLibrary );

                        if ( error || mLibrary == 0 )
                        {
                                std::cerr << "FT_Init_FreeType( .. ) failed. " << error << std::endl;
                                exit( -1 );
                        }
                }

                ~_initializeFreeType( )
                {
                        FT_Done_FreeType( mLibrary );
                }
} sFT2;


union _FontPixel32
{
	_FontPixel32( )
		  : integer( 0 )
	{
		;
	}

	_FontPixel32( uint8_t const xB
		    , uint8_t const xG
		    , uint8_t const xR
		    , uint8_t xA = 255 )
	{
	   	b = xB;
	    	g = xG;
	    	r = xR;
		a = xA;
	}

	_FontPixel32( _FontPixel32 const & xRhs )
	{
		b = xRhs.b;
		g = xRhs.g;
		r = xRhs.r;
		a = xRhs.a;
	}

	uint32_t integer;

	struct
	{
#ifdef SISU_BIG_ENDIAN
	    uint8_t a, b, g, r;
#else // SISU_BIG_ENDIAN
	    uint8_t b, g, r, a;
#endif // SISU_BIG_ENDIAN
	};
};

class FTFont
{
	struct Span
	{
		Span( ) { }
		Span( int32_t const xX
			, int32_t const xY
			, int32_t const xWidth
			, int32_t const xCoverage )
			: x( xX )
			, y( xY )
			, width( xWidth )
			, coverage( xCoverage )
		{
			;
		}

		int x, y, width, coverage;
	};

	struct _FontVec2
	{
		_FontVec2( ) { }
		_FontVec2( float const xX, float const xY )
			: x( xX )
			, y( xY )
		{
			;
		}

		float x, y;
	};

	struct _FontRect
	{
		float xMin, xMax, yMin, yMax;

		_FontRect( float const xLeft
			 , float const xTop
			 , float const xRight
			 , float const xBottom )
			: xMin( xLeft )
			, xMax( xRight )
			, yMin( xTop )
			, yMax( xBottom )
		{
			;
		}

		void include( _FontVec2 const & xR )
		{
			xMin = std::min( xMin, xR.x );
			xMax = std::max( xMax, xR.x );
			yMin = std::min( yMin, xR.y );
			yMax = std::max( yMax, xR.y );
		}
	};

	FT_Face mFace;

	typedef std::vector<Span> Spans;

	Spans mSpans, mOutlineSpans;

	int32_t mSize;

	static void _RasterCallback( int32_t const xY
				   , int32_t const xCount
				   , FT_Span const * const xSpans
				   , void *  const xUser )
	{
		Spans * spansPointer = (Spans *) xUser;

		for ( int32_t ii = 0; ii < xCount; ++ii )
		{
			spansPointer->push_back( Span( xSpans[ ii ].x, xY, xSpans[ ii ].len, xSpans[ ii ].coverage ) );
		}
	}

	public:
		FTFont( const FT_Byte * xFontData
		      , std::streamsize const xSize )
		      : mSize( 0 )
		{

			FT_Error const error = FT_New_Memory_Face( sFT2.mLibrary
								 , xFontData
								 , xSize
								 , 0
								 , &mFace );

			if ( error == FT_Err_Unknown_File_Format )
			{
				std::cerr << "Failed to load font from memory." << std::endl;
				exit( -1 );
			}
			else if ( error )
			{
				std::cerr << "Error loading font : " << error << std::endl;
				exit( -1 );
			}
		}

		FTFont( const char * xFontPath )
			: mSize( 0 )
		{
			FT_Error const error = FT_New_Face( sFT2.mLibrary, xFontPath, 0, &mFace );

			if ( error == FT_Err_Unknown_File_Format )
			{
				std::cerr << "Failed to load font " << xFontPath << std::endl;
				exit( -1 );
			}
			else if ( error )
			{
				std::cerr << "Error loading font " << xFontPath << ": " << error << std::endl;
				exit( -1 );
			}
		}

		~FTFont( )
		{
			FT_Error const error = FT_Done_Face( mFace );

			if ( error )
			{
				std::cerr << "Error destroying font : " << error << std::endl;
				exit( -1 );
			}

		}

		void setFontSize( int32_t const xSize )
		{
			if ( FT_Set_Char_Size( mFace, xSize << 6, xSize << 6, 90, 90 ) != 0 )
			{
				std::cerr << "FT_Set_Char_Size( .. ) failed." << std::endl;
				exit( -1 );
			}

			mSize = xSize;
			// Compute other variables. Assume 72 DPI for now. 
			// TODO: Support 96 DPI later when you can test it.
		}

		void loadGlyph( FT_ULong const xCharCode, float const xOutlineWidth )
		{
			mSpans.clear( );
			mOutlineSpans.clear( );

			FT_UInt const index = FT_Get_Char_Index( mFace, xCharCode );

			FT_Error error = FT_Load_Glyph( mFace, index, FT_LOAD_NO_BITMAP );

			if ( error != 0)
			{
				std::cerr << "FT_Load_Glyph( .. ) failed: " << error << std::endl;
				exit( -1 );
			}

			if ( mFace->glyph->format != FT_GLYPH_FORMAT_OUTLINE )
			{
				// TODO: Fix ..
				std::cerr << "Glyphs without outlines are unsupported at this time." << std::endl;
				exit( -1 );
			}

			auto renderSpans = [ & ]( Spans * xSpans, FT_Outline * xO )
			{
				FT_Raster_Params params;

				memset( &params, 0, sizeof( params ) );

				params.flags 	  = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
				params.gray_spans = _RasterCallback;
				params.user	  = xSpans;

				FT_Outline_Render( sFT2.mLibrary, xO, &params);
			};

			renderSpans( &mSpans , &mFace->glyph->outline );

			class Stroker
			{
				FT_Stroker mStroker;

				public:
					Stroker( float const xOutlineWidth )
					{
						if ( FT_Stroker_New( sFT2.mLibrary, &mStroker ) != 0 )
						{
							std::cerr << "FT_Stroker_New( .. ) failed." << std::endl;
							exit( -1 );
						}

						FT_Stroker_Set( mStroker
							      , (int32_t)( xOutlineWidth * 64 )
							      , FT_STROKER_LINECAP_ROUND
							      , FT_STROKER_LINEJOIN_ROUND
							      , 0 );

					}

					~Stroker( )
					{
						FT_Stroker_Done( mStroker );
					}

					void stroke( FT_Glyph & xGlyph )
					{
						FT_Glyph_StrokeBorder( &xGlyph, mStroker, 0, 1 );

					}
			} stroker( xOutlineWidth );

			FT_Glyph glyph;

			if ( FT_Get_Glyph( mFace->glyph, &glyph ) != 0 )
			{
				std::cerr << "FT_Get_Glyph( .. ) failed." << std::endl;
				exit( -1 );
			}


			stroker.stroke( glyph );

			if ( glyph->format != FT_GLYPH_FORMAT_OUTLINE )
			{
				std::cerr << "Glyph format was not FT_GLYPH_FORMAT_OUTLINE." << std::endl;
				exit( -1 );
			}

			FT_Outline * outline = &reinterpret_cast<FT_OutlineGlyph>(glyph)->outline;

			renderSpans( &mOutlineSpans, outline );

			FT_Done_Glyph( glyph );

		}

		template < class XImageType >
		void printSpans( XImageType & xImage
			       , _FontPixel32 const & xFontColor
			       , _FontPixel32 const & xOutlineColor
			       , uint32_t const xOffset = 0
			       , uint32_t const yOffset = 0 )

		{
			if ( mSpans.empty( ) )
			{
				return;
			}

			_FontRect rect( mSpans.front( ).x
			   	      , mSpans.front( ).y
				      , mSpans.front( ).x
				      , mSpans.front( ).y );

			for ( Spans::iterator s = mSpans.begin( ); s != mSpans.end( ); ++s )
			{
				rect.include( _FontVec2( s->x, 		 s->y ) );
				rect.include( _FontVec2( s->x + s->width - 1, s->y ) );
			}

			for ( Spans::iterator s = mOutlineSpans.begin( ); s != mOutlineSpans.end( ); ++s )
			{
				rect.include( _FontVec2( s->x,               s->y ) );
				rect.include( _FontVec2( s->x + s->width -1, s->y ) );
			}

			// shift advance goes here -- TODO
			//std::cout << "Shift advance goes here." << std::endl;

	  	       	float const bearingX = mFace->glyph->metrics.horiBearingX >> 6;
            		float const bearingY = mFace->glyph->metrics.horiBearingY >> 6;
            		float const advance  = mFace->glyph->advance.x >> 6;

			if ( mOutlineSpans.empty( ) )
			{
				std::cerr << "No outline found." << std::endl;
				exit( -1 );
			}

			auto blitSpans = [&]( Spans & xSpans, std::function<void(Spans::iterator  &, uint8_t*)> xProcessPixel )
			{
				for ( Spans::iterator s = xSpans.begin( ); s != xSpans.end( ); ++s )
				{
					for ( int32_t w = 0; w < s->width; ++w)
					{
						uint32_t const y = yOffset + ( s->y - rect.yMin );
						uint32_t const x = xOffset + ( s->x - rect.xMin + w );

						if ( y > ( xImage.getHeight( ) - 1 ) || x > ( xImage.getWidth ( ) - 1 ) ||
						     y < 0 || x < 0 )
						{
							continue;
						}

						uint8_t * data = xImage[ xImage.getHeight( ) - y - 1 ][ x ];

						xProcessPixel( s, data );
					}
				}
			};

			auto blendFunc = [&]( Spans::iterator & xS, uint8_t * xPixel )
			{
				xPixel [ 0 ] = (xPixel [ 0 ] + ( ( xFontColor.r - xPixel [ 0 ] ) * xFontColor.a) / 255.0f);
				xPixel [ 1 ] = (xPixel [ 1 ] + ( ( xFontColor.g - xPixel [ 1 ] ) * xFontColor.a) / 255.0f);
				xPixel [ 2 ] = (xPixel [ 2 ] + ( ( xFontColor.b - xPixel [ 2 ] ) * xFontColor.a) / 255.0f);
				xPixel [ 3 ] = std::min( (uint8_t)xS->coverage, (uint8_t)(xOutlineColor.a + xPixel[ 3 ]));
			};

			blitSpans( mOutlineSpans, blendFunc );

			blitSpans( mSpans, blendFunc );
		}

		FT_Bitmap * getBitmap( )   const { return & mFace->glyph->bitmap;      }

		uint32_t getGlyphWidth( )  const
		{
			return round( ( mFace->bbox.xMax - mFace->bbox.xMin ) * mSize / mFace->units_per_EM );
		}

		uint32_t getGlyphHeight( ) const
		{
			return round( ( mFace->bbox.yMax - mFace->bbox.yMin ) * mSize / mFace->units_per_EM );
		}

};

} // namespace sisu

#endif // FONT_EAF969A3C426403DA705D9614D79046D_HPP_
