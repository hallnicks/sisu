#ifndef TTYSTRING_9C189A09_9BDB_4AC1_B397_80643B6E70BF_HPP_
#define TTYSTRING_9C189A09_9BDB_4AC1_B397_80643B6E70BF_HPP_

#include "ttycolor.hpp"

#include <cstdlib>

namespace sisu
{

template< size_t XSize >
class TTYString
{
        TTYC mStr [ XSize ];

	public:
		TTYString( char const ( & xStr ) [ XSize ], TTYC const xC )
		{
			TTYCTransform t( xC );

	                for ( unsigned int i = 0; i < XSize; ++i )
        	        {
				t.setChar( xStr [ i ] );
				TTYC raw = t.getRaw( );
	                        mStr [ i ] = raw;
	                }
		}

		TTYC operator [ ] ( size_t const xIndex ) const { return mStr [ xIndex ]; }
};

} // namespace sisu

#endif // TTYSTRING_9C189A09_9BDB_4AC1_B397_80643B6E70BF_HPP_
