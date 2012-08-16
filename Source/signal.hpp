#ifndef SIGNAL_5BBBDB26_BC3A_4C18_93DD_F622BDCEE17E_HPP_
#define SIGNAL_5BBBDB26_BC3A_4C18_93DD_F622BDCEE17E_HPP_

#include "stacktrace.hpp"
#include <csignal>
#include <stdio.h>

namespace sisu
{

namespace detail
{
        class DefaultSignalHandler
        {
		DefaultSignalHandler( ); ~DefaultSignalHandler( );

		public:
			static inline void trace( char const * const xSignalName = "Unknown", bool xBlock = true )
			{
				std::cout << "===================================================================================================" << std::endl;
				std::cout << "===================================================================================================" << std::endl;
				std::cout << "===================================================================================================" << std::endl;
				std::cout << "===================================================================================================" << std::endl;

				if ( xBlock )
				{
					std::cout << "===================================================================================================" << std::endl;
					std::cout << "===================================================================================================" << std::endl;
					std::cout << "===================================================================================================" << std::endl;
					std::cout << "=================== You caught " << xSignalName << ". ┌∩┐(◣_◢)┌∩┐ That shit's terminal. ===========" << std::endl;
					std::cout << "===================================================================================================" << std::endl;
					std::cout << "===================================================================================================" << std::endl;


				}
				else
				{
					std::cout << "===================================================================================================" << std::endl;
					std::cout << "===================================================================================================" << std::endl;
					std::cout << "===== !!!!!ABANDON SHIP!!!!!  (╯°□°）╯︵ ┻━┻  ABORT!!! ABORT!!! ===================================" << std::endl;
					std::cout << "===================================================================================================" << std::endl;
					std::cout << "===================================================================================================" << std::endl;

				}

				printstack<10>( );

				if ( xBlock ) { static_cast<void>(getchar()); }

				std::cout << "===================================================================================================" << std::endl;
				std::cout << "===================================================================================================" << std::endl;
				std::cout << "===================================================================================================" << std::endl;
				std::cout << "==================================ONLY LOSERS DO DRUGS=============================================" << std::endl;
				std::cout << "=======================================GAME OVER===================================================" << std::endl;
				std::cout << "===================================================================================================" << std::endl;
				std::cout << "===================================================================================================" << std::endl;
			}

	                static inline void onAbort( int xParam ) { trace( __FUNCTION__ ); exit( -1 ); }

	                static inline void onFloatingPointException( int xParam ) { trace( __FUNCTION__); exit( -1 ); }

	                static inline void onIllegalInstruction( int xParam ) { trace( __FUNCTION__ ); exit( -1 ); }

	                static inline void onInterrupt( int xParam ) { trace( __FUNCTION__, false ); exit( 0 ); }

	                static inline void onSegmentationViolation( int xParam ) { trace( __FUNCTION__ ); exit( -1 ); }

	                static inline void onTerminate( int xParam ) { trace( __FUNCTION__ ); exit( -1 ); }
	};

} // namespace detail

template<class XSignalHandler = detail::DefaultSignalHandler>
class SignalHandler
{
	static inline void onAbort( int xParam )
	 { XSignalHandler::onAbort( xParam ); }

	static inline void onFloatingPointException( int xParam )
	 { XSignalHandler::onFloatingPointException( xParam ); }

	static inline void onIllegalInstruction( int xParam )
	 { XSignalHandler::onIllegalInstruction( xParam ); }

	static inline void onInterrupt( int xParam )
	 { XSignalHandler::onInterrupt( xParam ); }

	static inline void onSegmentationViolation( int xParam )
 	 { XSignalHandler::onSegmentationViolation( xParam ); }

	static inline void onTerminate( int xParam )
	 { XSignalHandler::onTerminate( xParam ); }

	public:
                inline void operator( )( )
                {
                        signal( SIGABRT, onAbort );
                        signal( SIGFPE, onFloatingPointException );
                        signal( SIGILL, onIllegalInstruction );
                        signal( SIGINT, onInterrupt );
                        signal( SIGSEGV, onSegmentationViolation );
                        signal( SIGTERM, onTerminate );
                }
};

typedef SignalHandler<detail::DefaultSignalHandler> StackTraceHandler;

} // namespace sisu

#endif // SIGNAL_5BBBDB26_BC3A_4C18_93DD_F622BDCEE17E_HPP_
