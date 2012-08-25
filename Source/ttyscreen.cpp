#include "ttyscreen.hpp"

#include <cstdlib>
#include <iostream>
#include <sys/ioctl.h>

using std::cerr;
using std::cout;
using std::endl;
using std::flush;

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
		cerr << "Failed to get terminal width" << endl;
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
	delete[] mMemory;
}

unsigned short TerminalScreen::getWidth( ) const { return mW; }

unsigned short TerminalScreen::getHeight( ) const { return mH; }

Screen::buffer TerminalScreen::scanLine( unsigned short const xY ) const { return mMemory + ( mW * xY ); }

Screen::const_buffer TerminalScreen::scanLineConst( unsigned short const xY ) const { return const_cast<const_buffer>( mMemory + ( mW * xY ) ); }

void TerminalScreen::refresh( )
{
	if ( system( "clear" ) != 0 )
	{
		cout << "clear command is unsupported"
			" on this platform."
				<< endl;
	}

	volatile int y = -1;

	char c = 0;
	eTTYColor fg = eTTYCNone, bg = eTTYCNone;
	eTTYModifier mod = eModNone;

	for ( unsigned int y = 0; y < mH; ++y )
	{
		for ( unsigned int x = 0; x < mW; ++x )
		{
			ttyc2color( scanLine( y )[ x ], &c, &fg, &bg, &mod );

			cout << ccolor( fg, bg, mod ) << c;
		}

		cout << endl;
	}

	cout << flush;
}

} // namespace sisu
