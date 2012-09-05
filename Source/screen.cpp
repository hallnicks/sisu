#include "screen.hpp"

namespace sisu
{

Scanline Screen::operator[ ] ( uint8_t const xY ) const
{
	static TTYC DeadChar = 0;
	static Scanline Deadline( &DeadChar, 1 );

	if ( xY < getWidth( ) )
	{
		return Scanline( scanLine( xY ), getWidth( ) );
	}

	return Deadline;
}


} // namespace sisu
