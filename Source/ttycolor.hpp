#ifndef TTYCOLOR_HPP_
#define TTYCOLOR_HPP_

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>

enum eTTYColor
{
	eTTYCNone	= 0x00,
	eTTYCBlack 	= 0x01,
	eTTYCRed 	= 0x02,
	eTTYCGreen	= 0x03,
	eTTYCYellow	= 0x04,
	eTTYCBlue	= 0x05,
	eTTYCMagenta	= 0x06,
	eTTYCCyan	= 0x07,
	eTTYCWhite	= 0x08,
	eTTYCMax	= 0x09
};

enum eTTYModifier
{
	eModNone 	= 0x00,
	eModBold	= 0x01,
	eModDim		= 0x02,
	eModUnderline	= 0x03,
	eModBlink	= 0x04,
	eModReverse	= 0x05,
	eModMax		= 0x06,
};

template< typename XE >
inline XE increment( XE & xE, XE const & xMax, XE const & xMin )
{
        return ( xE == xMax ? xE = xMin : xE = static_cast< XE >( static_cast< int >( xE  ) + 1 ) );
}

inline void operator ++ ( eTTYColor & xColor )
{
        xColor = increment< eTTYColor >( xColor, eTTYCMax, eTTYCNone );
}

inline void operator ++ ( eTTYModifier & xColor )
{
        xColor = increment< eTTYModifier >( xColor, eModMax, eModNone );
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
	*xCharOut 	= xC & 0x000000FF;
	*xFGOut 	= xC & 0x0000FF00;
	*xBGOut 	= xC & 0x00FF0000;
	*xModOut 	= xC & 0xFF0000;
}

inline void ttyc2color( TTYC const xC
			, char * xCharOut
			, eTTYColor * xFGOut
			, eTTYColor * xBGOut
			, eTTYModifier * xModOut )
{
	uint8_t xCharUint8 = 0, xFGUint8 = 0, xBGUint8 = 0, xModUint8 = 0;

	ttyc2uint8_t( xC, &xCharUint8, &xFGUint8, &xBGUint8, &xModUint8 );

	*xCharOut 	= static_cast<char>(xCharUint8);
	*xFGOut 	= static_cast<eTTYColor>(xFGUint8);
	*xBGOut 	= static_cast<eTTYColor>(xBGUint8);
	*xModOut 	= static_cast<eTTYModifier>(xModUint8);
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

		~basic_ttycolor( )
		{
		  	mOut << term << "[" << 0 << "m" << std::flush;
		}


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
					<< term_mods[ mMod ]
					<< ";"
					<< ( fgOffset + term_colors[ mFG ] );

                                if ( mBG != eTTYCNone )
                                {
                                        xStream << ";" << ( bgOffset + term_colors[ mBG ] );
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

#endif // TTYCOLOR_HPP_
