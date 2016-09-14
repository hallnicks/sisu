#if 0
#include "test.hpp"
#include "word.hpp"
#include <unistd.h>
#include <iostream>

// TODO - Fix Deficiencies: Assumes little endian for now. Need to think of a good way to mock up endianness from the compiler side.
using sisu::word;

namespace
{

class word_UT : public context
{
	public:
		word_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

template < typename XIntegralType, size_t XSize >
void print( word< XIntegralType > const ( & xArr ) [ XSize ] )
	{ for ( size_t i = 0; i < XSize; ++i )
		{ std::cout << xArr [ i ]; } }

template < typename XIntegralType, size_t XSize >
void shift( word < XIntegralType > ( & xWord ) [ XSize ] )
	{ for ( size_t i = 0; i < XSize; ++i )
		{ for ( size_t i = 0; i < sizeof( XIntegralType ) * CHAR_BIT; ++i )
			{ xWord [ i ] << i;
				print( xWord ); }
					for ( size_t i = 0; i < sizeof( XIntegralType ) * CHAR_BIT; ++i )
						{ xWord [ i ] >> i;
							print( xWord ); } } }

template < typename XIntegralType, size_t XSize >
void divide( word < XIntegralType > ( & xWord ) [ XSize ] )
	{ for ( size_t i = 0; i < XSize; ++i )
		{ for ( size_t i = 0; i < sizeof( XIntegralType ) * CHAR_BIT; ++i )
			{ xWord [ i ] /= i;
				print( xWord ); }
					for ( size_t i = 0; i < sizeof( XIntegralType ) * CHAR_BIT; ++i )
						{ xWord [ i ] *= i;
							print( xWord ); } } }
} // namespace

TEST(word_UT, words_cout)
{
	static int const maxCount = 100;
	int count = 0;
	for (int i = 1; i != 0; i <<= 1 )
		{ ++count; std::cout << word< int32_t >( i ) << std::endl; if(count > maxCount) break; }

	count =0;
	for (int i = 1; i != 0; i >>= 1 )
		{ ++count; std::cout << word< int32_t >( i ) << std::endl; if(count > maxCount) break; }

	#define W(xV) word< uint8_t >( xV )
	static word< uint8_t > uint8[] = { W( 0xFF ), W( 0xE0 ), W( 0xBE ), W( 0xEA ), W( 0xCA ), W( 0xA1 ), W( 0x11 ) };
	#undef W

	#define W(xV) word< int8_t >( xV )
	static word< int8_t > int8[] = { W( 0xFF ), W( 0x0E ), W( 0xEE ), W( 0xDE ), W( 0xBE ), W( 0x1E ), W( 0xDB ) };
	#undef W

	#define W(xV) word< uint16_t >( xV )
	static word< uint16_t > uint16[] = { W( 0xFFFF ), W( 0xE0E0 ), W( 0xDEAD ), W( 0xCEA5 ), W( 0xBABE ), W( 0xAB1E ), W( 0xBA11 ) };
	#undef W

	#define W(xV) word< int16_t >( xV )
	static word< int16_t > int16[] = { W( 0xFFFF ), W( 0x0E0E ), W( 0xBEEF ), W( 0xDECA ), W( 0xCAFE ), W( 0x5CA1 ), W( 0x80DD ) };
	#undef W

	#define W(xV) word< int32_t >( xV )
	static word< int32_t > int32[] = { W( 0xFFFFFFFF ), W( 0xE0E0E0E0 ), W( 0xDEADBEEF ), W( 0xDECEA5ED )
					 , W( 0xCAFEBABE ), W( 0x5CA1AB1E ), W( 0x80DDBA11 ) };
	#undef W

	#define W(xV) word< uint32_t >( xV )
	static word< uint32_t > uint32[] = { W( 0xCAFEBABE ), W( 0x5CA1AB1E ), W( 0x80DDBA11 ), W( 0xFFFFFFFF )
					   , W( 0xE0E0E0E0 ), W( 0xDEADBEEF ), W( 0xDECEA5ED ) };
	#undef W

	// assumes 32 bit float for now.
	#define W(xV) word< float >( xV )
	static word< float > floats[] = { W( 0xCAFEBABE ), W( 0xE0E0E0E0 ), W( 0xDEADBEEF ), W( 0xDECEA5ED )
					, W( 0x5CA1AB1E ), W( 0x80DDBA11 ), W( 0xFFFFFFFF ) };
	#undef W

	#define W(xV) word< uint64_t >( xV )
	static word< uint64_t > uint64[] = { W( 0xCEA5E1E550B57AC1 )
					   , W( 0xE5DEF1EC7ED5EA1E )
					   , W( 0xD5ECEDEDB007AB1E )
					   , W( 0xFEEB1EFA111B1EFA )
					   , W( 0x151F1AB1ED1AB011 )
					   , W( 0xCA13217397997799 )
					   , W( 0x1391937197734969 ) };
	#undef W

	#define W(xV) word< int64_t >( xV )
	static word< int64_t > int64[] = { W( 0xCEA5E1E550B57AC1 )
					 , W( 0xCA13217397997799 )
					 , W( 0xD5ECEDEDB007AB1E )
					 , W( 0x151F1AB1ED1AB011 )
					 , W( 0xE5DEF1EC7ED5EA1E )
					 , W( 0xFEEB1EFA111B1EFA )
					 , W( 0x1391937197734969 ) };
	#undef W

	#define W(xV) word< double >( xV )
	static word< double > doubles[] = { W( 0xCEA5E1E550B57AC1 )
					  , W( 0xFEEB1EFA111B1EFA )
					  , W( 0xD5ECEDEDB007AB1E )
					  , W( 0xE5DEF1EC7ED5EA1E )
					  , W( 0x151F1AB1ED1AB011 )
					  , W( 0xCA13217397997799 )
					  , W( 0x1391937197734969 ) };
	#undef W

	shift( uint8 );

	shift( int8 );

	shift( uint16 );

	shift( int16 );

	shift( int32 );

	shift( uint32 );

	divide( floats );

	shift( int64 );

	shift( uint64 );

	divide( doubles );
}

TEST(word_UT, words_set)
{
	static uint8_t bytes[] = { 0xFF, 0xFE, 0xFD, 0xFC };
	word< int32_t > p( 0x00000000 );

	MUSTEQ(sizeofarray(bytes), sizeof(int32_t))

	for ( size_t i = 0; i < p.size( ); ++i )
	{
		std::cout << p << " => ";
		p [ i ] = bytes [ i ];
		std::cout << p << std::endl;
	}
}
#endif
