#ifndef TTYSTRING_9C189A09_9BDB_4AC1_B397_80643B6E70BF_HPP_
#define TTYSTRING_9C189A09_9BDB_4AC1_B397_80643B6E70BF_HPP_

#include "ttycolor.hpp"

#include <cstdlib>

namespace sisu
{

template< size_t XSize >
struct TTYString
{
        TTYString( char const * const xBuffer, TTYC const xColor )
        {
                for ( unsigned int i = 0; i < XSize; ++i )
                {
                        STR [ i ] = ( xBuffer [ i ] << 24 ) | xColor;
                }
        }

        TTYC STR [ XSize ];
};

} // namespace sisu

#endif // TTYSTRING_9C189A09_9BDB_4AC1_B397_80643B6E70BF_HPP_
