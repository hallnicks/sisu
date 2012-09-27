#ifndef TTYCOLOR_17E6A430_32EA_4026_9A68_44C04B34E195_HPP_
#define TTYCOLOR_17E6A430_32EA_4026_9A68_44C04B34E195_HPP_

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include "ttyc.hpp"

namespace sisu
{

class ccolor
{
	eTTYColor mFG, mBG;

	eTTYModifier mMod;

	typedef std::basic_ostream<char, std::char_traits< char > > cout_t;

	public:
		ccolor( eTTYColor const xFG
			, eTTYColor const xBG
			, eTTYModifier const xAttr );

		~ccolor( );

                template< typename T >
                ccolor & operator << ( T & xStream )
                	{ std::cout << xStream; return (*this); }

                typedef cout_t & ( * endl_t )( cout_t & );

                ccolor & operator << ( endl_t xStream );

		std::ostream & operator >> ( std::ostream & xStream ) const;
};

inline std::ostream & operator << ( std::ostream & xS, ccolor const & xC )
	{ return xC >> xS; }

} // namespace sisu

#endif // TTYCOLOR_17E6A430_32EA_4026_9A68_44C04B34E195_HPP_
