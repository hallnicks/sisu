#ifndef SCREEN_4C324AB3_610A_45E5_8202_FAC1D57F3DE4_HPP_
#define SCREEN_4C324AB3_610A_45E5_8202_FAC1D57F3DE4_HPP_

#include "ttycolor.hpp"

namespace sisu
{

class Screen
{
	public:
		virtual unsigned short getWidth( ) const = 0;
		virtual unsigned short getHeight( ) const = 0;
		typedef TTYC * buffer;
		typedef TTYC const * const const_buffer;
		virtual buffer scanLine( unsigned short const xY ) const = 0;
		virtual const_buffer scanLineConst( unsigned short const xY ) const = 0;
		virtual void refresh( ) = 0;
};


} // namespace sisu

#endif // SCREEN_4C324AB3_610A_45E5_8202_FAC1D57F3DE4_HPP_
