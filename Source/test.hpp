//    This file is part of sisu.

//    sisu is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    sisu is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Pueblic License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.
#ifndef TEST_F88C97663CF44CE9B325FF50DC9B2C18_HPP_
#define TEST_F88C97663CF44CE9B325FF50DC9B2C18_HPP_

#include <algorithm>
#include <csignal>
#include <cstdio>
#include <iostream>
#include <limits>

#include "signal.hpp"
#include "stacktrace.hpp"
#include "ttycolor.hpp"
#include "typename.hpp"

#define KEYPRESS { constr f = __FILE__; constr fn = __PRETTY_FUNCTION__; unsigned long l = __LINE__; char c = 0; std::cout << "Paused: " << f << ":" << l << ":" << fn << std::endl; do { } while(c = std::cin.get() && c == 0); std::cout << "Resuming: " << f << ":" << l << ":" << fn << std::endl; }

typedef char const * const constr;

template<typename XType, size_t XSize >
inline static size_t sizeofarray( XType const ( & xDest ) [ XSize ] ) { return XSize; }

inline unsigned int Time( )
{
	static struct And
	{
		And( ) { srand( time( NULL ) ); }

		static inline unsigned int thePersistenceThereOf( )
		{
			return std::numeric_limits<size_t>::max( ) % rand( );
		}

	} SDFJDID;

	return And::thePersistenceThereOf( );
}

template< typename XType, size_t XDestSize, size_t XSourceSize >
void const_memcopy( XType ( & xDestination ) [ XDestSize ], XType const ( & xSource ) [ XSourceSize ], size_t xBytesToCopy = XSourceSize )
{
	size_t i = 0;

	for ( ; i < std::min( xBytesToCopy, std::min( XSourceSize, XDestSize ) ); ++i )
	{
		xDestination[ i ] = xSource[ i ];
	}

	xDestination[ i < XDestSize - 1 ? i + 1 : XDestSize - 1 ] = '\0';
}

template< typename XType, size_t XSize = Time( ) >
class constmemory
{
	XType mDest[ XSize ];

	constmemory( constmemory const & xRef ) { }

	constmemory & operator=( constmemory const & xRef );

	public:
		template< size_t XOtherSize >
		constmemory( XType ( & xSrc ) [ XOtherSize ] ) : mDest( )
		{
			for ( unsigned int i = 0; i < std::min( XOtherSize, XSize ); ++i )
			{
				mDest[ i ] = xSrc[ i ];
			}
		}

		~constmemory( ) { ; }

		typedef XType const * const ReadOnlyPtr;

		ReadOnlyPtr operator*( ) { return const_cast<ReadOnlyPtr>( mDest ); }
};

class context
{
	public:
		inline context( ) { }
		virtual ~context( ) { }
		virtual void Up ( ) = 0;
		virtual void Down( ) = 0;
};

inline constr HL( )
{
	static char const array[548] = "°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸"
						"°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸"
							"°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸"
								"°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸"
									"°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸ ";
	static char ret[300] = { 0, };


	unsigned int const upperbound = 90 + ( rand( ) % 207 );

	unsigned int i = 0;

	for ( ; i < upperbound; ++i ) { ret[ i ] = array[ i ]; }

	ret[ i + 1 ] = '\0';

	return ret;
}

inline sisu::ccolor const CHL( ) { return sisu::ccolor( sisu::eTTYCBlack, static_cast<sisu::eTTYColor>( ( rand( ) % 7 ) + 2 ), sisu::eModBold ); }
inline sisu::ccolor const CTXT( ) { static const sisu::ccolor cTXT( sisu::eTTYCBlack, sisu::eTTYCGreen, sisu::eModBold ); return cTXT; }
inline void LINE( ) { std::cout << CHL( ) << HL( ) << CTXT( ) << std::endl; }

template< class XContext >
class test : public XContext
{
	public:
		inline test( ) : XContext( ) { }
		inline ~test( ) { }
		template< typename XUnitTest > void Body( );
		template<typename XUnitTest> struct Unit;
		template< typename XUnitTest >
		void ExecuteTest( )
		{
			std::cout << "[ Set Name : " 		<< Unit<XUnitTest>::TestName( ) 	<< " ]" << std::endl;
			std::cout << "[ Test Name : " 		<< Unit<XUnitTest>::UnitName( ) 	<< " ]" << std::endl;
			std::cout << "[ Time Registered : " 	<< Unit<XUnitTest>::TimeRegistered( ) 	<< " ]" << std::endl;
			std::cout << "[ Date Registered : " 	<< Unit<XUnitTest>::DateRegistered( ) 	<< " ]" << std::endl;
			std::cout << "[ File : " 		<< Unit<XUnitTest>::File( ) 		<< " ]" << std::endl;
			std::cout << "[ Function : " 		<< Unit<XUnitTest>::Func( ) 		<< " ]" << std::endl;
			std::cout << "[ Line " 			<< Unit<XUnitTest>::Line( ) 		<< " ]" << std::endl;

			timespec ts;
			ts.tv_sec = 0;
			ts.tv_nsec = 0;

			clock_settime( CLOCK_PROCESS_CPUTIME_ID, &ts );
			Body<XUnitTest>( );
			clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts );

			LINE( );
			std::cout 	<< "[ Finished execution of "
						<< Unit<XUnitTest>::UnitName( )
						<< ts.tv_sec
						<< ","
						<< ts.tv_sec
					<< " ]" << std::endl;
			LINE( );

			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << std::endl;
			std::cout << std::endl;
		}
};

namespace sisu
{

enum eTestResult
{
	eExceptionInConstructor = 0,
	eExceptionInSetup 	= 1,
	eExceptionInTest 	= 2,
	eExceptionInTearDown 	= 3,
	eSuccess 		= 4,
	eUninitialized		= 5,
};

typedef void ( *XF )( );

template < typename T >
class chain
{
	struct N
	{
		inline N( ) : V( 0 ), O( 0 ) { }

		inline ~N( ) { if ( O != 0 ) { delete O; } }

		N * O; T V;

	} mH, * mC;

	public:
		inline chain( ) : mH( ), mC( &mH ) { }

		inline ~chain( ) { }

		inline void push_back( T & xV ) { mC->V = xV; mC = mC->O = new N; }

		template< typename XTransmitter >
		inline void operator * ( XTransmitter & xRef )
		{
			N * n = &mH; do { if ( n->V != 0 ) { (xRef.*n->V)( ); } } while( ( n = n->O ) != 0 );
		}

		template< typename XReceiver >
		inline void operator / ( XReceiver & xRef )
		{
			N * n = &mH; do { if ( n->V != 0 ) { xRef( n->V ); } } while( ( n = n->O ) != 0 );
		}

		inline void operator ( ) ( )
		{
			N * n = &mH; do { if ( n->V ) { ( *n->V )( ); } } while( ( n = n->O ) != 0 );
		}
};

template< typename T, typename XStorage >
inline XStorage & UnitTest( void ( test< T >::*XF ) ( ) = 0 )
{
	static XStorage instance;
	if ( XF != 0 ) { instance.push_back( XF ); }
	return instance;
}

inline void HookSignals( )
{
	static StackTraceHandler hook;
	hook( );
}

inline void ShowResult( eTestResult r )
{
	static constr messages[] =
	{
		 	"Exeception in constructor."
		, 	"Exception in setup."
		,	"Exception in test."
		, 	"Exception in teardown."
		, 	"Success!"
		, 	"Not initialized."
	};

	// TODO - Add Bitwise support
	static sisu::eTTYModifier mods[] =
	{
			sisu::eModBold
		,	sisu::eModBold
		,	sisu::eModBold
		,	sisu::eModBold
		,	sisu::eModBlink
		, 	sisu::eModBold
	};

	static sisu::eTTYColor fgs[] =
	{
			sisu::eTTYCRed
		,	sisu::eTTYCRed
		,	sisu::eTTYCRed
		,	sisu::eTTYCRed
		,	sisu::eTTYCBlack
		,	sisu::eTTYCRed
	};

	static sisu::eTTYColor bgs[] =
	{
			sisu::eTTYCBlack
		,	sisu::eTTYCBlack
		,	sisu::eTTYCBlack
		,	sisu::eTTYCBlack
		,	sisu::eTTYCGreen
		,	sisu::eTTYCBlack
	};

	sisu::ccolor const cMSG( bgs[ r ], fgs[ r ], mods[ r ] );

	std::cout << " Result is: " << cMSG << messages[ static_cast<uint8_t>(r) ] << std::endl;
}

inline void MSG( constr xMessage ) { std::cout << "[ " << xMessage << " ...";  }
inline void OK( ) { std::cout << " OK ]" << std::endl; }
inline void FAIL( ) { std::cout << " FAILED ]" << std::endl; }

template<typename XUnitTestSet>
struct UnitTestExecute
{
	template<typename XUnitTestFunc>
	void operator( )( XUnitTestFunc xXF )
	{
		LINE( );
		std::cout << "[ Running tests without sisu ... ]" << std::endl;
		#if defined(STEPTESTS)
			KEYPRESS;
		#endif
		LINE( );

		eTestResult r = eUninitialized;

		try
		{
			MSG( "Constructing unit test. -sisu." );
			test<XUnitTestSet> t;
			OK( );
#define TRYCATCH(xExpr,xEnum) try { xExpr; } catch( ... ) { std::cout << " FAILED (sisuless) ]" << std::endl; KEYPRESS; r = xEnum; }
			MSG( "Setting up unit test. -sisu." );
			TRYCATCH(t.Up( ), eExceptionInSetup);
			OK( );
			MSG( "Executing unit test. -sisu." );
			TRYCATCH((t.*xXF)( ), eExceptionInTest);
			OK( );
			r = eSuccess;
			MSG( "Tearing down unit test.");
			TRYCATCH(t.Down( ), eExceptionInTearDown);
			OK( );
		}
		catch ( ... )
		{
			r = eExceptionInConstructor;
			FAIL( );
		}

		ShowResult( r );

		if ( r != eSuccess )
		{
			KEYPRESS;
		}

		LINE( );
		std::cout << "[ Done running tests without sisu ... ]" << std::endl;
		LINE( );
	}
};

template< typename XContext >
inline void RunUnitTestSet( )
{
	LINE( );
	std::cout << "[ Running new unit test set .. ]" << std::endl;
	LINE( );

	typedef void(test< XContext >::*XFUnitTest)();
	typedef chain< XFUnitTest > list_t;

	static const int sp = 8;

	static class TestResult
	{
		static inline eTestResult Run(  )
		{
			LINE( );
			std::cout << "[ Starting test run with sisu ... ]" << std::endl;
			LINE( );

			eTestResult r = eUninitialized;

			try
			{
				LINE( );
				MSG("Constructing unit test.");
				OK( );
				test<XContext> t;
				MSG("Setting up unit test.");
				OK( );
				TRYCATCH(t.Up( ), eExceptionInSetup);
				try
				{
					MSG( "Executing unit test" );
					UnitTest< XContext, list_t >( ) * t;
					r = eSuccess;
					OK( );
				}
				catch ( ... )
				{
					FAIL( );
					r = eExceptionInTest;
				}
				MSG( "Tearing down unit test" );
				TRYCATCH(t.Down( ), eExceptionInTearDown);
				OK( );
			}
			catch ( ... )
			{
				FAIL( );
				r = eExceptionInConstructor;
			}

			MSG( "Showing results " );

			ShowResult( r );

			OK( );

			LINE( );

			std::cout << "[ Finished test run with sisu. ]" << std::endl;

			LINE( );

			if ( r != eSuccess )
			{
				KEYPRESS;
			}
		}

	public:
		TestResult( )
		{
			LINE( );

			std::cout << "[ Creating Test Results ]" << std::endl;

			LINE( );

			Run( );

			LINE( );

			std::cout << "[ Finished Test Results ]" << std::endl;

			LINE( );

			UnitTestExecute<XContext> functor;

			UnitTest< XContext, list_t >( ) / functor;
		}

		inline ~TestResult( ) { HookSignals( ); }

#undef TRYCATCH

	} r;

	LINE( );

	std::cout << "[ Finished running unit test set. ]" << std::endl;

	LINE( );
}

inline int XFList( XF xF = 0 )
{
	static chain< XF > s;
	xF != 0 ? s.push_back( xF ) : s( );
	return 0;
}

inline int Execute( )
{
	HookSignals( );

	LINE( );

	std::cout << "[ Invoking XFList ... ]" << std::endl;

	LINE( );

	return XFList( );
}


template< typename T>
inline void Once( XF i ) { static struct o { o( XF i ) { i( ); } } s( i ) ; };

class TestFailedException
{
	constr mMsg;

	protected:
		TestFailedException( constr xMsg )
			: mMsg( xMsg )
		{
			;
		}

		virtual ~TestFailedException( ) { ; }
	public:
		TestFailedException( TestFailedException const & xRhs )
			: mMsg( xRhs.mMsg )
		{
			;
		}

		TestFailedException & operator = ( TestFailedException const & xRhs )
		{
			*this = TestFailedException( xRhs );
		}

		constr getMessage( ) const { return mMsg; }
};

#define SMPLXCPT(xName) class xName : public TestFailedException { public: xName( constr xMsg ) : TestFailedException( xMsg ) { ; } ~xName( ) { } };

SMPLXCPT(DidNotEqualException);
SMPLXCPT(EqualedException);

#undef SMPLXCPT

} // namespace sisu

#define FAIL(xMsg) { throw ::sisu::TestFailedException( xMsg ); }
#define MUSTNEQ( xLHS, xRHS ) if ( xLHS == xRHS ) { throw ::sisu::EqualedException( " #xLhs did not equal #xRhs " ); }
#define MUSTEQ( xLHS, xRHS ) if ( xLHS != xRHS) { throw ::sisu::DidNotEqualException( " #xLhs equaled #xRhs " ); }
#define UTSTORAGE( xUTClass, xUTName ) xUTClass ## xUTName ## _storage_decl
#define UTINSTANCE( xUTClass, xUTName ) s ## xUTClass ## xUTName ## _decl
#define UTCLASS( xUTClass, xUTName ) xUTClass ## _ ## xUTName
#define TEST(xUTClass,xUTName)\
class UTCLASS(xUTClass, xUTName) { private: UTCLASS(xUTClass, xUTName)( ); };\
template<> template<> void test< xUTClass >::Body< UTCLASS(xUTClass, xUTName) >();\
template<> template<> struct test< xUTClass >::Unit< UTCLASS(xUTClass, xUTName) > { \
static constr TimeRegistered( ) { return __TIME__; }\
static constr DateRegistered( ) { return __DATE__; }\
static constr File( ) { return __FILE__; }\
static constr Func( ) { return __PRETTY_FUNCTION__; }\
static int Line( ) { return __LINE__; }\
static constr TestName( ) { return # xUTClass; }\
static constr UnitName( ) { return # xUTName; }\
};\
static class UTSTORAGE(xUTClass, xUTName) {\
	typedef void(test< xUTClass >::*XFUnitTest)();\
	typedef ::sisu::chain< XFUnitTest > list_t;\
	static inline void initContext() { ::sisu::XFList( &::sisu::RunUnitTestSet< xUTClass > ); }\
	static inline void initTest( ) { ::sisu::UnitTest< xUTClass, list_t >( &test< xUTClass >::ExecuteTest< UTCLASS(xUTClass, xUTName) > ); }\
	public:\
		UTSTORAGE(xUTClass, xUTName)( ) {\
			static sisu::ccolor const clr( sisu::eTTYCBlack, sisu::eTTYCGreen, sisu::eModBold );\
			static sisu::ccolor const rnd( sisu::eTTYCBlack, static_cast<sisu::eTTYColor>( ( rand( ) % 7 ) + 2 ), sisu::eModBold );\
			std::cout << clr << "[ " << rnd << "Test " << # xUTClass << # xUTName << clr << "\t\t\t identified. ]" << std::endl;\
			::sisu::Once< xUTClass >( &initContext );\
			::sisu::Once< UTCLASS(xUTClass, xUTName) >( &initTest );\
		}\
} UTINSTANCE(xUTClass, xUTName);\
template<> template<>\
void test< xUTClass >::Body< UTCLASS(xUTClass, xUTName) >()

#endif // TEST_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_

