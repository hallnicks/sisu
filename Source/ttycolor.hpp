#ifndef TTYCOLOR_17E6A430_32EA_4026_9A68_44C04B34E195_HPP_
#define TTYCOLOR_17E6A430_32EA_4026_9A68_44C04B34E195_HPP_

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <iostream>

#include "ttyc.hpp"

namespace sisu
{

template<typename XChar>
class basic_ttycolor
{
	eTTYColor mFG, mBG;

	eTTYModifier mMod;

	static char const sTerm = 27;

//	basic_ttycolor( basic_ttycolor const & );

//	basic_ttycolor & operator=( basic_ttycolor const & );

	template<typename YChar>
	struct is_wide { static bool const yes = (sizeof(YChar) > 1); };

	typedef std::basic_ostream< XChar, std::char_traits< XChar > > cout_t;

	cout_t & mOut;

	public:
		basic_ttycolor( eTTYColor xFG
				, eTTYColor xBG
				, eTTYModifier xAttr
				, std::ostream & xOut = std::cout )
			: mMod( xAttr )
	                , mFG( xFG )
	                , mBG( xBG )
	                , mOut( xOut )
		{
		        ;
		}

                typedef cout_t & ( * endl_t )( cout_t & );

		~basic_ttycolor( ) { mOut << sTerm << "[" << 0 << "m" << std::flush; }

                template< typename T >
                basic_ttycolor & operator << ( T & xStream )
                {
			mOut << xStream;
			return (*this);
                }

                basic_ttycolor & operator << ( endl_t xStream )
		{
		        return (*this) << xStream;
		}

		std::ostream & operator >> ( std::ostream & xStream ) const
		{
		        static int const bgOffset = 30;
		        static int const fgOffset = 40;

		        if ( mFG != eTTYCNone )
		        {
		                xStream	<< sTerm
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
};

template<typename XCharType>
std::ostream & operator << ( std::ostream & xStream, basic_ttycolor<XCharType> const & xColor )
{
        return xColor >> xStream;
}

typedef basic_ttycolor<char> ccolor;

} // namespace sisu

#endif // TTYCOLOR_17E6A430_32EA_4026_9A68_44C04B34E195_HPP_
