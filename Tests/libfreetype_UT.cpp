#ifdef LIBFREETYPE_UT
#include "test.hpp"

#include "GLCharacterMap.hpp"
using namespace sisu;

namespace
{
	class libfreetype_UT : public context
	{
		public:
			libfreetype_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};
} // namespace

TEST(libfreetype_UT, loadFontFromFile)
{
	FTFont font( "resources/Cactus_Love.ttf" );
}

TEST(libfreetype_UT, createCharacterMap)
{
	GLCharacterMap charmap( "resources/terminus.ttf", 32, "" );

	for ( char c = '!'; c < '~'; c++ )
	{
		GLCharacter * pC = charmap[c];

		MUSTNEQ( pC, NULL );
	}

	GLCharacter * pC = charmap[ ' ' ];
	MUSTNEQ( pC, NULL );
}


TEST(libfreetype_UT, loadFontFromMemory)
{
	std::vector<unsigned char> buffer = fileToMemory<unsigned char>( "resources/terminus.ttf" );

	FTFont font( buffer.data( ), buffer.size( ) );

	font.setFontSize( 32 );

	//□
	for ( char c = '!'; c < '~'; c++ )
	{
		font.loadGlyph( c, 0.25f );

		std::cout << c;

		PNGImage output( { font.getGlyphWidth( ), font.getGlyphHeight( ) } );

		output.perPixel( [ ] ( PNGPixel xPixel )
		{
			xPixel.data[ 0 ] = 0;
			xPixel.data[ 1 ] = 0;
			xPixel.data[ 2 ] = 0;
			xPixel.data[ 3 ] = 255;
		} );

		font.printSpans( output, _FontPixel32( 255, 255, 255 ), _FontPixel32( 255, 255, 255 ) );

		std::stringstream ss;

		ss << "texture_" << c << ".png";

		std::ofstream ofs(ss.str( ).c_str( ), std::ios::binary);

		ofs << output;

		ofs.close( );
	}
}
#endif // LIBFREETYPE_UT
