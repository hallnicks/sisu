#ifndef TTYC_2F072687_A857_4DF0_B28D_1DF6A0058BEF_HPP_
#define TTYC_2F072687_A857_4DF0_B28D_1DF6A0058BEF_HPP_

#include <stdint.h>
#include <cstdlib>

namespace sisu
{

enum eTTYColor
{
        eTTYCNone       = 0x00,
        eTTYCBlack      = 0x01,
        eTTYCRed        = 0x02,
        eTTYCGreen      = 0x04,
        eTTYCYellow     = 0x08,
        eTTYCBlue       = 0x10,
        eTTYCMagenta    = 0x20,
        eTTYCCyan       = 0x40,
        eTTYCWhite      = 0x80,
        eTTYCMax        = 0x120
};

enum eTTYModifier
{
        eModNone        = 0x00,
        eModBold        = 0x01,
        eModDim         = 0x02,
        eModUnderline   = 0x04,
        eModBlink       = 0x08,
        eModReverse     = 0x10,
        eModMax         = 0x20,
};

typedef uint32_t TTYC;

class TTYCMap
{
        TTYCMap( );

        static eTTYColor const colors [ 10 ];

        static eTTYModifier const modifiers [ 7 ];

	friend class TTYCTransform;

	static uint8_t getColorCount( );

	static uint8_t getModifierCount( );

	static uint8_t getColorIndex( eTTYColor const xC );

	static uint8_t getModifierIndex( eTTYModifier const xM );

        public:
		static int getModifier( eTTYModifier const xM );

		static int getColor( eTTYColor const xC );

                static uint8_t randomCharacter( );

                static eTTYColor randomColor( );

                static eTTYModifier randomModifier( );

                static TTYC randomTTYC( );

                static eTTYColor incrementColor( eTTYColor const xC );

                static eTTYModifier incrementModifier( eTTYModifier const xM );
};

inline eTTYColor operator ++ ( eTTYColor & xRhs )
{
        return ( xRhs = TTYCMap::incrementColor( xRhs ) );
}

inline eTTYModifier operator ++ ( eTTYModifier & xRhs )
{
        return ( xRhs = TTYCMap::incrementModifier( xRhs ) );
}

class TTYCTransform
{
	TTYC mC;

	public:
		TTYCTransform( TTYC const xC );

		TTYCTransform( uint8_t const xChar
				, uint8_t const xFG
				, uint8_t const xBG
				, uint8_t const xMod );

		TTYCTransform( char const xChar
				, eTTYColor const xFG
				, eTTYColor const xBG
				, eTTYModifier const xMod );

		~TTYCTransform( );

		TTYC const & getRaw( ) const;

		uint8_t getCharU( ) const;
		char getChar( ) const;

		uint8_t getFGU( ) const;
		eTTYColor getFG( ) const;

		uint8_t getBGU( ) const;
		eTTYColor getBG( ) const;

		uint8_t getModifierU( ) const;
		eTTYModifier getModifier( ) const;

		void setCharU( uint8_t const xChar );
		void setChar( char const xChar );

		void setFGU( uint8_t const xForeground );
		void setFG( eTTYColor const xForeground );

		void setBGU( uint8_t const xBackground );
		void setBG( eTTYColor const xBackground );

		void setModifierU( uint8_t const xModifier );
		void setModifier( eTTYModifier const xModifier );

		static TTYC uint2ttyc( uint8_t const xChar
					, uint8_t const xFG
					, uint8_t const xBG
					, uint8_t const xMod );

		static TTYC color2ttyc( char const xChar
					, eTTYColor const xFG
					, eTTYColor const xBG
					, eTTYModifier const xMod );

		static void ttyc2uint( TTYC const xC
					, uint8_t * xCharOut
					, uint8_t * xFGOut
					, uint8_t * xBGOut
					, uint8_t * xModOut );

		static void ttyc2color( TTYC const xC
					, char & xCharOut
					, eTTYColor & xFGOut
					, eTTYColor & xBGOut
					, eTTYModifier & xModOut );
};

} // namespace sisu

#endif // TTYC_2F072687_A857_4DF0_B28D_1DF6A0058BEF_HPP_
