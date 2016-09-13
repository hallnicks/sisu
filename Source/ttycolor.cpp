#include "ttycolor.hpp"

namespace sisu
{

namespace { static char const sTerm = 27; }

ccolor::ccolor( eTTYColor const xFG
		, eTTYColor const xBG
		, eTTYModifier const xAttr )
	: mMod( xAttr )
	, mFG( xFG )
	, mBG( xBG )
{
	;
}

ccolor::~ccolor( )
	{ std::cout << sTerm << "[" << 0 << "m" << std::flush; }

ccolor & ccolor::operator << ( endl_t xStream )
	{ return (*this) << xStream; }

std::ostream & ccolor::operator >> ( std::ostream & xStream ) const
{
	static int const bgOffset = 30;
	static int const fgOffset = 40;

	if ( mFG != eTTYCNone )
	{
		xStream << sTerm
			<< "["
			<< TTYCMap::getModifier( mMod )
			<< ";"
			<< ( fgOffset + TTYCMap::getColor( mFG ) );

		if ( mBG != eTTYCNone )
		{
			xStream << ";" << ( bgOffset + TTYCMap::getColor( mBG ) );
		}

		xStream << "m" << std::flush;
	}

	return xStream;
}

} // namespace
