#ifndef TTYCOLOR_HPP_
#define TTYCOLOR_HPP_

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <time.h>

#include <iostream>

#include "bits.hpp"

namespace sisu
{

// TODO - Convert to tables here using #include "mydef.tbl" pattern.

enum eTTYColor
{
	eTTYCNone	= 0x00,
	eTTYCBlack 	= 0x01,
	eTTYCRed 	= 0x02,
	eTTYCGreen	= 0x04,
	eTTYCYellow	= 0x08,
	eTTYCBlue	= 0x10,
	eTTYCMagenta	= 0x20,
	eTTYCCyan	= 0x40,
	eTTYCWhite	= 0x80,
	eTTYCMax	= 0x120
};

inline char const * const getColorStr( eTTYColor xColor )
{
	switch ( xColor )
	{
		case eTTYCNone:		return "eTTYCNone";
	        case eTTYCBlack:	return "eTTYCBlack";
	        case eTTYCRed:		return "eTTYCRed";
	        case eTTYCGreen:	return "eTTYCGreen";
	        case eTTYCYellow:	return "eTTYCYellow";
	        case eTTYCBlue:		return "eTTYCBlue";
	        case eTTYCMagenta:	return "eTTYCMagenta";
	        case eTTYCCyan:		return "eTTYCCyan";
	        case eTTYCWhite:	return "eTTYCWhite";
	        case eTTYCMax:		return "eTTYCMax";
		default: 		return "eTTYColor - Unsupported or masked";
	}
}

enum eTTYModifier
{
	eModNone 	= 0x00,
	eModBold	= 0x01,
	eModDim		= 0x02,
	eModUnderline	= 0x04,
	eModBlink	= 0x08,
	eModReverse	= 0x10,
	eModMax		= 0x20,
};

inline char const * const getModifier( eTTYModifier xModifier )
{
        switch ( xModifier )
        {
	        case eModNone:		return "eModNone";
	        case eModBold:		return "eModBold";
	        case eModDim:		return "eModDim";
	        case eModUnderline:	return "eModUnderline";
	        case eModBlink:		return "eModBlink";
        	case eModReverse:	return "eModReverse";
	        case eModMax:		return "eModMax";
		default: 		return "eTTYModifier - Unsupported or masked.";
	}
}

template< typename XE >
inline XE increment( XE & xE, XE const & xMax, XE const & xMin )
{
	return ( xE == xMax ? xE = xMin : xE = static_cast< XE >( static_cast< int >( xE  ) + 1 ) );
}

class TTYCMap
{
	static eTTYColor const colors[10];

       	static eTTYModifier const modifiers[7];

	public:
		static int getColorCount( ) { return sizeof(colors) / sizeof(eTTYColor); }

		static int getModifierCount( ) { return sizeof(modifiers) / sizeof(eTTYModifier); }

		static int getColorIndex( eTTYColor xColor )
		{
			unsigned int i = 0;

			unsigned int const bound = ( sizeof( colors ) / sizeof( eTTYColor ) );

			for ( ; i < bound; ++i )
			{
				if ( colors[ i ] == xColor )
				{
					break;
				}
			}

			return i;
		}

		static int getModifierIndex( eTTYModifier xMod )
		{
			unsigned int i = 0;
			unsigned int const bound = ( sizeof( modifiers ) / sizeof( eTTYModifier ) );

			for ( ; i < bound; ++i )
			{
				if ( modifiers[ i ] == xMod )
				{
					break;
				}
		        }

			return i;
		}

		static eTTYColor randomColor( )
		{
			return colors[ rand( ) % ( sizeof(colors) / sizeof(eTTYColor) ) ];
		}

		static eTTYModifier randomModifier( )
		{
			return modifiers[ rand( ) % ( sizeof(modifiers) / sizeof(eTTYModifier) ) ];
		}

		static eTTYColor incrementColor( eTTYColor const xColor )
		{
			return colors[ xColor != eTTYCMax ? getColorIndex( xColor ) + 1 : 0 ];
		}

		static eTTYModifier incrementModifier( eTTYModifier const xMod )
		{
			return modifiers[ xMod != eModMax ? getModifierIndex( xMod ) + 1 : 0  ];
		}
};

inline void operator ++ ( eTTYColor & xColor )
{
	xColor = TTYCMap::incrementColor( xColor );

}

inline void operator ++ ( eTTYModifier & xMod )
{
	xMod = TTYCMap::incrementModifier( xMod );
}

typedef uint32_t TTYC;

inline TTYC uint8_t2ttyc( uint8_t const xChar
			, uint8_t const xFG
			, uint8_t const xBG
			, uint8_t const xMod )
{
	return ( xChar << 24 ) | ( xFG << 16 ) | ( xBG << 8 ) | ( xMod );
}

inline TTYC color2ttyc( char const xChar
			, eTTYColor const xFG
			, eTTYColor const xBG
			, eTTYModifier const xMod )
{
	#define INUINT8(x) static_cast<uint8_t>(x)
	return uint8_t2ttyc( INUINT8(xChar), INUINT8(xFG), INUINT8(xBG), INUINT8(xMod) );
	#undef INUINT8
}

inline void ttyc2uint8_t( TTYC const xC
			, uint8_t * xCharOut
			, uint8_t * xFGOut
			, uint8_t * xBGOut
			, uint8_t * xModOut )
{
	// TODO - support little endian / middle endian types based on platform
	static uint32_t const mask = 0x000000FF;

	*xModOut 	= static_cast<uint8_t>( xC & mask );
	*xBGOut 	= static_cast<uint8_t>( (xC >> 8) & mask );
	*xFGOut 	= static_cast<uint8_t>( (xC >> 16) & mask );
	*xCharOut 	= static_cast<uint8_t>( (xC >> 24) & mask );
}

inline void ttyc2color( TTYC const xC
			, char * xCharOut
			, eTTYColor * xFGOut
			, eTTYColor * xBGOut
			, eTTYModifier * xModOut )
{
	uint8_t xCharUint8 = 0, xFGUint8 = 0, xBGUint8 = 0, xModUint8 = 0;

	ttyc2uint8_t( xC, &xCharUint8, &xFGUint8, &xBGUint8, &xModUint8 );

	*xCharOut 	= static_cast<char>( xCharUint8 );
	*xFGOut 	= static_cast<eTTYColor>( xFGUint8 );
	*xBGOut 	= static_cast<eTTYColor>( xBGUint8 );
	*xModOut 	= static_cast<eTTYModifier>( xModUint8 );
}

template<typename XChar>
class basic_ttycolor
{
	eTTYColor mFG, mBG;

	eTTYModifier mMod;

	static char const term = 27;

//	basic_ttycolor( basic_ttycolor const & );

//	basic_ttycolor & operator=( basic_ttycolor const & );

	template<typename YChar>
	struct is_wide { static bool const yes = (sizeof(YChar) > 1); };

	typedef std::basic_ostream< XChar, std::char_traits< XChar > > cout_t;

	cout_t & mOut;

	public:
		basic_ttycolor( eTTYColor xFG
				, eTTYColor xBG
				, eTTYModifier xAttr
				, std::ostream & xOut = std::cout )
			: mMod( xAttr )
			, mFG( xFG )
			, mBG( xBG )
			, mOut( xOut ) { }

                typedef cout_t & ( * endl_t )( cout_t & );

		~basic_ttycolor( ) { mOut << term << "[" << 0 << "m" << std::flush; }

                template< typename T >
                basic_ttycolor & operator << ( T & xStream )
                {
			mOut << xStream;

			return (*this);
                }

                basic_ttycolor & operator << ( endl_t xStream )
		{
		        return (*this) << xStream;
		}

		std::ostream & operator >> ( std::ostream & xStream ) const
		{
		        static int const bgOffset = 30;
		        static int const fgOffset = 40;

		        if ( mFG != eTTYCNone )
		        {
		                static int const term_colors[] = { -1, 0, 1, 2, 3, 4, 5, 6, 7 };
		                static int const term_mods[] = { 0, 1, 2, 4, 5, 7 };

		                xStream	<< term
					<< "["
					<< term_mods[ TTYCMap::getModifierIndex( mMod ) ]
					<< ";"
					<< ( fgOffset + term_colors[ TTYCMap::getColorIndex( mFG ) ] );

                                if ( mBG != eTTYCNone )
                                {
                                        xStream << ";" << ( bgOffset + term_colors[ TTYCMap::getColorIndex( mBG ) ] );
                                }

                               	xStream << "m" << std::flush;
		        }

		        return xStream;
		}
};

template<typename XCharType>
std::ostream & operator << ( std::ostream & xStream, basic_ttycolor<XCharType> const & xColor )
{
        return xColor >> xStream;
}

typedef basic_ttycolor<char> ccolor;

} // namespace sisu

#endif // TTYCOLOR_HPP_
