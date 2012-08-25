#ifndef SCREEN_4C324AB3_610A_45E5_8202_FAC1D57F3DE4_HPP_
#define SCREEN_4C324AB3_610A_45E5_8202_FAC1D57F3DE4_HPP_

#include "ttycolor.hpp"

namespace sisu
{

class Scanline
{
	TTYC * mBuffer;

	unsigned int mWidth;

	public:
		Scanline( TTYC * xBuffer, unsigned int xWidth )
			: mBuffer( xBuffer )
			, mWidth( xWidth )
		{
			;
		}

		TTYC * operator [ ] ( unsigned int xIdx )
		{
			static TTYC DeadBit = 0;
			TTYC * ret = &DeadBit;

			if ( mWidth > xIdx )
			{
				ret = &mBuffer[ xIdx ];
			}

			return ret;
		}

};

class Screen
{
	public:
		typedef TTYC * buffer;
		typedef TTYC const * const const_buffer;

	protected:
		virtual buffer scanLine( unsigned short const xY ) const = 0;
		virtual const_buffer scanLineConst( unsigned short const xY ) const = 0;

	public:
		virtual Scanline operator[ ]( unsigned int xY )
		{
			static TTYC DeadChar = 0;
			static Scanline Deadline( &DeadChar, 1 );

			if ( xY < getWidth( ) )
			{
				return Scanline( scanLine( xY ), getWidth( ) );
			}

			return Deadline;
		}

		virtual unsigned short getWidth( ) const = 0;
		virtual unsigned short getHeight( ) const = 0;
		virtual void setPosition( unsigned int xX, unsigned int xY ) = 0;
		virtual void refresh( ) = 0;
};


} // namespace sisu

#endif // SCREEN_4C324AB3_610A_45E5_8202_FAC1D57F3DE4_HPP_
