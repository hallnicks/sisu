#ifndef SIGNAL_5BBBDB26_BC3A_4C18_93DD_F622BDCEE17E_HPP_
#define SIGNAL_5BBBDB26_BC3A_4C18_93DD_F622BDCEE17E_HPP_

#include <csignal>

namespace sisu
{

namespace detail
{
        class DefaultSignalHandler
        {
		DefaultSignalHandler( ); ~DefaultSignalHandler( );
		public:
			static void trace( char const * const xSignalName = "Unknown", bool xBlock = true );
	                static void onAbort( int xParam );
	                static void onFloatingPointException( int xParam );
	                static void onIllegalInstruction( int xParam );
	                static void onInterrupt( int xParam );
	                static void onSegmentationViolation( int xParam );
	                static void onTerminate( int xParam );
	};

} // namespace detail

template<class XSignalHandler = detail::DefaultSignalHandler>
class SignalHandler
{
	static void onAbort( int xParam )
	 { XSignalHandler::onAbort( xParam ); }

	static void onFloatingPointException( int xParam )
	 { XSignalHandler::onFloatingPointException( xParam ); }

	static void onIllegalInstruction( int xParam )
	 { XSignalHandler::onIllegalInstruction( xParam ); }

	static void onInterrupt( int xParam )
	 { XSignalHandler::onInterrupt( xParam ); }

	static void onSegmentationViolation( int xParam )
 	 { XSignalHandler::onSegmentationViolation( xParam ); }

	static void onTerminate( int xParam )
	 { XSignalHandler::onTerminate( xParam ); }

	public:
                void operator( )( )
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
