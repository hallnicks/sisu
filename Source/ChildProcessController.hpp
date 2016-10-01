#ifndef CHILD_PROCES_CONTROLLER_FFEB3CC5D5A74F06BEBA44D7D6DDF445_HPP_
#define CHILD_PROCES_CONTROLLER_FFEB3CC5D5A74F06BEBA44D7D6DDF445_HPP_
#include "test.hpp"

#include "memblock.hpp"
#include "ttyc.hpp"
#include "ioassist.hpp"
#include "threadgears.hpp"
#include "Stopwatch.hpp"

#include <windows.h>
#include <sstream>

namespace sisu {

template < uint32_t XBufSize = 4096 >
class ChildProcessController
{
	HANDLE mChildStdInRead
	     , mChildStdInWrite
	     , mChildStdOutRead
             , mChildStdOutWrite;

	PROCESS_INFORMATION mProcInfo;

	char mCurrentDirectory[ MAX_PATH ];

	void _readPipe( std::function<void(uint8_t*, uint64_t)> xOnReceivePipeData )
	{
		DWORD dwRead, dwWritten;

		CHAR buffer[ XBufSize ];

		Stopwatch sw;

		sw.startMs( );

		while ( 1 )
		{
			DWORD bytesAvailable = 0;

			if ( !PeekNamedPipe( mChildStdOutRead, NULL, 0, NULL, &bytesAvailable, NULL ) )
			{
				std::cerr << "PeekNamedPipe( .. ) failed." << std::endl;
				exit( -1 );
			}

			if ( bytesAvailable <= 0 )
			{
				static const double _1Quartsec = 250.0;

				double const elapsed = sw.stop( );

				if ( elapsed > _1Quartsec )
				{
					DWORD exitCode = 0;

					if ( !GetExitCodeProcess( mProcInfo.hProcess, &exitCode ) )
					{
						std::cerr << "GetExitCodeProcess( .. ) failed." << std::endl;
						exit( -1 );
					}

					if ( exitCode != STILL_ACTIVE )
					{
						break;
					}
				}

				continue;
			}

			while ( bytesAvailable > 0 )
			{
				if ( !ReadFile( mChildStdOutRead, buffer, XBufSize, &dwRead, NULL ) || dwRead == 0 )
				{
					std::cout << "ReadFile( .. ) failed or read " << dwRead <<  " bytes" << std::endl;
					break;
				}

				bytesAvailable = ( bytesAvailable - dwRead ) > 0 ? 0 : bytesAvailable - dwRead;

				xOnReceivePipeData( (uint8_t*)buffer, (uint64_t)dwRead );

				fflush( stdout );
			}
		}
	}

	void _writePipe( )
	{
		DWORD dwRead, dwWritten;

		char getsBuffer[ XBufSize ];

		TCHAR * commandLine = NULL;

		while ( 1 )
		{
			if ( ( commandLine = fgets( getsBuffer, XBufSize, stdin ) ) == NULL )
			{
				break;
			}

			if ( ( dwRead = strlen( getsBuffer ) ) <= 1 )
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

	bool _createChildProcess( char * xCmd = NULL )
	{
		TCHAR * commandLine;

		char getsBuffer[ XBufSize ];

		STARTUPINFO startUpInfo;

		if ( ( xCmd == NULL ) &&
		     ( commandLine = fgets( getsBuffer, XBufSize, stdin ) ) == NULL )
		{
			std::cerr << "fgets( .. ) failed." <<std::endl;
			return false;
		}
		else
		{
			commandLine = xCmd;
		}

		ZeroMemory( &mProcInfo, sizeof( PROCESS_INFORMATION ) );

		ZeroMemory( &startUpInfo, sizeof( STARTUPINFO ) );

		startUpInfo.cb 	       = sizeof( STARTUPINFO );
		startUpInfo.hStdError  = mChildStdInRead;
		startUpInfo.hStdOutput = mChildStdOutWrite;
		startUpInfo.hStdInput  = mChildStdOutWrite;
		startUpInfo.dwFlags   |= STARTF_USESTDHANDLES;

		std::stringstream ss;

		ss << "cmd /c " << commandLine;

		strncpy( getsBuffer, ss.str( ).c_str( ), ss.str( ).length( ) );

		getsBuffer[ ss.str( ).length( ) ] = '\0';

		if ( !CreateProcessA( NULL
				    , getsBuffer
				    , NULL
				    , NULL
				    , TRUE
				    , CREATE_NO_WINDOW
				    , NULL
				    , NULL
				    , &startUpInfo
				    , &mProcInfo ) )
		{
			std::cerr << "CreateProcess( ... ) failed: " << GetLastError( ) << std::endl;
			return false;
		}

		return true;
	}

	void _closeHandles( )
	{
		auto closeHandle = [ & ]( HANDLE xX ) { if ( xX != INVALID_HANDLE_VALUE ) { CloseHandle( xX ); } };

		closeHandle( mChildStdInRead    );
		closeHandle( mChildStdInWrite   );
		closeHandle( mChildStdOutRead   );
		closeHandle( mChildStdOutWrite  );
		closeHandle( mProcInfo.hProcess );
		closeHandle( mProcInfo.hThread  );
	}

	public:
		ChildProcessController( )
			: mChildStdInRead  ( INVALID_HANDLE_VALUE )
			, mChildStdInWrite ( INVALID_HANDLE_VALUE )
			, mChildStdOutRead ( INVALID_HANDLE_VALUE )
			, mChildStdOutWrite( INVALID_HANDLE_VALUE )
			, mProcInfo( )
		{
			;
		}

		~ChildProcessController( )
		{
			_closeHandles( );
		}

		void initialize( )
		{
			DWORD result = 0;

			if ( GetCurrentDirectory( MAX_PATH, mCurrentDirectory ) == 0 )
			{
				std::cerr << "GetCurrentDirectory( .. ) failed." << std::endl;
				exit( -1 );
			}

			_closeHandles( );

			SECURITY_ATTRIBUTES attributes;

			attributes.nLength 	  	= sizeof( SECURITY_ATTRIBUTES );
			attributes.bInheritHandle 	= TRUE;
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

		bool runCommand( char* xCmd, std::function<void(uint8_t*, uint64_t)> xOnReceivePipeData )
		{
			if ( _createChildProcess( xCmd ) )
			{
				DWORD exitCode = STILL_ACTIVE;

				do
				{
					 _readPipe( xOnReceivePipeData );

					if ( !GetExitCodeProcess( mProcInfo.hProcess, &exitCode ) )
					{
						std::cerr << "GetExitCodeProcess( .. ) failed." << std::endl;
						exit( -1 );
					}

					WaitForSingleObject( mProcInfo.hProcess, 100 );

				} while ( exitCode == STILL_ACTIVE );

				if ( exitCode != 0 )
				{
					return false;
				}

				return true;
			}

			return false;
		}
};

} // namespace sisu

#endif // CHILD_PROCES_CONTROLLER_FFEB3CC5D5A74F06BEBA44D7D6DDF445_HPP_
