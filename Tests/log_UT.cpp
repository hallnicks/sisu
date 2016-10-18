#ifdef LOG_UT
#include "test.hpp"

#include <fstream>
#include <iostream>
#include <cstring>
#include <clocale>

#include <SDL2/SDL.h>

namespace
{

class log_UT : public context
{
	public:
		log_UT( ) : context( ) { }
		void Up( ) { }
		void Down( ) { }
};

#ifndef LOG_NAME
 #define LOG_NAME "SDLLog.txt"
#endif

static class SDLLogHandler
{
	std::ofstream mOfs;

	static void _SDL_LogOutputFunction( void * xPOfs
					 , int xCategory
					 , SDL_LogPriority xPriority
					 , const char * xMessage )
	{

		std::ofstream & ofs = ( *static_cast<std::ofstream*>( xPOfs ) );

		ofs << "[ SDL_Log ]"  << std::endl
		    << "[ Category: " << xCategory << " ]" << std::endl
		    << "[ Priority: " << xPriority << " ]" << std::endl
		    << "[ Memo: "     << xMessage  << " ]" << std::endl;
	}

	public:
		SDLLogHandler( )
		{
			mOfs.open( LOG_NAME );

			SDL_LogSetOutputFunction( _SDL_LogOutputFunction, &mOfs );
		}

		~SDLLogHandler( )
		{
			mOfs.close( );
		}

} sSDLLogHandler;


} // namespace

TEST(log_UT, UseSDLLoggingSystem)
{
	SDL_LogCritical( SDL_LOG_CATEGORY_APPLICATION, "%s", "Test" );
	BLOCK_EXECUTION;
}
#endif // LOG_UT
