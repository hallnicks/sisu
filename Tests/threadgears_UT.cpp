#if 0
// This file is part of sisu.

// sisu is free software: you can redistribute it and/or modify // it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or // (at your option) any later version.

// sisu is distributed in the hope that it will be useful, // but WITHOUT ANY WARRANTY; without even the implied warranty of //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the // GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.
#include "test.hpp"

#include "threadgears.hpp"
#include "stacktrace.hpp"
#include "ttyc.hpp"
#include "ttycolor.hpp"

using namespace sisu;

namespace
{

class tg_UT : public context
{
	public:
		tg_UT( ) : context( ) { }
		void Up() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
		void Down() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

int vanillaFunction(const char * xMessage) {
	std::cout << xMessage << std::endl;
	return 0;
}

} // namespace


typedef gear<int32_t, const char *>	MultithreadedCout;
typedef gear<int32_t, uint8_t > 	screenfiller;

TEST(tg_UT, MutexTrivial)
{
	mutex m;
	m.lock( );
	m.unlock( );
}

TEST(tg_UT, InsaneLambda)
{
	screenfiller( [&]( uint8_t const xChar ) -> int32_t
						{ for (int i = 0; i < 5000; ++i) { std::cout << xChar; }
							} )('a')('b')('c')('d')('e')('f')('g')
							   ('h')('i')('j')('k')('l')('m')('n')
							   ('o')('p')('q')('r')('s')('t')('u')
							   ('v')('w')('x')('y')('z');
}

TEST(tg_UT, MutexLambdas) {

	mutex m;
	bool isDone = false;

	std::vector<std::string> strings;

	MultithreadedCout g( [&](const char * xPtr ) -> int {
								while ( 1 )
								{
									sleep::ms( 60 );

									bool done = false;

									m.run([&] {

										done = isDone;

										std::string message = std::string("background thread" ) + xPtr;

										std::stringstream oss;

										oss << message << rand( ) % 50 << std::endl;

										strings.push_back( oss.str( ) );

									 	std::cout << ccolor( eTTYCGreen, eTTYCBlack, eModNone ) << " backgroound thread still alive !" << std::endl;

										} );

									if ( done )
									{
										break;
									}
								}

								std::cout << "Background thread exiting." << xPtr << std::endl;

								return 0;
							    } );

	g("O")("D")("is")("great");

	int count = 0;

	while( 1 )
	{
		sleep::ms(30);

		m.run([&]( )
			{
				if ( strings.size( ) > 0 )
				{
					std::cout << ccolor( eTTYCYellow, eTTYCBlack, eModNone ) << strings.back( ) << std::endl;
					strings.pop_back( );
				}
				else
				{
					std::cout << ccolor( eTTYCRed, eTTYCBlack, eModNone ) << " empty!" << std::endl;
				}

			 	std::cout << ccolor( eTTYCRed, eTTYCBlack, eModBlink ) << " Foreground thread still alive !" << std::endl;



				isDone = ++count > 30;
			});

		if (isDone)
			break;
	}

	std::cout << "Mutex lambda test complete." << std::endl;
}

TEST(tg_UT, EventTrivial1)
{
	event e;

	gear<uint8_t, const char*> g([&] ( const char * xLabel)
	{
		std::cout << xLabel << std::endl;
		sleep::ms( 255 + rand( ) % 1000 );
		e.set( );
		return 0;
	});

	g("Main setter.");

	do
	{
		sleep::ms( 1000 );
		std::cout << "Waiting for set." << std::endl;

	} while (!e.isSet( ));

	std::cout << "Set!" << std::endl;
}


TEST(tg_UT, LambdaTrivial)
{
	// establish some basic lambda functionality 
	std::function< int (const char * )> func = []( const char * xPtr ) -> int { std::cout << xPtr << std::endl; return 0; };
	std::cout << "func()=" << func("hello, world") << std::endl;
}
#if 0

TEST(tg_UT, ThreadTrivial)
{
	// Make sure it works with vanilla functions
	{
		MultithreadedCout t1( &vanillaFunction );

		t1( "Hello" );
		t1( "There" );
		t1( "This"  );
		t1( "Hurts" );

		t1.join( );

	}

	std::cout << "Scope cleared " << std::endl;
}
#endif

TEST(tg_UT, ThreadCreatesThreads)
{
	{
		mutex m;

		static uint32_t results = 0;

		MultithreadedCout g( [&](const char * xPtr ) -> int
			{
				m.lock( );
				++results;
				m.unlock( );
				return results;
			} );

		g( "Hello, world."    )
		 ( "Hello, user."     )
		 ( "Hello, Children." )
		 ( "Hello, Sun."      )
		 ( "Hello, Moon."     );

		g.join( );

		MUSTEQ(g.size( ), 5);

	}
}

TEST(tg_UT, ThreadCreatesThreadsWithForLoop)
{
	{
		mutex m;

		static uint32_t results = 0;

		MultithreadedCout g( [&]( const char * ) -> int
			{
				m.lock( );
				++results;
				m.unlock( );
				return results;
			} );

		for ( int32_t ii = 0; ii < 50; ++ii )
		{
			g( "Hello, world."    );

		}
		g.join( );

		MUSTEQ(g.size( ), 50);
	}
}

TEST(tg_UT, ThreadLambdaCute1)
{
	MultithreadedCout g( [&](const char * xPtr ) -> int {  std::cout << "cute " << xPtr <<  std::endl; return 0; } );
	g( "Hello, world." );

	g.join( );
}

// Mysterious segfault!
TEST(tg_UT, ThreadLambdaCute2)
{
	{
		MultithreadedCout( [&](const char * xPtr ) -> int {  std::cout << "cute " << xPtr << std::endl; return 0; } )( "Hello, world." );
	}
	std::cout << "scope cleared." << std::endl;
}
#endif
