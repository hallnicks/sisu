#include "ttyscreen.hpp"

#include <cstdlib>
#include <iostream>

#include <sys/ioctl.h>
#include <unistd.h>
#include <term.h>

using std::cout;

namespace sisu
{

TerminalScreen::TerminalScreen( )
	: Screen( )
	, mW( 0 )
	, mH( 0 )
	, mSize( 0 )
	, mRadius( 0 )
	, mMemory( 0 )
{
	winsize w;

	if ( ioctl( STDOUT_FILENO, TIOCGWINSZ, &w ) != 0 )
	{
		std::cerr << "Failed to get terminal width" << std::endl;
		exit( -1 );
	}
	else
	{
		mH = w.ws_row;
		mW = w.ws_col;
	}

	mSize = mH * mW;

	mRadius = ( mH + mW ) / 2;

	mMemory = new TTYC[ mH * mW ];

	for ( unsigned int i = 0; i < mSize; ++i ) { mMemory[ i ] = 0x0U; }
}

TerminalScreen::~TerminalScreen( )
{
	setPosition( 0, 0 );

	delete [] mMemory;
}

uint8_t TerminalScreen::getWidth( ) const { return mW; }

uint8_t TerminalScreen::getHeight( ) const { return mH; }

Screen::buffer TerminalScreen::scanLine( uint8_t const xY ) const
{
	Screen::buffer ret = NULL;

	if ( xY < getHeight( ) )
	{
		ret = mMemory + ( mW * xY );
	}

	return ret;
}

void TerminalScreen::blitPixel( uint8_t const xX, uint8_t const xY )
{
	if ( xX < mW && xY < mH )
	{
		TTYCTransform c( scanLine( xY )[ xX ] );

		cout << ccolor( c.getFG( )
				, c.getBG( )
				, c.getModifier( ) )

			<< c.getChar( )

			<< std::flush;
	}
}

void TerminalScreen::refreshPosition( uint8_t const xX, uint8_t const xY )
{
	setPosition( xX, xY );

	blitPixel( xX, xY );
}

Screen::const_buffer TerminalScreen::scanLineConst( uint8_t const xY ) const
{
	return const_cast<const_buffer>( mMemory + ( mW * xY ) );
}

void TerminalScreen::setPosition( uint8_t const xX, uint8_t const xY )
{
	cout << "\033[" << +xX << ";" << +xY << "m";
}

void TerminalScreen::refresh( )
{
	setPosition( 0, 0 );

	volatile int y = -1;

	for ( unsigned int y = 0; y < mH; ++y )
	{
		for ( unsigned int x = 0; x < mW; ++x )
		{
			blitPixel( x, y );
		}
	}

	cout << std::flush;
}

void TerminalScreen::setPixel( uint8_t const xX, uint8_t const xY, TTYC const xColor )
{
	setPosition( xX, xY );

	if ( xX < mW && xY < mH )
	{
		TTYCTransform c( xColor );

		cout 	<< ccolor( c.getFG( )
				, c.getBG( )
				, c.getModifier( ) )

			<< c.getChar( );
	}
}

} // namespace sisu
