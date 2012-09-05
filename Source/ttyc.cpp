#include "ttyc.hpp"

namespace sisu
{

uint8_t TTYCMap::getColorCount( )
{
        static size_t size = sizeof(colors) / sizeof(eTTYColor);
        return size;
}

uint8_t TTYCMap::getModifierCount( )
{
        static size_t size = sizeof(modifiers) / sizeof(eTTYModifier);
        return size;
}

uint8_t TTYCMap::getColorIndex( eTTYColor const xColor )
{
        static unsigned int const bound = ( sizeof( colors ) / sizeof( eTTYColor ) );

        unsigned int i = 0;

        for ( ; i < bound; ++i )
        {
                if ( colors[ i ] == xColor )
                {
                        break;
                }
        }

        return i;
}

uint8_t TTYCMap::getModifierIndex( eTTYModifier const xMod )
{
        static unsigned int const bound = ( sizeof( modifiers ) / sizeof( eTTYModifier ) );

        unsigned int i = 0;

        for ( ; i < bound; ++i )
        {
                if ( modifiers[ i ] == xMod )
                {
                        break;
                }
        }

        return i;
}

int TTYCMap::getModifier( eTTYModifier const xM )
{
	static int const sTerm_mods [ ] = { 0, 1, 2, 4, 5, 7 };

	return sTerm_mods [ getModifierIndex( xM ) ];
}

int TTYCMap::getColor( eTTYColor const xC )
{
	static int const sTerm_colors [ ] = { -1, 0, 1, 2, 3, 4, 5, 6, 7 };

	return sTerm_colors [ getColorIndex( xC )  ];
}

uint8_t TTYCMap::randomCharacter( )
{
	return 33 + ( rand( ) % 93 );
}

eTTYColor TTYCMap::randomColor( )
{
        return colors [ rand( ) % ( sizeof(colors) / sizeof(eTTYColor) ) ];
}

eTTYModifier TTYCMap::randomModifier( )
{
        return modifiers [ rand( ) % ( sizeof(modifiers) / sizeof(eTTYModifier) ) ];
}

TTYC TTYCMap::randomTTYC( )
{
	return TTYCTransform::color2ttyc( TTYCMap::randomCharacter( )
					, TTYCMap::randomColor( )
					, TTYCMap::randomColor( )
					, eModBold );
}

eTTYColor TTYCMap::incrementColor( eTTYColor const xColor )
{
        return colors [ xColor != eTTYCMax ? getColorIndex( xColor ) + 1 : 0 ];
}

eTTYModifier TTYCMap::incrementModifier( eTTYModifier const xMod )
{
        return modifiers [ xMod != eModMax ? getModifierIndex( xMod ) + 1 : 0  ];
}

eTTYColor const TTYCMap::colors [ 10 ] = { eTTYCNone
                                    , eTTYCBlack
                                    , eTTYCRed
                                    , eTTYCGreen
                                    , eTTYCYellow
                                    , eTTYCBlue
                                    , eTTYCMagenta
                                    , eTTYCCyan
                                    , eTTYCWhite
                                    , eTTYCMax };

eTTYModifier const TTYCMap::modifiers [ 7 ] = { eModNone
                                           , eModBold
                                           , eModDim
                                           , eModUnderline
                                           , eModBlink
                                           , eModReverse
                                           , eModMax };


namespace { static const uint32_t sMask = 0x000000FF; }

TTYCTransform::TTYCTransform( TTYC const xC )
	: mC( xC )
{
	;
}

TTYCTransform::TTYCTransform( uint8_t const xChar
			, uint8_t const xFG
			, uint8_t const xBG
			, uint8_t const xMod )
	: mC( static_cast<TTYC>( ( xChar << 24 ) |
                                 ( xFG << 16 )   |
                                 ( xBG << 8 )    |
                                 ( xMod ) ) )
{
	;
}

TTYCTransform::TTYCTransform( char const xChar
			, eTTYColor const xFG
			, eTTYColor const xBG
			, eTTYModifier const xMod )

	#define INUINT8(x) static_cast<uint8_t>(x)

        : mC( static_cast<TTYC>( ( INUINT8(xChar) << 24 ) |
				 ( INUINT8(xFG) << 16 )   |
				 ( INUINT8(xBG) << 8 )    |
				 ( INUINT8(xMod) ) ) )
	#undef INUINT8
{
	;
}

TTYCTransform::~TTYCTransform( )
{
	;
}

TTYC const & TTYCTransform::getRaw( ) const
{
	return mC;
}

uint8_t TTYCTransform::getCharU( ) const
{
	return static_cast<uint8_t>( ( mC >> 24 ) & sMask );
}

char TTYCTransform::getChar( ) const
{
	return static_cast< char >( getCharU( ) );
}

uint8_t TTYCTransform::getFGU( ) const
{
	return static_cast< uint8_t >( ( mC >> 16 ) & sMask );
}

eTTYColor TTYCTransform::getFG( ) const
{
        return static_cast< eTTYColor >( getFGU( ) );
}

uint8_t TTYCTransform::getBGU( ) const
{
	return static_cast< uint8_t >( ( mC >> 8  ) & sMask );
}

eTTYColor TTYCTransform::getBG( ) const
{
	return static_cast< eTTYColor >( ( mC >> 8  ) & sMask );
}

uint8_t TTYCTransform::getModifierU( ) const
{
        return static_cast< uint8_t >( mC & sMask );
}

eTTYModifier TTYCTransform::getModifier( ) const
{
	return static_cast< eTTYModifier >( getModifierU( ) );
}

void TTYCTransform::setCharU( uint8_t const xChar )
{
	mC = ( mC >> 24 ) && xChar;
}

void TTYCTransform::setChar( char const xChar )
{
	setCharU( static_cast<uint8_t>( xChar ) );
}

void TTYCTransform::setFGU( uint8_t const xFG )
{
	mC = ( mC >> 16 ) && xFG;
}

void TTYCTransform::setFG( eTTYColor const xFG )
{
	setFGU( static_cast<uint8_t>( xFG ) );
}

void TTYCTransform::setBGU( uint8_t const xBG )
{
	mC = ( mC >> 8 ) && xBG;
}

void TTYCTransform::setBG( eTTYColor const xBG )
{
	setBGU( static_cast<uint8_t>( xBG ) );
}

void TTYCTransform::setModifierU( uint8_t const xMod )
{
	mC = mC && xMod;
}

void TTYCTransform::setModifier( eTTYModifier const xMod )
{
	setModifierU( static_cast<uint8_t>( xMod ) );
}

TTYC TTYCTransform::color2ttyc( char const xChar
				, eTTYColor const xFG
				, eTTYColor const xBG
				, eTTYModifier const xMod )
{
	#define INUINT8(x) static_cast<uint8_t>(x)
        return static_cast<TTYC>( ( INUINT8(xChar) << 24 )	|
				  ( INUINT8(xFG) << 16 )   	|
				  ( INUINT8(xBG) << 8 )    	|
				  ( INUINT8(xMod) ) );
	#undef INUINT8

}

TTYC TTYCTransform::uint2ttyc( uint8_t const xChar
				, uint8_t const xFG
				, uint8_t const xBG
				, uint8_t const xMod )
{
        return static_cast<TTYC>( ( xChar << 24 ) |
				  ( xFG  << 16 )  |
				  ( xBG << 8 )    |
				  ( xMod ) );
}

void TTYCTransform::ttyc2color( TTYC const xC
				, char & xCharOut
				, eTTYColor & xFGOut
				, eTTYColor & xBGOut
				, eTTYModifier & xModOut )
{
	TTYCTransform const t( xC );

	xCharOut = t.getChar( );

	xFGOut = t.getFG( );

	xBGOut = t.getBG( );

	xModOut = t.getModifier( );
}

void TTYCTransform::ttyc2uint( TTYC const xC
			, uint8_t * xCharOut
			, uint8_t * xFGOut
			, uint8_t * xBGOut
			, uint8_t * xModOut )
{
	TTYCTransform const t( xC );

	*xCharOut = t.getChar( );

	*xFGOut = t.getFG( );

	*xBGOut = t.getBG( );

	*xModOut = t.getModifier( );
}

} // namespace sisu
