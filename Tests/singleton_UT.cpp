#ifdef SINGLETON_UT
#include "test.hpp"

#include "singleton.hpp"
#include "threadgears.hpp"

#include <fstream>


using namespace sisu;

namespace
{
	class singleton_UT : public context
	{
		public:
			singleton_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};

	class FinalType : public ISingleton<FinalType>
	{
		friend class Singleton<FinalType>;

		std::stringstream mMessages;

		FinalType( )
			: ISingleton<FinalType>( )
			, mMessages( )
		{
			;
		}


		public:
			void printMessage( const char * xMessage )
			{
				std::cout << xMessage << std::endl;
			}

			void save( const char * xMessage )
			{
				mMessages << xMessage << std::endl;
			}

			void print( )
			{
				std::cout << mMessages.str( );
			}

			void write( std::ofstream & xOfs )
			{
				xOfs << mMessages.str( );
			}

			void clear( )
			{
				mMessages.str( std::string( ) );
			}

			bool done( )
			{
				static int i = 0;
				return ++i > 1000;
			}
	};


} // namespace
typedef gear<std::string, const char * > writer;
typedef gear<uint8_t, uint8_t> 		 sequencer;

TEST(singleton_UT, singletonTest)
{

// Uncomment/comment as necessary to ensure these antipatterns do not work
#if defined(COMPILE_TESTS_singleton_UT)
	ISingleton singleton; // should not compile
	FinalType finalTYpe; // also should not compile.
	Singleton<ISingleton> mySingleton; // should not work
	Singleton<FinalType> mySingleton; // should not work
#endif
 	Singleton<FinalType>::get( )->printMessage(" Hello, world.");

 	LockedSingleton<FinalType>::doLocked( [&]( FinalType * xType )
	{
		xType->printMessage("Locked world.");
	});

	// Create a whole bunch of threads to do locked work on singleton.


	writer w( [&] ( const char * xChar )
	{
		bool done = false;
		do
		{
			std::stringstream message;

			message << xChar << " " << rand( ) % 255;

			LockedSingleton<FinalType>::doLocked( [&]( FinalType * xType )
			{
				xType->save( message.str( ).c_str( ) );

				done = xType->done( );
			} );


		} while ( !done );

		return std::string(xChar) + " done";

	});

	 w("thread 1")
	  ("thread 2")
	  ("thread 3");

	w.join( );

	MUSTEQ(3, w.size( ));

 	LockedSingleton<FinalType>::doLocked( [&]( FinalType * xType )
	{
		xType->print( );
	});

	std::ofstream output1( "singleton_UT_output01.txt" );
	std::ofstream output2( "singleton_UT_output02.txt" );
	std::ofstream output3( "singleton_UT_output03.txt" );

 	LockedSingleton<FinalType>::doLocked( [&]( FinalType * xType )
	{
		xType->write( output1 );
	});


	output1.close( );

 	LockedSingleton<FinalType>::doLocked( [&]( FinalType * xType )
	{
		xType->clear( );
	});
}
#endif // SINGLETON_UT
