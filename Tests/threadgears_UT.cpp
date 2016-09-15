
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


#define BLOCKMAIN while(1) { sleep::ms( 100 ) ; std::cout << "Main thread active. "<< std::endl;}
#define CREATESINK(xReturnType, xParamType, xVarName)\
	std::function< xReturnType ( xParamType ) > xVarName = []( xParamType xPtr ) -> xReturnType {\
							std::cout << "In Lambda!" << std::endl;\
							sleep::ms( 1000 + rand() % 10000 );\
							std::cout << "Proceeding" << std::endl;\
							MUSTNEQ(xPtr, NULL);\
							static int count = 0;\
							std::cout << static_cast< xParamType >( xPtr ) << ", " << ++count << std::endl;\
							std::cout << "thread exiting." << std::endl;\
							return 700;\
						};

	CREATESINK(int, const char *, sink);

	typedef gear<int, const char *> MultithreadedCout;

TEST(tg_UT, MutexTrivial)
{
	mutex m;
	m.lock( );
	m.unlock( );
}

TEST(tg_UT, MutexLambdas)
{
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

		m.run([&] 
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
	

				isDone = ++count > 30;
			});

		if (isDone)
			break;
	}

	std::cout << "Mutex lambda test complete." << std::endl;
}

TEST(tg_UT, LambdaTrivial)
{
	// establish some basic lambda functionality 
	std::function< int (const char * )> func = []( const char * xPtr ) -> int { std::cout << xPtr << std::endl; return 0; };
	std::cout << "func()=" << func("hello, world") << std::endl;
}

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
	
		while( t1.size( ) > 0 ) {
			std::cout << "Result= " << *t1 << std::endl; }
	}
	
	std::cout << "Scope cleared " << std::endl;
}

TEST(tg_UT, ThreadLambdaCute1)
{
	MultithreadedCout g( [&](const char * xPtr ) -> int {  std::cout << "cute " << xPtr <<  std::endl; return 0; } );
	g( "Hello, world." );
	
	g.join( );
	while( g.size( ) > 0 ) {
		std::cout << "Result= " << *g << std::endl; }
}

// Mysterious segfault!
TEST(tg_UT, ThreadLambdaCute2)
{
	{
		MultithreadedCout( [&](const char * xPtr ) -> int {  std::cout << "cute " << xPtr <<  std::endl; return 0; } )( "Hello, world." );
	}
	std::cout << "scope cleared." << std::endl;
}
