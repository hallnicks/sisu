
#ifndef BACKTRACE_HPP_
#define BACKTRACE_HPP_

#include <cxxabi.h>
#ifdef __linux__
#ifndef ANDROID
#include <execinfo.h>
#endif
#else
#include <Windows.h>
#include <DbgHelp.h>
#endif

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

#include "ttycolor.hpp"

namespace sisu
{

template< unsigned int XDepth = 10 >
class stacktrace
{
	void * mArray[ XDepth ];

	int mSymbolCount;

	char ** mStrings;

	std::ostream & mOut;

	stacktrace( stacktrace const & );

	stacktrace & operator=( stacktrace const & );

	public:
		stacktrace( std::ostream & mOut = std::cout )
			: mArray( )
#ifdef LINUX
// TODO: Implement android backtrace!
#ifdef ANDROID
			, mSymbolCount( 0 )
			, mStrings( NULL )
#else
			, mSymbolCount( backtrace( mArray, XDepth ) )
			, mStrings( backtrace_symbols( mArray, mSymbolCount ) )
#endif
#else
//#warning Backtrace not implemented on windows yet
			, mSymbolCount( 0 )
			, mStrings( NULL )
#endif
			, mOut( std::cout )
		{
#ifdef WIN32
			HANDLE         process;

  			process = GetCurrentProcess();

			SymInitialize( process, NULL, TRUE );
			unsigned int   i;
			void         * stack[ 100 ];
		     	unsigned short frames;
     			SYMBOL_INFO  * symbol;

    			frames               = CaptureStackBackTrace( 0, 100, stack, NULL );
 			symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
     			symbol->MaxNameLen   = 255;
     			symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

			mSymbolCount = frames;

			mStrings = (char**)malloc( frames * sizeof(char*));

 		    	for( i = 0; i < frames; i++ )
     			{
 			        SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );
				std::ostringstream oss;
				oss << symbol->Name << " - 0x" << symbol->Address << std::endl;
				std::cout << oss.str( );
				memcpy( mStrings, oss.str().c_str( ), oss.str( ).size( ) );
			}

		     	free( symbol );
#endif
		}

		~stacktrace( ) { if ( mStrings ) { free( mStrings ); } }

	        typedef std::basic_ostream< char, std::char_traits< char > > cout_t;

                typedef cout_t & ( * endl_t )( cout_t & );

		template< typename T >
		stacktrace & operator << ( T & xStream ) { mOut << xStream; return (*this); }

		stacktrace & operator >> ( endl_t xStream ) { return (*this) << xStream; }

		std::ostream & operator >> ( std::ostream & xStream ) const
		{
			xStream << "[ Stacktrace ( " <<  mSymbolCount << " frames ) ]" << std::endl;

			if ( mStrings )
			{
				if ( mSymbolCount != 0 )
				{
					for ( int ii = 0; ii < mSymbolCount; ++ii )
					{
						xStream << "[ Frame " << ii << ":";

						if ( mStrings[ ii ] != NULL )
						{
							xStream << mStrings[ ii ] << ":";

							struct demangled
							{
								demangled( char * const xSymbol, std::ostream & xOut ) : N( 0 ), A( 0 ), O( 0 )
								{
									std::string const sym( xSymbol );
									std::string s( sym.substr( sym.find_first_of('('), sym.find_first_of(')') ) );
									std::cout << "Sym: " << sym << ". Sub:" << s << std::endl;

									int status = -1;

									N = abi::__cxa_demangle( NULL, 0, 0, &status );

									if ( status != 0 )
									{
										xOut << status;

										N = 0;
									}
									else
									{
										xOut << "0";
									}
								}

								~demangled( )
								{
									if ( N )
									{
										free( N );
									}
								}

								char * N, * A, * O;

							} sym( mStrings[ ii ], xStream );

							xStream << ( sym.N != NULL ? sym.N : "(xMANGLED)" );
						}
						else
						{
							xStream << "(0xUNKNOWN)";
						}

						xStream << " ]" << std::endl;
					}

					xStream  << "[ Trace complete ]" << std::endl;
				}
				else
				{
					xStream << "[ Error : Found 0 symbols in frame count. ]" << std::endl;
				}
			}
			else
			{
				xStream << "[ Error : No frames found. ]" << std::endl;
			}

			return xStream;
		}
};

template<unsigned int XDepth>
std::ostream & operator << ( std::ostream & xStream, stacktrace<XDepth> const & xStackTrace )
{
        return xStackTrace >> xStream;
}

// TODO: Implement stack tracking on android
#ifndef ANDROID
template< unsigned int XDepth >
inline void printstack( )
{
	std::cerr << ccolor( eTTYCRed, eTTYCBlack, eModNone ) << stacktrace< XDepth >( ) << std::endl;
}
#endif

} // namespace sisu

#endif // BACKTRACE_HPP_

