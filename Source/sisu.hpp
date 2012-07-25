#ifndef TDD_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_
#define TDD_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_

#include <iostream>

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

	typedef void(*InvokeTests)();

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

			inline ~chain( ) { }

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
	inline XStorage & UnitTest( void ( context::*XF ) ( ) = 0 )
	{
		static XStorage instance;
		if ( XF != 0 ) { instance.push_back( XF ); }
		return instance;
	}

	template< typename T, typename XStorage >
	inline void RunUnitTests( )
	{
		try
		{
			static T type;
			type.Up( );
			UnitTest< T, XStorage >( )( type );
			type.Down( );
		}
		catch( ... )
		{
			std::cerr << "Test failed." << std::endl;
			// TODO - create test object!
		}
	}

	inline void InvocationList( InvokeTests xF = 0 )
	{
		static chain< InvokeTests > s;
		xF != 0 ? s.push_back( xF ) : s( );
	}

	inline void ExecuteTests( )
	{
		InvocationList( );
	}

	class TestFailedException { };

#define MUST_EQUAL( xLHS, xRHS ) if ( xlHS != xRHS) { throw TestFailedException( ); }
#define UT_STORAGE( xUTClass, xUTName ) xUTClass ## xUTName ## _storage_decl
#define UT_INSTANCE( xUTClass, xUTName ) s ## xUTClass ## xUTName ## _decl
#define UT_CLASS( xUTClass, xUTName ) xUTClass ## _ ## xUTName
#define DECL_UT( xUTClass, xUTName, xExpression )\
	namespace sisu {\
		class UT_CLASS(xUTClass, xUTName) { private: UT_CLASS(xUTClass, xUTName)( ); };\
	 	template<> void context::Body< UT_CLASS(xUTClass, xUTName) >( )xExpression\
	}\
	namespace {\
		static class UT_STORAGE(xUTClass, xUTName) {\
		public:\
			UT_STORAGE(xUTClass, xUTName)( ) {\
				typedef void(sisu::context::*InvokeUnitTest)();\
				typedef sisu::chain< InvokeUnitTest > list_t;\
				sisu::UnitTest< xUTClass, list_t >( &sisu::context::Body< sisu::UT_CLASS(xUTClass, xUTName) > );\
				sisu::InvocationList( &sisu::RunUnitTests< xUTClass, list_t > );\
			}\
		} UT_INSTANCE(xUTClass, xUTName);\
	}

} // namespace sisu

#endif // TDD_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_
