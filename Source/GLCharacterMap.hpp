#ifndef GL_CHARACTER_MAP_178D5BA50C504563822A94CFC0A9D005_HPP_
#define GL_CHARACTER_MAP_178D5BA50C504563822A94CFC0A9D005_HPP_

#include "Font.hpp"
#include "PNGImage.hpp"

#include <string>
#include <sstream>
#include <map>

namespace sisu {
	class GLCharacter
	{
		char const mC;

		PNGImage mImage;

		GLCharacter( char const xC
			   , const char * xPath )
			: mC( xC )
			, mImage( xPath )
		{
			;
		}

		friend class GLCharacterMap;

		public:
			GLubyte * allocGLBuffer( ) { return mImage.toGLTextureBuffer( ); }

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

		// TODO: Support different colors. For now, just use white on black. 
		void _makePNG( char const xC )
		{
			mFont.loadGlyph( xC, 0.0f );

			PNGImage output( { mFont.getGlyphWidth( ), mFont.getGlyphHeight( ) } );

			mFont.printSpans( output, mOutlineColor, mFillColor );

			std::stringstream filename;

			_makeFilename( filename, xC );

			std::ofstream ofs( filename.str( ).c_str( ), std::ios::binary);

			ofs << output;

			ofs.close( );
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
					std::cerr << "Texture storage location was not specified (null)." << std::endl;
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
					// We have not cached this character yet!
					// Check for a file.

					std::stringstream filename;

					_makeFilename( filename, xCharacter );

					auto checkFile = [&]( )->bool
					{
						std::ifstream ifs( filename.str( ).c_str( ) );
						return ifs.good( );
					};


					while ( !checkFile( ) ) { _makePNG( xCharacter ); }

					mCharacters[ xCharacter ] = ret = new GLCharacter( xCharacter, filename.str( ).c_str( ) );

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
