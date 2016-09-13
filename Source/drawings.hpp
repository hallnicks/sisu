#ifndef DRAWINGS_99BB3631_3CE9_4F0A_9A33_0A792E6750A7_HPP_
#define DRAWINGS_99BB3631_3CE9_4F0A_9A33_0A792E6750A7_HPP_

#include "screen.hpp"
#include "ttystring.hpp"
#include "primitives.hpp"
#include <stdint.h>

namespace sisu
{
	struct ThoughtBox
	{
		ThoughtBox( char const * const xMessage
			, uint8_t const xX
			, uint8_t const xY
			, uint8_t const xW
			, uint8_t const xH
			, TTYC const xBG
			, TTYC const xFG );

		enum eTBMode
		{
			eNone		= 0,
			eOK		= 1,
			eOKCancel 	= 2,
			eCheckBox	= 3,
		};

		uint8_t X, Y, W, H;

		char const * const Message;

		TTYC BG, FG;
	};

	enum eDrawingMode
	{
		eRedrawImmediately 	= 0,
		eVsync 			= 1,
		eOnlyChanged 		= 2,
		eDrawingModeMax 	= 3,
	};

	inline eDrawingMode operator++( eDrawingMode & xMode )
	{
		return ( xMode = static_cast<eDrawingMode>( static_cast<int>( xMode ) + 1 ) );
	}

	class Changed
	{
		int16_t mMax;

	        uint8_t mMagnitude;

		uint8_t mBound;

		Point2DOU * mPoints;

		public:
			Changed( Screen * const xS );

			~Changed( );

			Point2DOU const & operator [ ] ( uint8_t const xIndex ) const;

			void operator ( ) ( uint8_t const xX, uint8_t const xY );

			void reset( );

			uint8_t max( ) const;
	};

        class Drawings
        {
                Screen * mS;

		Changed * mCh;

		eDrawingMode mMode;

		public:
                        Drawings( Screen * const xScreen, eDrawingMode const xDrawMode = eOnlyChanged );

                        ~Drawings( );

			void hline( uint8_t const xY, uint8_t const xStart, uint8_t const xWidth, TTYC const xC );

			void setPixel( uint8_t const xX, uint8_t const xY, TTYC const xC );

			void thoughtBox( ThoughtBox const & xTB );

			template< uint8_t XSize >
			void drawText( uint8_t const xStartX, uint8_t const xYPos, TTYString<XSize> const & xString )
			{
				uint8_t x = xStartX;

				for ( uint8_t i = 0; i < XSize; ++i )
				{
					setPixel( x, xYPos, xString[ i ] ); ++x;
				}
			}

                        void mandlebrot( TTYC const xC, double const xScale = 1.0 );

			void midpointCircle( uint8_t const xX, uint8_t const xY, uint8_t const xRadius, TTYC const xC );

                        void clear( TTYC const xC );

			void refresh( );
        };

} // namespace sisu

#endif // DRAWINGS_99BB3631_3CE9_4F0A_9A33_0A792E6750A7_HPP_
