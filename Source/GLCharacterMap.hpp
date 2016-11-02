#ifndef GL_CHARACTER_MAP_178D5BA50C504563822A94CFC0A9D005_HPP_
#define GL_CHARACTER_MAP_178D5BA50C504563822A94CFC0A9D005_HPP_

#include "Font.hpp"
#include "PNGImage.hpp"
#include "DevILImage.hpp"
#include "ioassist.hpp"
#include "AndroidLogWrapper.hpp"

#include <string>
#include <sstream>
#include <map>

namespace sisu {
	class RAWRGBAImage
	{
		GLubyte * mData;
		uint32_t const mWidth, mHeight;

		public:
			RAWRGBAImage( GLubyte * const xData
				    , uint32_t const xWidth
	 			    , uint32_t const xHeight )
				: mData( xData )
				, mWidth( xWidth )
				, mHeight( xHeight )
			{
				;
			}

			~RAWRGBAImage( )
			{
				/*
				if ( mData != NULL )
				{
					free( mData );
				}
				*/
				// TODO: This causes a double free error in libdri.so.
				// However, it should be freed here.
				// Perhaps it has something to do with the texture binding.
			}

			// TODO: Rename this. It's not really an alloc anymore.
			GLubyte * allocGLBuffer( ) { return (GLubyte*)mData; }

			uint32_t getWidth( )  const { return mWidth;  }

			uint32_t getHeight( ) const { return mHeight; }
	};


	class GLCharacter
	{
		char const mC;

		//DevILImage mImage;
		RAWRGBAImage mImage;

		GLCharacter( char const xC
			   , uint8_t * xData
			   , uint32_t const xWidth
			   , uint32_t const xHeight )
			: mC( xC )
			, mImage( xData, xWidth, xHeight )
		{
			;
		}
#if 0
		GLCharacter( char const xC
			   , const char * xPath )
			: mC( xC )
			, mImage( xPath )
		{
			;
		}

		GLCharacter( char const xC
			   , uint8_t * xData
			   , size_t const xSize  )
			: mC( xC )
			, mImage( xData, xSize )
		{
			;
		}
#endif

		friend class GLCharacterMap;

		public:
			GLubyte * allocGLBuffer( ) { return mImage.allocGLBuffer( ); }

			char getCharacter() const { return mC; }

			uint32_t getWidth( )  { return mImage.getWidth( );  }

			uint32_t getHeight( ) { return mImage.getHeight( ); }
	};

	class GLCharacterMap
	{
		FTFont mFont;
		std::string const mTextureStorage;
		std::map< char, GLCharacter * > mCharacters;
		uint32_t const mFontSize;
		_FontPixel32 mOutlineColor, mFillColor;

		void _makeFilename( std::stringstream & xSS, char const xC )
		{
			xSS << mTextureStorage
			    << "texture_"
			    << ( uint64_t )xC
			    << ( uint64_t )mOutlineColor.r
			    << ( uint64_t )mOutlineColor.g
			    << ( uint64_t )mOutlineColor.b
			    << ( uint64_t )mOutlineColor.a
			    << ( uint64_t )mFillColor.r
			    << ( uint64_t )mFillColor.g
			    << ( uint64_t )mFillColor.b
			    << ( uint64_t )mFillColor.a
			    << ".png";
		}

		std::vector<uint8_t> _makePNG( char const xC )
		{
			mFont.loadGlyph( xC, 0.0f );

			PNGImage output( { mFont.getGlyphWidth( ), mFont.getGlyphHeight( ) } );

			mFont.printSpans( output, mOutlineColor, mFillColor );

#if 0
			SISULOG("Creating filename.");
			std::stringstream filename;

			_makeFilename( filename, xC );
#endif

			SISULOG("Blitting.");
			output.blit( );

			uint8_t *    data = output.getData( );
			size_t const size = output.getDataSize( );

			SISULOG("Writing to file for posterity.");

			std::cout << "Output data size was: " << size << std::endl;

#if 0
			memoryToFile( filename.str( ).c_str( ), data, size );
#endif

			std::vector<uint8_t> buffer;

			buffer.assign( data, data + size );

			SISULOG("_makePNG OUT");

			return buffer;
		}

		public:
			GLCharacterMap( const char * xFontPath
			       	      , uint32_t const xFontSize // fixed for now
				      , const char * xTextureStoragePath
				      , _FontPixel32 const & xOutline = _FontPixel32( 255, 255, 255 )
				      , _FontPixel32 const & xFill    = _FontPixel32( 255, 255, 255 ) )// TODO: Hook in memory font support if it becomes necessary (like streaming from net)
				: mFont( xFontPath )
				, mFontSize( xFontSize )
				, mTextureStorage( xTextureStoragePath )
				, mOutlineColor( xOutline )
				, mFillColor( xFill )
			{
				if ( xTextureStoragePath == NULL )
				{
					SISULOG("Texture storage location was not specified (null).");
					exit( -1 );
				}

				mFont.setFontSize( xFontSize );
			}

			~GLCharacterMap( )
			{
				for ( auto && ii : mCharacters )
				{
					delete ii.second;
				}
			}

			GLCharacter * operator[ ]( char const xCharacter )
			{
				GLCharacter * ret = NULL;

				if ( mCharacters.find( xCharacter ) == mCharacters.end( ) )
				{
//#ifdef ANDROID
#if 1
					mFont.loadGlyph( xCharacter, 0.0f );

					PNGImage output( { mFont.getGlyphWidth( ), mFont.getGlyphHeight( ) } );

					mFont.printSpans( output, mOutlineColor, mFillColor );

					mCharacters[ xCharacter ] = ret = new GLCharacter( xCharacter
											 , output.toGLTextureBuffer( )
											 , mFont.getGlyphWidth( )
											 , mFont.getGlyphHeight( ) );
#else
					while ( !fileExists( filename.str( ).c_str( ) ) ) { _makePNG( xCharacter ); }
					mCharacters[ xCharacter ] = ret = new GLCharacter( xCharacter, filename.str( ).c_str( ) );
#endif

				}
				else
				{
					ret = mCharacters[ xCharacter ];
				}

				return ret;
			}
	};

} // namespace sisu
#endif // GL_CHARACTER_MAP_178D5BA50C504563822A94CFC0A9D005_HPP_
