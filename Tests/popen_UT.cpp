#if 0
#include "test.hpp"

#include "memblock.hpp"
#include "ttyc.hpp"
#include "ioassist.hpp"

#include <windows.h>


using namespace sisu;

namespace
{
	class popen_UT : public context
	{

		static constexpr const char * sInputMp4 = "trip.mp4";

		public:
			popen_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};

template < uint32_t XBufSize = 4096 >
class ChildProcessController
{
	HANDLE mChildStdInRead
	     , mChildStdInWrite
	     , mChildStdOutRead
             , mChildStdOutWrite;


	void _readPipe( )
	{
		DWORD dwRead, dwWritten;

		CHAR buffer[ XBufSize ];

		HANDLE parentStdOut = GetStdHandle( STD_OUTPUT_HANDLE );

		while ( 1 )
		{
			if ( !ReadFile( mChildStdOutRead, buffer, XBufSize, &dwRead, NULL ) || dwRead == 0 )
			{
				break;
			}

			if ( !WriteFile( parentStdOut, buffer, dwRead, &dwWritten, NULL ) )
			{
				break;
			}
		}
	}

	void _writePipe( )
	{
		DWORD dwRead, dwWritten;

		char getsBuffer[ XBufSize ];

		TCHAR * commandLine;

		while ( 1 )
		{
			std::cout << "Input:" << std::endl;
			if ( ( commandLine = fgets( getsBuffer, XBufSize, stdin ) ) == NULL )
			{
				break;
			}

			// hack
			if ( strcmp( getsBuffer, "quit" ) == 0 )
			{
				break;
			}

			if ( !WriteFile( mChildStdInWrite, getsBuffer, dwRead, &dwWritten, NULL ) )
			{
				break;
			}
		}

		if ( !CloseHandle( mChildStdInWrite ) )
		{
			std::cerr << "CloseHandle( .. ) failed." << std::endl;
			exit( -1 );
		}

	}

	void _createChildProcess( )
	{
		TCHAR * commandLine;

		char getsBuffer[ XBufSize ];

		PROCESS_INFORMATION procInfo;

		STARTUPINFO startUpInfo;

		std::cout << "Input:" << std::endl;
		if ( ( commandLine = fgets( getsBuffer, XBufSize, stdin ) ) == NULL )
		{
			std::cerr << "fgets( .. ) failed." <<std::endl;
			exit( -1 );
		}

		ZeroMemory( &procInfo, sizeof( PROCESS_INFORMATION ) );

		ZeroMemory( &startUpInfo, sizeof( STARTUPINFO ) );

		startUpInfo.cb 	       = sizeof( STARTUPINFO );
		startUpInfo.hStdError  = mChildStdOutWrite;
		startUpInfo.hStdOutput = mChildStdOutWrite;
		startUpInfo.hStdInput  = mChildStdInRead;
		startUpInfo.dwFlags   |= STARTF_USESTDHANDLES;

		if ( !CreateProcess( NULL
				   , commandLine
				   , NULL
				   , NULL
				   , TRUE
				   , 0
				   , NULL
				   , NULL
				   , &startUpInfo
				   , &procInfo ) )
		{
			std::cerr << "CreateProcess( ... ) failed." << std::endl;
			exit( -1 );
		}

		if ( !CloseHandle( procInfo.hProcess ) )
		{
			std::cerr << "CloseHandle( .. ) failed." << std::endl;
			exit( -1 );
		}

		if ( !CloseHandle( procInfo.hThread ) )
		{
			std::cerr << "CloseHandle( .. ) failed." << std::endl;
			exit( -1 );
		}
	}

	public:
		ChildProcessController( )
			: mChildStdInRead  ( INVALID_HANDLE_VALUE )
			, mChildStdInWrite ( INVALID_HANDLE_VALUE )
			, mChildStdOutRead ( INVALID_HANDLE_VALUE )
			, mChildStdOutWrite( INVALID_HANDLE_VALUE )
		{
			;
		}

		void initialize( )
		{
			SECURITY_ATTRIBUTES attributes;

			attributes.nLength 	  	= sizeof( SECURITY_ATTRIBUTES );
			attributes.bInheritHandle 	= true;
			attributes.lpSecurityDescriptor = NULL;

			if ( !CreatePipe( &mChildStdOutRead
				        , &mChildStdOutWrite
 				        , &attributes
                                        , 0 ) )
			{
				std::cerr << "CreatePipe( .. ) failed" << std::endl;
				exit( -1 );
			}

			if ( !SetHandleInformation( mChildStdOutRead
						  , HANDLE_FLAG_INHERIT
						  , 0 ) )
			{
				std::cerr << "SetHandleInformation( .. ) failed." << std::endl;
			}

			if ( !CreatePipe( &mChildStdInRead
				        , &mChildStdInWrite
 				        , &attributes
                                        , 0 ) )
			{
				std::cerr << "CreatePipe( .. ) failed" << std::endl;
				exit( -1 );
			}


			if ( !SetHandleInformation( mChildStdInWrite
						  , HANDLE_FLAG_INHERIT
						  , 0 ) )
			{
				std::cerr << "SetHandleInformation( .. ) failed." << std::endl;
			}
		}


		// TODO: Try running this in its own thread.
		void runChildProcess( )
		{
			_createChildProcess( );

			_writePipe( );

			_readPipe( );
		}
};

} // namespace



TEST(popen_UT, popenSimpleDirectoryListing)
{

	ChildProcessController<4096> ctl;

	ctl.initialize( );

	ctl.runChildProcess( );

	BLOCK_EXECUTION;
}
#endif
