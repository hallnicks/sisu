#include "ttyscreen.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include <unistd.h>
#ifdef __linux__
#error __linux__ is defined on windows.

#include <sys/ioctl.h>
#include <term.h>
#else
//#warning Windows not yet supported
#include <windows.h>
#endif

using std::cout;
using std::endl;

namespace sisu
{

TerminalScreen::TerminalScreen( )
	: Screen( )
	, mW( 0 )
	, mH( 0 )
	, mSize( 0 )
	, mRadius( 0 )
	, mMemory( 0 )
	#ifndef __linux__
	, mStdOut( INVALID_HANDLE_VALUE )
#endif
{
#ifndef __linux__

#if 0
	mStdOut = CreateConsoleScreenBuffer( GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

	if ( mStdOut == INVALID_HANDLE_VALUE )
	{
		cout << "CreateConsoleScreenBuffer() failed." << GetLastError() << endl;
		exit(-1);
	}

	if ( SetConsoleActiveScreenBuffer( mStdOut ) == FALSE )
	{
		cout << "SetConsoleActiveScreenBuffer() failed." << GetLastError() << endl;
		exit(-1);
	}

	if ( SetStdHandle( STD_OUTPUT_HANDLE, mStdOut ) == FALSE ) 
	{
		cout << "SetStdHandle() failed." << GetLastError() << endl;
		exit(-1);
	}

	if ( SetConsoleTitle( "sisu" ) == FALSE )
	{
		cout << "SetConsoleTitle() failed." << GetLastError() << endl;
		exit(-1);
	}
#endif

#endif

#ifdef __linux__
	winsize w;

	if ( ioctl( STDOUT_FILENO, TIOCGWINSZ, &w ) != 0 )
	{
		std::cerr << "Failed to get terminal width" << endl;
		exit( -1 );
	}
	else
	{
		mH = w.ws_row;
		mW = w.ws_col;
	}
#else
	// very bad but hack for now
	mH = 30;
	mW = 100;
#endif

	mSize = mH * mW;

	mRadius = ( mH + mW ) / 2;

	mMemory = new TTYC[ mH * mW ];

	for ( unsigned int i = 0; i < mSize; ++i ) { mMemory[ i ] = 0x0U; }
}

TerminalScreen::~TerminalScreen( )
{
	setPosition( 0, 0 );

#ifndef __linux__
	CloseHandle( mStdOut );
#endif

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
//#warning Use console functions here on windows TODO
#if 1 //__linux__
		setPosition( xX, xY );

		cout << ccolor( c.getFG( )
				, c.getBG( )
				, c.getModifier( ) )

			<< c.getChar( )

			<< std::flush;
#else

		DWORD written;
		TCHAR const ascii = c.getChar();
		if ( FillConsoleOutputCharacter( mStdOut, ascii, 1, { xX, xY }, &written) == FALSE )
		{
			std::cout << "FillCOnsolePutCharacter() Failed. " << GetLastError() << std::endl;
			exit( -1 ); 
		}
#endif
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
#ifdef __linux__
	cout << "\033[" << +xX << ";" << +xY << "m";
#else
	COORD const position = { xX > mW ? mW : xX, xY > mH ? mH : xY };

#if 0
	if (mStdOut == INVALID_HANDLE_VALUE) 
	{
		cout << "Invalid handle!" << endl;
		// throw exception
		exit( -1 );
	}
        if( SetConsoleCursorPosition( mStdOut, position ) == ERROR_INVALID_PARAMETER) 
        {
		cout << "!!! Failed to set position to " << (int)xX << ", " << (int)xY << "." << endl;
		cout << "GetLastError()" << GetLastError() << endl;
		// TODO: throw exception
		exit(-1 );
        }
#endif

#endif
}

void TerminalScreen::fill( TTYC const xColor )
{
	for ( unsigned int y = 0; y < mH; ++y )
	{
		for ( unsigned int x = 0; x < mW; ++x )
		{
			setPixel( x, y, xColor );
		}
	}
}

#ifndef __linux__
// purposefully break this for linux build so we can fix it 
void TerminalScreen::clearScreen( char const xAttribute ) {
    COORD pos = { 0, 0};
    DWORD written;
    unsigned size;

    size = mW * mH;

    FillConsoleOutputCharacter( mStdOut, ' ', size, pos, &written);
    FillConsoleOutputAttribute( mStdOut, xAttribute, size, pos, &written);
    SetConsoleCursorPosition( mStdOut , pos );
//    cout << std::string( mH, '\n' );
}
#endif

void TerminalScreen::refresh( )
{
//	setPosition( 0, 0 );
//	clearScreen(0x71);
#ifndef __linux__
	//#warning - assumes mingw
	system("clear");
#endif
	for ( unsigned int y = 0; y < mH; ++y )
	{
		for ( unsigned int x = 0; x < mW; ++x )
		{
			blitPixel( x, y );
		}
	}
#ifdef __linux__
	cout << std::flush;
#endif
}

void TerminalScreen::setPixel( uint8_t const xX, uint8_t const xY, TTYC const xColor )
{
	if ( xX < mW && xY < mH )
	{
		setPosition( xX, xY );

		TTYCTransform c( xColor );
	
//#ifdef __linux__
#if 1
		cout 	<< ccolor( c.getFG( )
				, c.getBG( )
				, c.getModifier( ) )

			<< c.getChar( );
#else
		char c2 = c.getChar();
		DWORD charsWritten = 0;
		BOOL const result = WriteConsoleOutputCharacter( mStdOut, &c2, 1, { xX, xY }, &charsWritten);
		if (!result) 
		{
			cout << "WriteConsoleOutputCharacter failed!" << endl;
			exit(-1);
		}
	
#endif
	}
	else
	{
		cout << "Invalid position " << (int)xX << "," << (int)xY << " in " 
			  << mW << "," << mH << " cartesian space."  << endl;
		exit(-1);
	}
}

} // namespace sisu
