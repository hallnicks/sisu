#ifndef TDD_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_
#define TDD_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_

#include <iostream>

namespace TinyTDD
{
	class TDDBase
	{
		public:
			inline TDDBase( ) { }
			virtual ~TDDBase( ) { }
			virtual void Setup ( ) = 0;
			virtual void TearDown( ) = 0;
			template< typename XTDDClass >
			void TDDBody( );
	};

	typedef void(*InvokeableVoid)();

	template < typename XType >
	class LinkedList
	{
		struct Node
		{
			inline Node( ) : Value( 0 ), Next( 0 ) { }

			inline ~Node( )
			{
				if ( Next != 0 )
				{
					delete Next;
				}
			}

			XType Value;
			Node * Next;
		};

		Node mHead, * mCurrent;

		public:
			inline LinkedList( ) : mHead( ), mCurrent( &mHead ) { }

			inline ~LinkedList( ) { }

			inline void push_back( XType & xValue )
			{
				mCurrent->Value = xValue;
				mCurrent = mCurrent->Next = new Node;
			}

			template< typename XClass >
			inline void operator( )( XClass & xRef )
			{
				Node * n = &mHead;
				do
				{
					if ( n->Value != 0 )
					{
						( xRef.*n->Value )( );
					}

				} while( ( n = n->Next ) != 0 );
			}

			inline void operator( )( )
			{
				Node * n = &mHead;
				do
				{
					if ( n->Value )
					{
						( *n->Value )( );
					}
				}
				while( ( n = n->Next ) != 0 );
			}
	};

	namespace TDDStorage
	{
		template< typename XTDDClass, typename XStorage >
		inline XStorage & TDDList( void ( TDDBase:: * XEventFunction ) ( ) = 0 )
		{
			static XStorage instance;
			if ( XEventFunction != 0 )
			{
				instance.push_back( XEventFunction );
			}
			return instance;
		}

		template< typename XTDDType, typename XStorage >
		inline void RunTDD( )
		{
			try
			{
				static XTDDType type;
				type.Setup( );
				TDDList< XTDDType, XStorage >( )( type );
				type.TearDown( );
			}
			catch( ... )
			{
				std::cerr << "Test failed." << std::endl;
				// TODO - create test object!
			}
		}

		inline void InvocationList( InvokeableVoid xF = 0 )
		{
			static LinkedList< InvokeableVoid > s;
			xF != 0 ? s.push_back( xF ) : s( );
		}
	}

	inline void ExecuteTests( )
	{
		TDDStorage::InvocationList( );
	}

	namespace TestResults
	{
		class TestFailedException { };
	}

#define MUST_EQUAL( xLHS, xRHS ) if ( xlHS != xRHS) { throw TestFailedException( ); }
#define TDD_STORAGE( xTDDClass, xTDDName ) xTDDClass ## xTDDName ## _storage_decl
#define TDD_INSTANCE( xTDDClass, xTDDName ) s ## xTDDClass ## xTDDName ## _decl
#define TDD_CLASS( xTDDClass, xTDDName ) xTDDClass ## _ ## xTDDName
#define DECL_TDD( xTDDClass, xTDDName, xExpression )\
	namespace TinyTDD {\
		class TDD_CLASS(xTDDClass, xTDDName) { private: TDD_CLASS(xTDDClass, xTDDName)( ); };\
	 	template<> void TDDBase::TDDBody< TDD_CLASS(xTDDClass, xTDDName) >( )xExpression\
	}\
	namespace {\
		static class TDD_STORAGE(xTDDClass, xTDDName) {\
		public:\
			TDD_STORAGE(xTDDClass, xTDDName)( ) {\
				typedef void(TinyTDD::TDDBase::*InvokeTDD)();\
				typedef TinyTDD::LinkedList< InvokeTDD > list_t;\
				TinyTDD::TDDStorage::TDDList< xTDDClass, list_t >( &TinyTDD::TDDBase::TDDBody< TinyTDD::TDD_CLASS(xTDDClass, xTDDName) > );\
				TinyTDD::TDDStorage::InvocationList( &TinyTDD::TDDStorage::RunTDD< xTDDClass, list_t > );\
			}\
		} TDD_INSTANCE(xTDDClass, xTDDName);\
	}

} // namespace TinyTDD

#endif // TDD_F88C97663Cf44CE9B325FF50DC9B2C18_HPP_
