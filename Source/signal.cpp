
#include "signal.hpp"

#include "stacktrace.hpp"
#include <stdio.h>

#include "AndroidLogWrapper.hpp"

namespace sisu
{

namespace detail
{

void DefaultSignalHandler::trace( char const * const xSignalName, bool xBlock )
{
	SISULOG("===================================================================================================");
	SISULOG("===================================================================================================");
	SISULOG("===================================================================================================");
	SISULOG("===================================================================================================");

	if ( xBlock )
	{
		SISULOG("===================================================================================================");
		SISULOG("===================================================================================================");
		SISULOG("===================================================================================================");
#ifdef ANDROID
		__android_log_print( ANDROID_LOG_VERBOSE \
                                         , "sisu" \
                                         , "Caught signal : %s \n" \
                                         , xSignalName );
#else
		std::cout << "=================== You caught " << xSignalName << ". ┌∩┐(◣_◢)┌∩┐ That shit's terminal. ===========" << std::endl;
#endif
		SISULOG("===================================================================================================");
		SISULOG("===================================================================================================");
		SISULOG("===================================================================================================");
	}
	else
	{
		SISULOG("===================================================================================================");
		SISULOG("===================================================================================================");
		SISULOG("======================================= ABORTING ==================================================");
		SISULOG("===================================================================================================");
		SISULOG("===================================================================================================");
	}

#ifdef ANDROID
	SISULOG("TODO: Implement stack trace on android to see more thorough debug information.");
#else
	printstack<10>( );
#endif

	if ( xBlock ) { static_cast<void>(getchar()); }

	SISULOG("===================================================================================================");
	SISULOG("===================================================================================================");
	SISULOG("================================== APPLICATION FAILED =============================================");
	SISULOG("===================================================================================================");
	SISULOG("===================================================================================================");
	SISULOG("===================================================================================================");
}

void DefaultSignalHandler::onAbort( int xParam ) { trace( __FUNCTION__ ); exit( -1 ); }

void DefaultSignalHandler::onFloatingPointException( int xParam ) { trace( __FUNCTION__); exit( -1 ); }

void DefaultSignalHandler::onIllegalInstruction( int xParam ) { trace( __FUNCTION__ ); exit( -1 ); }

void DefaultSignalHandler::onInterrupt( int xParam ) { trace( __FUNCTION__, false ); exit( 0 ); }

void DefaultSignalHandler::onSegmentationViolation( int xParam ) { trace( __FUNCTION__ ); exit( -1 ); }

void DefaultSignalHandler::onTerminate( int xParam ) { trace( __FUNCTION__ ); exit( -1 ); }


} // namespace detail

} // namespace sisu
