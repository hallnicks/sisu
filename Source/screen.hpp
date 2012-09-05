#ifndef SCREEN_4C324AB3_610A_45E5_8202_FAC1D57F3DE4_HPP_
#define SCREEN_4C324AB3_610A_45E5_8202_FAC1D57F3DE4_HPP_

#include "scanline.hpp"
#include "ttycolor.hpp"

#include <stdint.h>

namespace sisu
{

class Screen
{
	public:
		typedef TTYC * buffer;
		typedef TTYC const * const const_buffer;

	protected:
		virtual buffer scanLine( uint8_t const xY ) const = 0;
		virtual const_buffer scanLineConst( uint8_t const xY ) const = 0;

	public:
		Scanline operator [ ] ( uint8_t const xY ) const;
		virtual uint8_t getWidth( ) const = 0;
		virtual uint8_t getHeight( ) const = 0;
		virtual void refreshPosition( uint8_t const xX, uint8_t const xY ) = 0;
		virtual void blitPixel( uint8_t const xX, uint8_t const xY ) = 0;
		virtual void setPixel( uint8_t const xX, uint8_t const xY, TTYC const xColor ) = 0;
		virtual void refresh( ) = 0;
};

} // namespace sisu

#endif // SCREEN_4C324AB3_610A_45E5_8202_FAC1D57F3DE4_HPP_
