#ifndef TTYSCREEN_432BD495_F79C_4480_993B_9D34C509B77E_HPP_
#define TTYSCREEN_432BD495_F79C_4480_993B_9D34C509B77E_HPP_

#include "screen.hpp"
#include "ttyc.hpp"
#ifndef __linux__
#include <Windows.h>
#endif

namespace sisu
{

class TerminalScreen : public Screen
{

#ifndef __linux__
	HANDLE mStdOut;
#endif

	uint8_t mW, mH, mSize, mRadius;

	buffer mMemory, mDeadline;

	void clearScreen( char const xAttribute );
	void getPixel( uint8_t const xX, uint8_t const xY );

	void setPosition( uint8_t const xX, uint8_t const xY );

	public:
		TerminalScreen( );
		~TerminalScreen( );

		virtual uint8_t getWidth( ) const;
		virtual uint8_t getHeight( ) const;
		virtual Screen::buffer scanLine( uint8_t const xY ) const;
		virtual Screen::const_buffer scanLineConst( uint8_t const xY ) const;
		virtual void refreshPosition( uint8_t const xX, uint8_t const xY );
		virtual void blitPixel( uint8_t const xX, uint8_t const xY );
		virtual void setPixel( uint8_t const xX, uint8_t const xY, TTYC const xColor );
                virtual void fill( TTYC const xColor );
		virtual void refresh( );
};

} // namespace sisu

#endif // TTYSCREEN_432BD495_F79C_4480_993B_9D34C509B77E_HPP_
