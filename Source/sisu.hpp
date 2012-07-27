#ifndef SISU_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_
#define SISU_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_

#include <csignal>
#include <iostream>

#include "typename.hpp"

namespace sisu
{
	class context
	{
		public:
			inline context( ) { }
			virtual ~context( ) { }
			virtual void Up ( ) = 0;
			virtual void Down( ) = 0;
			template< typename T >
			void Body( );
	};

	typedef void ( *InvokeTests ) ( );

	template< class XContext >
	class test : public XContext
	{
		public:
			test( ) : XContext( ) { }
			~test( ) { }
			template< typename T>
			void Body( );
	};

	template < typename T >
	class chain
	{
		struct N
		{
			inline N( ) : V( 0 ), X( 0 ) { }
			inline ~N( ) { if ( X != 0 ) { delete X; } }
			T V; N * X;

		} mH, * mC;

	public:

		inline chain( ) : mH( ), mC( &mH ) { }

		virtual ~chain( ) { }

		inline void push_back( T & xV ) { mC->V = xV; mC = mC->X = new N; }

		template< typename XClass >
		inline void operator( )( XClass & xRef )
		{
			N * n = &mH; do { if ( n->V != 0 ) { ( xRef.*n->V )( ); } } while( ( n = n->X ) != 0 );
		}

		inline void operator( )( )
		{
			N * n = &mH; do { if ( n->V ) { ( *n->V )( ); } } while( ( n = n->X ) != 0 );
		}
	};

	template< typename T, typename XStorage >
	inline XStorage & UnitTest( void ( test< T >::*XF ) ( ) = 0 )
	{
		static XStorage instance;
		if ( XF != 0 ) { instance.push_back( XF ); }
		return instance;
	}

	void HookSignals( )
	{
		struct StaticHandler
		{
			static inline void onAbort( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onFloatingPointException( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onIllegalInstruction( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onInterrupt( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onSegmentationViolation( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onTerminate( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			inline StaticHandler( )
			{
				signal( SIGABRT, onAbort );
				signal( SIGFPE, onFloatingPointException );
				signal( SIGILL, onIllegalInstruction );
				signal( SIGINT, onInterrupt );
				signal( SIGSEGV, onSegmentationViolation );
				signal( SIGTERM, onTerminate );
			}
		} m;
	}

	template< typename T, typename XStorage >
	inline void RunUnitTest( )
	{
		static struct TestResult
		{
			static inline void onAbort( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onFloatingPointException( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onIllegalInstruction( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onInterrupt( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onSegmentationViolation( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			static inline void onTerminate( int xParam )
			{
				std::cerr << __FUNCTION__ << std::endl;
			}

			enum eTestResult
			{
				eFailed,
				eExceptionInConstructor,
				eExceptionInSetup,
				eExceptionInTearDown,
				eSuccess,

			} mResult;

		public:

			TestResult( ) : mResult( eFailed )
			{
				signal( SIGABRT, onAbort );
				signal( SIGFPE, onFloatingPointException );
				signal( SIGILL, onIllegalInstruction );
				signal( SIGINT, onInterrupt );
				signal( SIGSEGV, onSegmentationViolation );
				signal( SIGTERM, onTerminate );
			}

			virtual ~TestResult( ) { ::sisu::HookSignals( ); }

			void Run( )
			{
				try
				{
					static test<T> type;

					try
					{
						type.Up( );
					}
					catch( ... )
					{
						mResult = eExceptionInSetup;
					}

					try
					{
						::sisu::UnitTest< T, XStorage >( )( type );
					}
					catch ( ... )
					{
						mResult = eExceptionInConstructor;
					}
					try
					{
						type.Down( );
					}
					catch( ... )
					{
						mResult = eExceptionInTearDown;
					}
				}
				catch ( ... )
				{
					mResult = eExceptionInConstructor;
				}
			}
		} r;

		r.Run( );
	}

	inline void InvocationList( InvokeTests xF = 0 )
	{
		static chain< InvokeTests > s;
		xF != 0 ? s.push_back( xF ) : s( );
	}

	inline void ExecuteTests( ) { InvocationList( ); }

	class TestFailedException { };

#define MUST_EQUAL( xLHS, xRHS ) if ( xlHS != xRHS) { throw TestFailedException( ); }
#define UT_STORAGE( xUTClass, xUTName ) xUTClass ## xUTName ## _storage_decl
#define UT_INSTANCE( xUTClass, xUTName ) s ## xUTClass ## xUTName ## _decl
#define UT_CLASS( xUTClass, xUTName ) xUTClass ## _ ## xUTName
#define DECL_UT( xUTClass, xUTName, xExpression )\
	namespace sisu {\
		class UT_CLASS(xUTClass, xUTName) { private: UT_CLASS(xUTClass, xUTName)( ); };\
		template<> template<>\
		void test< xUTClass >::Body< UT_CLASS(xUTClass, xUTName) >( )xExpression\
	}\
	namespace {\
		static class UT_STORAGE(xUTClass, xUTName) {\
		public:\
			UT_STORAGE(xUTClass, xUTName)( ) {\
				typedef void(::sisu::test< xUTClass >::*InvokeUnitTest)();\
				typedef ::sisu::chain< InvokeUnitTest > list_t;\
				::sisu::UnitTest< xUTClass, list_t >( &sisu::test< xUTClass >::Body< sisu::UT_CLASS(xUTClass, xUTName) > );\
				::sisu::InvocationList( &sisu::RunUnitTest< xUTClass, list_t > );\
			}\
		} UT_INSTANCE(xUTClass, xUTName);\
	}

} // namespace sisu

#endif // SISU_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_
