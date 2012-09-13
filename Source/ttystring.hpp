#ifndef TTYSTRING_9C189A09_9BDB_4AC1_B397_80643B6E70BF_HPP_
#define TTYSTRING_9C189A09_9BDB_4AC1_B397_80643B6E70BF_HPP_

#include "ttycolor.hpp"
#include "bits.hpp"

#include <cstdlib>
#include <iostream>

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
				std::cout << "character to be set:" << bits<char>( xStr [ i ] ) << std::endl;

				std::cout << "TTYC before set:" << bits<TTYC>( t.getRaw( ) ) << std::endl;
				std::cout << "char before set:" << bits<char>( t.getChar( ) ) << std::endl;

				t.setChar( xStr [ i ] );

				std::cout << "TTYC after set:" << bits<TTYC>( t.getRaw( ) ) << std::endl;
				std::cout << "after set:" << bits<char>( t.getChar( ) ) << std::endl;

				while ( 1 ) { }

	                        mStr [ i ] = t.getRaw( );
	                }
		}

		TTYC operator [ ] ( size_t const xIndex ) const { return mStr [ xIndex ]; }
};

} // namespace sisu

#endif // TTYSTRING_9C189A09_9BDB_4AC1_B397_80643B6E70BF_HPP_
