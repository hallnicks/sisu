#ifndef BITS_DE20CC90_8F6F_497C_981B_7537B10BA529_HPP_
#define BITS_DE20CC90_8F6F_497C_981B_7537B10BA529_HPP_

#include <stdint.h>
#include <iostream>
#include <limits>
#include <cstring>
#include <clocale>
#include <climits>

namespace sisu
{

#if 0
template<typename XType>
struct BitsType
{
	BitsType( XType const & xRef ) : value( xRef ) { }
	XType value;
	typedef XType type;
};

#define SPECBITS(xType, xShiftableAndable, X_cast)\
template<>\
struct BitsType<xType>\
{\
	BitsType( xType const & xRef )\
		: value( X_cast<xShiftableAndable>( xRef ) ) { }\
	xShiftableAndable value;\
	typedef xShiftableAndable type;\
}

//SPECBITS(double, uint64_t, reinterpret_cast);
//SPECBITS(float, uint32_t, static_cast);

#undef SPECBITS

#endif // 0

template< typename XType, bool XSpaced = true >
class bits
{
        static const uint32_t sB = sizeof(XType) * CHAR_BIT + 1 + ( XSpaced ? sizeof(XType) : 0 );

        char mB[ sB ];

        public:
                bits( XType const & xRef ) : mB( )
                {
                        char * xB = mB + ( sB - 1 );

			#if 0
			typedef typename BitsType<XType>::type local_int;
			local_int a = BitsType<XType>::value;
			#endif

			XType a = xRef;

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
                }

                ~bits( ) { }

                typedef std::basic_ostream< char, std::char_traits< char > > cout_t;

                std::ostream & operator >> ( std::ostream & xS ) const { return xS << mB; }

                template<typename T>
		bits & operator << ( T const & xS ) { std::cout << xS; return (*this); }

                typedef cout_t & ( * endl_t )( cout_t & );
                std::ostream & operator << ( endl_t const & xS ) { return (*this) << xS; }
};

template<typename XType, bool XSpaces>
inline std::ostream & operator << ( std::ostream & xS, bits<XType, XSpaces> const & xBits )
{
	return xBits >> xS;
}

} // namespace sisu


#endif // BITS_DE20CC90_8F6F_497C_981B_7537B10BA529_HPP_
