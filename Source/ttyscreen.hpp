#ifndef TTYSCREEN_432BD495_F79C_4480_993B_9D34C509B77E_HPP_
#define TTYSCREEN_432BD495_F79C_4480_993B_9D34C509B77E_HPP_

#include "screen.hpp"

namespace sisu
{

class TerminalScreen : public Screen
{
	unsigned int mW, mH, mSize, mRadius;

	buffer mMemory;

	buffer mDeadline;

	public:
		TerminalScreen( );
		~TerminalScreen( );
		virtual unsigned short getWidth( ) const;
		virtual unsigned short getHeight( ) const;
		virtual Screen::buffer scanLine( unsigned short const xY ) const;
		virtual Screen::const_buffer scanLineConst( unsigned short const xY ) const;
		virtual void setPosition( unsigned int xX, unsigned int xY );
		virtual void refresh( );
};

} // namespace sisu

#endif // TTYSCREEN_432BD495_F79C_4480_993B_9D34C509B77E_HPP_
