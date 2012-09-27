#ifndef BITS_DE20CC90_8F6F_497C_981B_7537B10BA529_HPP_
#define BITS_DE20CC90_8F6F_497C_981B_7537B10BA529_HPP_

#include <stdint.h>
#include <iostream>
#include <limits>
#include <cstring>
#include <clocale>
#include <climits>

#include "nibble.hpp"

namespace sisu
{

template<typename XType>
struct BitsType
{
	BitsType( XType const & xRef ) : value( xRef ) { }
	XType value;
	typedef XType type;
};

#define SPECBITS(xType, xShiftableAndableType, X_cast)\
template<>\
struct BitsType<xType>\
{\
	BitsType( xType const & xRef )\
		: value( X_cast< xShiftableAndableType >( xRef ) ) { ; }\
	xShiftableAndableType value;\
	typedef xShiftableAndableType type;\
}

SPECBITS(double, uint64_t, static_cast);
SPECBITS(float, uint32_t, static_cast);

#undef SPECBITS

template< typename XType, bool XSpaced = true >
class bits
{
        static const size_t sT = sizeof( XType ) * CHAR_BIT;

	XType & mRef;

	class nibbler
	{
		static const uint32_t sMask = 0x000000FF, fMask = 0x00FFFFFF;

		XType & mRef;

		size_t const mShift;

		public:
			nibbler( size_t const xIndex, XType & xRef )
				: mRef( xRef )
				, mShift( xIndex * CHAR_BIT )
			{
				;
			}


			uint8_t & operator [ ] ( eUpperLower const xUL )
			{
				//	return xUL == eUL_Lower ? mB & 0x0F : mB & 0xF0;
				static uint8_t hoge;
				return hoge;
			}

			nibbler & operator = ( uint8_t const & xRhs )
			{
//				mRef = ( xRhs << mShift & ) | mRef;
				return (*this);
			}

	};

        public:
                bits( XType & xRef )
			: mRef( xRef )
                {
			;
                }

                bits( XType const & xRef )
			: mRef( const_cast<XType & >( xRef ) )
                {
			;
                }

                ~bits( ) { }

                std::ostream & operator >> ( std::ostream & xS ) const
		{
		        static const uint32_t sB = sT + 1 + ( XSpaced ? sizeof( XType ) : 0 );

		        uint8_t mB[ sB ];

                        uint8_t * xB = mB + ( sB - 1 );

			typename BitsType<XType>::type a = BitsType<XType>( mRef ).value;

                        int d = 0;

                        for ( int8_t i = sB - 2; i >= 0; --i )
                        {
                                if ( XSpaced && d++ % CHAR_BIT == 0 )
				{
					*xB-- = ' ';
				}

                                *xB-- = ( a & 1 ) + '0';

                                a >>= 1;
                        }

                        mB[ sB - 1 ] = '\0';

			return xS << mB;
		}

		static size_t numberOfBytes( ) { return sT; }

		nibbler operator [ ] ( size_t const xIndex )
			{ return nibbler( xIndex, mRef ); }
};

template< typename XType, bool XSpaces >
inline std::ostream & operator << ( std::ostream & xS, bits< XType, XSpaces > const & xBits )
	{ return xBits >> xS; }

} // namespace sisu


#endif // BITS_DE20CC90_8F6F_497C_981B_7537B10BA529_HPP_
