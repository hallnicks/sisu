#ifndef WORD_994C7683_1BE8_4B4C_A725_D89633A03FDA_HPP_
#define WORD_994C7683_1BE8_4B4C_A725_D89633A03FDA_HPP_

// The author is aware of std::bitset
// This is written this way for educational purposes and platform-specific controls such as branching.
#include "accept_cout.hpp"
#include "bits.hpp"
#include <algorithm>
#include <climits>

namespace sisu
{

inline static bool isBigEndian(void)
{
	static const union { uint32_t const i; char const c [ 4 ]; } u = { 0x01010405 };
	return u.c [ 0 ] == 1;
}

#define ACCEPTOPS(xT, xV)\
template< typename XOtherType > xT < XType > & operator ~  ( ) { xV = ~xV;  return (*this); }\
template< typename XOtherType > xT < XType > & operator !  ( ) { xV = !xV;  return (*this); }\
template< typename XOtherType > xT < XType > & operator == ( XOtherType const & xRef ) { xV == xRef;  return (*this); }\
template< typename XOtherType > xT < XType > & operator <  ( XOtherType const & xRef ) { xV < xRef;   return (*this); }\
template< typename XOtherType > xT < XType > & operator >  ( XOtherType const & xRef ) { xV > xRef;   return (*this); }\
template< typename XOtherType > xT < XType > & operator *  ( XOtherType const & xRef ) { xV *= xRef;  return (*this); }\
template< typename XOtherType > xT < XType > & operator /  ( XOtherType const & xRef ) { xV /= xRef;  return (*this); }\
template< typename XOtherType > xT < XType > & operator *= ( XOtherType const & xRef ) { xV *= xRef;  return (*this); }\
template< typename XOtherType > xT < XType > & operator /= ( XOtherType const & xRef ) { xV /= xRef;  return (*this); }\
template< typename XOtherType >	xT < XType > & operator %  ( XOtherType const & xRef ) { xV %= xRef;  return (*this); }\
template< typename XOtherType >	xT < XType > & operator -  ( XOtherType const & xRef ) { xV -= xRef;  return (*this); }\
template< typename XOtherType > xT < XType > & operator +  ( XOtherType const & xRef ) { xV += xRef;  return (*this); }\
template< typename XOtherType >	xT < XType > & operator &  ( XOtherType const & xRef ) { xV &= xRef;  return (*this); }\
template< typename XOtherType > xT < XType > & operator |  ( XOtherType const & xRef ) { xV |= xRef;  return (*this); }\
template< typename XOtherType > xT < XType > & operator ^  ( XOtherType const & xRef ) { xV ^= xRef;  return (*this); }\
template< typename XOtherType >	xT < XType > & operator << ( XOtherType const & xRef ) { xV <<= xRef; return (*this); }\
template< typename XOtherType > xT < XType > & operator >> ( XOtherType const & xRef ) { xV >>= xRef; return (*this); }

template<typename XType>
struct ShiftableTypeMap
{
	ShiftableTypeMap( XType & xRef ) : value( xRef ) { }
        XType & value;
        typedef XType type;
};

#define SPECBITS(xType, xShiftableAndableType, X_cast)\
template<>\
struct ShiftableTypeMap<xType>\
{\
	ShiftableTypeMap( xType const & xRef ) : value( X_cast< xShiftableAndableType >( xRef ) ) { ; }\
        xShiftableAndableType value;\
        typedef xShiftableAndableType type;\
}

SPECBITS(double, uint64_t, static_cast);
SPECBITS(float, uint32_t, static_cast);

#undef SPECBITS

template< typename XType >
class bit
{
	XType & mV;

	XType const mM;

	size_t const mB;

	public:
		bit( XType & xV, size_t const xByteOffset, size_t const xBitOffset )
			: mV( xV )
			, mM ( mask( xByteOffset, xBitOffset ) )
			, mB( xByteOffset )
		{
			;
		}

		bit & operator = ( bool const xSet )
		{
			setBits( mV, mM, xSet );
			return ( *this );
		}

		bool operator [ ] ( size_t const xBitOffset )
			{ return ( mV & mask( mB, xBitOffset ) ); }
 
		static XType mask( size_t const xByteOffset, size_t const xBitOffset )
			{ return 1 << ( ( xByteOffset * CHAR_BIT ) + xBitOffset ); }

		static void setBits( XType & xV, XType const xM, bool const xS = true )
			{ xV = ( xV & ~xM ) | ( -( xS ? 1 : 0 ) & xM ); }
};

template< typename XType >
class byte
{
	XType & mV;

	size_t const mShift;

	public:
		byte( XType & xV, size_t const xIdx )
			: mV( xV )
			, mShift( std::min( xIdx, sizeof( XType ) ) * CHAR_BIT )
		{
			;
		}

		byte < XType > & operator = ( uint8_t const & xByte )
		{
			mV |= ( xByte << mShift );
			return ( *this );
		}

		bit < XType > operator [ ] ( size_t const xIdx )
			{ return bit< XType > ( mV, xIdx ); }

		static XType mask( size_t const xByteOffset )
			{ return 0xFF << xByteOffset; }
};

template < typename XType, bool XSpaced = true > class word;
template < typename XType, bool XSpaced >
class word : public accept_cout < word < XType > >
{
	XType mV;

	public:
		word( XType const & xInitialValue )
			: mV ( xInitialValue ) { ; }

		word < XType > & operator = ( XType const & xRhs )
		{
			mV = xRhs;
			return ( *this );
		}

	        std::ostream & operator >> ( std::ostream & xS ) const
		{
                        static const uint32_t sB = sizeof( XType ) * CHAR_BIT + 1 + ( XSpaced ? sizeof( XType ) : 0 );

                        uint8_t c[ sB ];

                        uint8_t * p = c + ( sB - 1 );

			typename ShiftableTypeMap< XType >::type a( mV );

                        int d = 0;

                        for ( int8_t i = sB - 2; i >= 0; --i )
                        {
                                if ( XSpaced && ( d++ % CHAR_BIT ) == 0 )
                                {
                                        *p-- = ' ';
                                }

                                *p-- = ( a & 1 ) + '0';

                                a >>= 1;
                        }

                        c[ sB - 1 ] = '\0';

                        return xS << c;
		}

		static size_t size( ) { return sizeof( XType ); }

		byte < XType > operator [ ] ( size_t const xIdx )
			{ return byte < XType > ( mV, xIdx ); }

		ACCEPTOPS(word, mV)
};

template < typename XType >
inline std::ostream & operator << ( std::ostream & xS, word < XType > const & xC )
        { return xC >> xS; }

} // namespace sisu

#endif // WORD_994C7683_1BE8_4B4C_A725_D89633A03FDA_HPP_
