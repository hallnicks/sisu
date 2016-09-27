#if 0
#include <fstream>
#ifndef __linux__
#include <winsock2.h>
#else
#error Socket port is sorely needed.
#endif

// for select
#include <fcntl.h>

#include "test.hpp"


// C style IO is necessary to talk to libpng. This could be seen as a wrapper between stl-style c++ and libpng

#include "memblock.hpp"
#include "ttyc.hpp"
#include "ttycolor.hpp"
#include "ioassist.hpp"

#include "PNGImage.hpp"
#include "threadgears.hpp"


// for bio/ssl connections
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <openssl/crypto.h>

// for make cert
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif

//#include <sys/socket.h>

using namespace sisu;

namespace
{
	class libopenssl_UT : public context
	{
		public:
			libopenssl_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};

	static const class _OpenSSLInitializer
	{
		public:
			_OpenSSLInitializer( )
			{
				SSL_library_init( );
				SSL_load_error_strings( );
				ERR_load_BIO_strings( );
				OpenSSL_add_all_algorithms( );
			}

			~_OpenSSLInitializer( )
			{
#ifndef OPENSSL_NO_ENGINE
				ENGINE_cleanup( );
#endif
				CRYPTO_cleanup_all_ex_data( );
			}

	} sOpenSSLInitializer;

	template< int32_t XBits = 2048, unsigned long XParams = RSA_F4 >
	class OpenSSLCertificate
	{
		RSA * mRSA;
		BIGNUM * mBigNum;

		bool mInitOK;

		public:
			OpenSSLCertificate( )
				: mRSA( NULL )
				, mBigNum( NULL )
				, mInitOK( false )
			{
				if ( ( mBigNum = BN_new( ) ) == NULL )
				{
					std::cerr << "BL_new( ) failed." << std::endl;
					exit( -1 );
				}

				if ( ( mRSA = RSA_new( ) ) == NULL )
				{
					std::cerr << "RSA_new( ) failed." << std::endl;
					exit( -1 );
				}
			}

			~OpenSSLCertificate( )
			{
				if ( mRSA != NULL )
				{
					RSA_free( mRSA );
				}

				if ( mBigNum != NULL )
				{
					BN_free( mBigNum );
				}
			}

			void initialize( )
			{
				if ( mInitOK )
				{
					std::cerr << "OpenSSLCertificate is already initialized." << std::endl;
					exit( -1 );
				}

				int32_t result;

				if ( ( result = BN_set_word( mBigNum, XParams ) ) != 1 )
				{
					std::cerr << "BN_set_word( .. ) failed." << std::endl;
					exit( -1 );
				}

				if ( ( result = RSA_generate_key_ex( mRSA, XBits, mBigNum, NULL ) ) != 1 )
				{
					std::cerr << "RSA_generate_key_ex( .. ) failed." << std::endl;
					exit( -1 );
				}

				mInitOK = true;
			}

			// TODO: Add ofstream support.
			void write( const char * xFile)
			{
				if ( !mInitOK )
				{
					std::cerr << "OpenSSLCertificate was not initialized." << std::endl;
					exit( -1 );
				}


				std::string const prefix( xFile );

				auto writeKeyFile = [ & ] ( std::function<void(BIO * xFile)> xInBetween, const char * xSuffix )
				{
					BIO * file = NULL;

					std::string const completeString = prefix + xSuffix;

					if ( ( file = BIO_new_file( completeString.c_str( ), "w+" ) ) == NULL )
					{
						std::cerr << "BIO_new_file( .. ) failed." << std::endl;
						exit( -1 );
					}

					xInBetween( file );

					BIO_free_all( file );
				};

				writeKeyFile( [ & ] ( BIO * xFile )
				{
					if ( PEM_write_bio_RSAPublicKey( xFile, mRSA ) != 1)
					{
						std::cerr << "PEM_write_bio_RSAPublicKey( .. ) failed." << std::endl;
						exit( -1 );
					}

				}, "_public.pem" );

				writeKeyFile( [ & ] ( BIO * xFile )
				{
					if ( PEM_write_bio_RSAPrivateKey( xFile, mRSA, NULL, NULL, 0, NULL, NULL ) != 1 )
					{
						std::cerr << "PEM_write_bio_RSAPrivateKey failed." << std::endl;
						exit( -1 );
					}

				}, "_private.pem" );
			}
	};

	class Connection
	{
		public:
			virtual void connect( const char * xIPAddress, uint32_t const xPort ) = 0;
			virtual void disconnect( ) = 0;

			virtual uint32_t read( uint8_t * xDestination, uint32_t const xWantBytes ) = 0;
			virtual void write( uint8_t * xSourceBuffer, uint32_t const xBytesToWrite ) = 0;
	};

	class OpenSSLConnection : public Connection
	{
		SSL_CTX * mSSLCtx;

		event mIsConnected, mIsDisconnected;

		public:
			OpenSSLConnection( bool const xIsServer = false)
				: Connection( )
				, mSSLCtx( NULL )
			{

			}

			virtual void connect( const char * xIPAddress, uint32_t const xPort )
			{

			}

			virtual void disconnect( )
			{


			}

			virtual uint32_t read( uint8_t * xDestination, uint32_t const xWantBytes )
			{

			}

			virtual void write( uint8_t * xSourceBuffer, uint32_t const xBytesToWrite )
			{

			}

	};

	class SecuredOpenSSLConnection
	{
		public:
	};


typedef enum {
	eSSLAction_Continue,
	eSSLAction_Break,
	eSSLAction_Neither,
} eSSLAction;


static struct _WinsockInitializer
{
	WSADATA wsaData;

	_WinsockInitializer( )
	{
		if ( WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) != 0 )
		{
			std::cerr << "WSA_Startup ( .. ) failed." << std::endl;
			exit( -1 );
		}
	}

	~_WinsockInitializer( )
	{
		WSACleanup( );
	}

} sWinsockInitializer;


template< uint32_t XBufferSize >
struct SOCKET_INFORMATION
{
   CHAR       Buffer[ XBufferSize ];
   WSABUF     DataBuf;
   SOCKET     Socket;
   OVERLAPPED Overlapped;
   DWORD      BytesSEND;
   DWORD      BytesRECV;
   sockaddr   sockaddr_info;
   uint64_t   socketID;

};

template< uint32_t XBufferSize >
struct SelectData
{
	SelectData( uint8_t * const xData
		   , uint64_t const xSize
		   , SOCKET_INFORMATION<XBufferSize> * xSocket )
		: data( xData )
		, size( xSize )
		, socket( xSocket )
	{
		;
	}

	uint8_t * 			  data;
	uint64_t  			  size;
	SOCKET_INFORMATION<XBufferSize> * socket;
};

typedef gear<uint32_t, const char*> NamedThread;

enum eSocketConnectionArchitecture
{
	eSocketConnectionArchitecture_Synchronous	= 0,
	eSocketConnectionArchitecture_NonblockingSelect = 1,
	eSocketConnectionArchitecture_Asynchronous	= 2,
	eSocketConnectionArchitecture_Max		= 3
};

enum eDatagramProtocol
{
	eDatagramProtocol_UDP = 0,
	eDatagramProtocol_TCP = 1,
	eDatagramProtocol_MAx = 2,
};

template< uint32_t XBufferSize >
class SocketConnector
{
	typedef std::function<void(SelectData<XBufferSize> const &)> OnReceiveSelectData;

	SOCKET_INFORMATION<XBufferSize> * mSocketArray[ FD_SETSIZE ];

	enum eSocketConnectorRole
	{	eSocketConnectorRole_Server = 0,
		eSocketConnectorRole_Client = 1,
	} mRole;

	eSocketConnectionArchitecture mArch;

	SOCKET mListenSocket, mAcceptSocket;
	struct sockaddr_in mListenSocketServer, mOtherSocketIP;
	int32_t mSendLength, mReceiveLength;

	char mBuffer[ XBufferSize ];
	uint32_t const mPort;
	bool mInitialized;

	std::string const mIPAddress;
	int32_t mNewSocketDescriptor;

	// TODO: Support multiple clients in all three modes.
	struct sockaddr_storage mNonBlockingLastConnected;
	int32_t			mNonBlockingLastConnectedSize;

	FD_SET mReadSet, mWriteSet;

	uint64_t mTotalConnectedSockets;

	event mQuit;

	typedef std::vector<OnReceiveSelectData> OnReceiveSelectDataCallbacks;

	OnReceiveSelectDataCallbacks mCallbacks;
	mutex mCallbackMutex;
	std::function<uint32_t(SocketConnector *)> mSelectFunction;

	gear<uint32_t, SocketConnector *> mSelectThread;

	eDatagramProtocol mProtocol;

	void _registerSocket( SOCKET xSocket , sockaddr const & xAddressInfo )
	{
		SOCKET_INFORMATION<XBufferSize> * si = NULL;

		if ( ( si = (SOCKET_INFORMATION<XBufferSize>*) GlobalAlloc( GPTR, sizeof( SOCKET_INFORMATION<XBufferSize> ) ) ) == NULL )
		{
			std::cerr << "GlobalAlloc( .. ) failed." << std::endl;
			exit( -1 );
		}

		static uint64_t _SocketIDFactory = 0;

		si->Socket        = xSocket;
		si->BytesSEND     = 0;
		si->BytesRECV     = 0;
		si->sockaddr_info = xAddressInfo;
		si->socketID 	  = ++_SocketIDFactory;

		mSocketArray[ mTotalConnectedSockets ] = si;

		++mTotalConnectedSockets;
	}

	void _unregisterSocket( uint32_t const xSocketIndex )
	{
		SOCKET_INFORMATION<XBufferSize> * si = mSocketArray[ xSocketIndex ];

		closesocket( si->Socket );

		GlobalFree( si );

		for ( int32_t ii = 0; ii < mTotalConnectedSockets; ++ii )
		{
			mSocketArray[ ii ] = mSocketArray[ ii + 1 ];
		}

		--mTotalConnectedSockets;
	}

	uint32_t _select( )
	{
		uint32_t total = 0;
		FD_ZERO( &mReadSet );
		FD_ZERO( &mWriteSet );

		FD_SET( mListenSocket, &mReadSet );

		for ( int32_t ii = 0; ii < mTotalConnectedSockets; ++ii )
		{
			bool const receivesMore = mSocketArray[ ii ]->BytesRECV > mSocketArray[ ii ]->BytesSEND;

			FD_SET( mSocketArray[ ii ]->Socket, receivesMore ? &mWriteSet : &mReadSet );
		}

		if ( ( total = select( 0, &mReadSet, &mWriteSet, NULL, NULL ) ) == SOCKET_ERROR )
		{
			std::cerr << "select( .. ) failed." << std::endl;
			exit( -1 );
		}

		return total;
	}

	void _checkForWSAEWOULDBLOCK( )
	{
		int32_t const error = WSAGetLastError( );

		if ( error != WSAEWOULDBLOCK && error != 0 )
		{
			std::cerr << "WSAGetLastError( .. ) failed." << WSAGetLastError( ) << std::endl;
			exit( -1 );
		}
	}

	void _onReceiveSelectData( uint8_t * xBuffer, DWORD const xReceiveBytes, SOCKET_INFORMATION<XBufferSize> * xSocketInformation )
	{
		mCallbackMutex.run([&]()
		{
			for ( auto && ii : mCallbacks )
			{
				ii( SelectData<XBufferSize>( xBuffer, ( uint64_t )xReceiveBytes, xSocketInformation ) );
			}
		});
	}

	void _readNonBlocking( SOCKET_INFORMATION<XBufferSize> * xSocketInformation, uint32_t const xIndex )
	{
		DWORD receiveBytes = 0, flags = 0;

		xSocketInformation->DataBuf.buf = xSocketInformation->Buffer;
		xSocketInformation->DataBuf.len = XBufferSize;

		if  ( WSARecv( xSocketInformation->Socket, &(xSocketInformation->DataBuf), 1, &receiveBytes, &flags, NULL, NULL ) == SOCKET_ERROR)
		{
			_checkForWSAEWOULDBLOCK( );
		}
		else if ( ( xSocketInformation->BytesRECV = receiveBytes ) == 0 )
		{
			_unregisterSocket( xIndex );
		}

		_onReceiveSelectData( ( uint8_t* )xSocketInformation->DataBuf.buf, receiveBytes, xSocketInformation );
	}

	void _writeNonBlocking( SOCKET_INFORMATION<XBufferSize> * xSocketInformation, uint32_t const XIndex)
	{
		DWORD sendBytes = 0;

		xSocketInformation->DataBuf.buf = xSocketInformation->Buffer  + xSocketInformation->BytesSEND;
		//static char defaultResponse[] = "OK";
		//xSocketInformation->DataBuf.buf = defaultResponse;
		//xSocketInformation->DataBuf.len = strlen( defaultResponse );

		if ( WSASend( xSocketInformation->Socket, &(xSocketInformation->DataBuf), 1, &sendBytes, 0, NULL, NULL ) == SOCKET_ERROR )
		{
			_checkForWSAEWOULDBLOCK( );
		}
		else if ( ( xSocketInformation->BytesSEND += sendBytes ) == xSocketInformation->BytesRECV )
		{
			xSocketInformation->BytesSEND =
			xSocketInformation->BytesRECV = 0;
		}
	}

	void _serviceSockets( )
	{
		for ( int32_t ii = 0; ii < mTotalConnectedSockets; ++ii )
		{
			SOCKET_INFORMATION<XBufferSize> * si = mSocketArray[ ii ];

			if ( FD_ISSET( si->Socket, &mReadSet ) )
			{
				_readNonBlocking( si, ii );
				continue;
			}

			if ( FD_ISSET( si->Socket, &mWriteSet ) )
			{
				_writeNonBlocking( si, ii );
				continue;
			}
		}
	}

	static uint32_t _selectFunction( SocketConnector * xConnector )
	{
		xConnector->_listen( );

		while ( !xConnector->mQuit.isSet( ) )
		{
			for ( uint32_t ii = xConnector->_select( ); ii > 0; --ii)
			{
				if ( FD_ISSET( xConnector->mListenSocket, &xConnector->mReadSet ) )
				{
					struct sockaddr sockaddr_info;
					int32_t sockAddrInfoLength = sizeof( sockaddr_info );

					// TODO: Put a sockaddr_info object into accept( ) and keep track of connected clients!
					if ( ( xConnector->mAcceptSocket = accept( xConnector->mListenSocket, (struct sockaddr*)&sockaddr_info, &sockAddrInfoLength ) ) == INVALID_SOCKET )
					{
						if ( WSAGetLastError( ) == WSAEWOULDBLOCK )
						{
							continue; // try again!
						}

						std::cerr << "accept( .. ) failed in _selectFunction: " << WSAGetLastError( )<< std::endl;
						exit( -1 );
					}

					xConnector->_setSocketNonBlocking( xConnector->mAcceptSocket );

					xConnector->_registerSocket( xConnector->mAcceptSocket, sockaddr_info );

					xConnector->_checkForWSAEWOULDBLOCK( );
				}

				xConnector->_serviceSockets( );
			}
		}

		return 0;
	}

	void _doRuntimeCheck( )
	{

#ifndef __linux__
		if ( mArch == eSocketConnectionArchitecture_NonblockingSelect && mProtocol != eDatagramProtocol_TCP )
		{
			std::cerr << "Only TCP/IP supports non-blocking select on Windows." << std::endl;
			exit( -1 );
		}
#endif
	}

	public:
		SocketConnector( std::string const & xIPAddress
				  , uint32_t const xPort
				  , eDatagramProtocol const xProtocol = eDatagramProtocol_UDP
				  , eSocketConnectionArchitecture const xArch = eSocketConnectionArchitecture_Synchronous )
			: mSocketArray( )
			, mRole ( eSocketConnectorRole_Client )
			, mArch( xArch )
			, mListenSocket( )
			, mAcceptSocket( )
			, mListenSocketServer( )
			, mOtherSocketIP( )
			, mSendLength( sizeof( mOtherSocketIP ) )
			, mReceiveLength( 0 )
			, mBuffer( )
			, mPort( xPort )
			, mInitialized( false )
			, mIPAddress( xIPAddress )
			, mNewSocketDescriptor( -1 )
			, mNonBlockingLastConnected( )
			, mNonBlockingLastConnectedSize( 0 )
			, mReadSet( )
			, mWriteSet( )
			, mTotalConnectedSockets( 0 )
			, mQuit( )
			, mCallbacks( )
			, mCallbackMutex( )
			, mSelectFunction( [ & ] ( SocketConnector * ) ->uint32_t { return 0; } )
			, mSelectThread( mSelectFunction )
			, mProtocol( xProtocol )

		{
			_doRuntimeCheck( );
		}

			SocketConnector( uint32_t const xPort
  	 			      , eDatagramProtocol const xProtocol = eDatagramProtocol_UDP
				      , eSocketConnectionArchitecture const xArch = eSocketConnectionArchitecture_Synchronous )
			: mSocketArray( )
			, mRole ( eSocketConnectorRole_Server )
			, mArch( xArch )
			, mListenSocket( )
			, mAcceptSocket( )
			, mListenSocketServer( )
			, mOtherSocketIP( )
			, mSendLength( sizeof( mOtherSocketIP ) )
			, mReceiveLength( 0 )
			, mBuffer( )
			, mPort( xPort )
			, mInitialized( false )
			, mIPAddress( "127.0.0.1" )
			, mNewSocketDescriptor( -1 )
			, mReadSet( )
			, mWriteSet( )
			, mTotalConnectedSockets( 0 )
			, mQuit( )
			, mCallbacks( )
			, mCallbackMutex( )
			, mSelectFunction( _selectFunction )
			, mSelectThread( mSelectFunction )
			, mProtocol( xProtocol )
		{
			_doRuntimeCheck( );
		}

		~SocketConnector( )
		{
			disconnect( );
		}


		// TODO: Fix this. Will not work with multiple clients!!!!
		// TODO: Iterate all socket options in https://msdn.microsoft.com/en-us/library/windows/desktop/ms740476(v=vs.85).aspx
		sockaddr_in * getLastConnectedClientInfo( ) { return &mOtherSocketIP; }

		void disconnect( )
		{
			mQuit.set( );

			if ( mInitialized )
			{
				closesocket( mListenSocket );
				closesocket( mAcceptSocket );
			}

			mSelectThread.join( );
		}

		void initialize( )
		{
			if ( mInitialized )
			{
				std::cerr << "Behavior of SocketConnector::initialize( .. ) is undefined when called twice." << std::endl;
				exit( -1 );
			}

			/*
			if ( ( mListenSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == INVALID_SOCKET )
			{
				std::cerr << "socket( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}
			*/

			bool const isUDP = mProtocol == eDatagramProtocol_UDP;

			if ( ( mListenSocket = WSASocket( AF_INET
							, isUDP ? SOCK_DGRAM  : SOCK_STREAM
							, isUDP ? IPPROTO_UDP : IPPROTO_TCP
							, NULL
							, 0
							, WSA_FLAG_OVERLAPPED ) ) == INVALID_SOCKET )
			{
				std::cerr << "WSAocket( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}

			if ( mArch == eSocketConnectionArchitecture_NonblockingSelect )
			{
				bool yes = true;

				if ( setsockopt( mListenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes) ) == SOCKET_ERROR )
				{
					std::cerr << "setsockopt failed." <<std::endl;
					exit( -1 );
				}

				unlink( mIPAddress.c_str( ) );
			}

			if ( mRole == eSocketConnectorRole_Server )
			{
				mListenSocketServer.sin_family 	= AF_INET;

				mListenSocketServer.sin_port   	= htons( mPort );

				/*
				if ( ( mListenSocketServer.sin_addr.S_un.S_addr = inet_addr( mIPAddress.c_str( ) ) ) == INADDR_NONE )
				{
					hostent * host = gethostbyname( mIPAddress.c_str( ) );

					CopyMemory( &mListenSocketServer.sin_addr.S_un.S_addr, host->h_addr_list[ 0 ], host->h_length );
				}
				*/
				mListenSocketServer.sin_addr.s_addr = htonl( INADDR_ANY );

				if ( bind( mListenSocket, (struct sockaddr * ) &mListenSocketServer , sizeof( mListenSocketServer ) ) == SOCKET_ERROR)
				{
					std::cerr << "bind( .. ) failed: " << WSAGetLastError( ) << std::endl;
					exit( -1 );
				}

			}
			else if ( mRole == eSocketConnectorRole_Client )
			{
				mOtherSocketIP.sin_family 	  = AF_INET;

				mOtherSocketIP.sin_port    	  = htons( mPort );

				if ( ( mOtherSocketIP.sin_addr.S_un.S_addr = inet_addr( mIPAddress.c_str( ) ) ) == INADDR_NONE )
				{
					hostent * host = gethostbyname( mIPAddress.c_str( ) );

					CopyMemory( &mOtherSocketIP.sin_addr.S_un.S_addr, host->h_addr_list[ 0 ], host->h_length );
				}

				if ( connect( mListenSocket, (struct sockaddr*)&mOtherSocketIP, sizeof(mOtherSocketIP) ) == SOCKET_ERROR )
				{
					std::cerr << "connect( .. ) failed." << std::endl;
					exit( -1 );
				}
			}

			if ( mArch == eSocketConnectionArchitecture_NonblockingSelect &&
			     mRole == eSocketConnectorRole_Server )
			{
				mSelectThread( this );
			}

			mInitialized = true;
		}

#ifndef __linux__
		void _setSocketNonBlocking( SOCKET & xSocket )
		{
			ULONG mode = 1;
			if ( ioctlsocket( xSocket, FIONBIO, &mode ) != 0 )
			{
				std::cerr << "ioctlsocket( .. ) failed." << std::endl;
				exit( -1 );
			}

		}
#endif

		void registerSelectCallback( OnReceiveSelectData xLambda )
		{
			mCallbackMutex.run( [ & ] ( ) { mCallbacks.push_back( xLambda ); } );
		}

		void _listen( )
		{
			if ( listen ( mListenSocket, 5 ) < 0 )
			{
				std::cerr << "listen( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}

#ifdef __linux__
			int32_t flags = -1;

			if ( ( flags = fcntl( fd, F_GETFL, 0 ) ) == -1 )
			{
					std::cerr << "fcntl( .. ) failed." << std::endl;
					exit( -1 );
			}

			fcntl( mListenSocket, F_SETFL, flags | O_NONBLOCK );
#else
			_setSocketNonBlocking( mListenSocket );
#endif
		}

		void send( uint8_t * const xMemory, DWORD xSize )
		{
			switch ( mArch )
			{
				case eSocketConnectionArchitecture_NonblockingSelect:
					{
						if ( mRole == eSocketConnectorRole_Client )
						{
							WSABUF dataBuf;
							dataBuf.len = xSize;
							dataBuf.buf = (char*)xMemory;

							if ( WSASend( mListenSocket, &dataBuf, 1, &xSize, 0, NULL, NULL ) == SOCKET_ERROR )
							{
								std::cerr << "WSASend( .. ) failed: " << WSAGetLastError( ) << std::endl;
								exit( -1 );
							}
							std::cout << "WSASend completed: " << xSize << " bytes." << std::endl;
						}
						else
						{
							std::cerr << "Server side send not yet implemented. Use queue instead." << std::endl;
							exit( -1 );
						}
					}
					break;
				default:
					if ( sendto( mListenSocket, (char*)xMemory, xSize, 0, (struct sockaddr*) &mOtherSocketIP, mSendLength ) == SOCKET_ERROR )
					{
						int32_t const error = WSAGetLastError( );

					     	if( error != WSAEINTR && error != WSAENOTSOCK )// WSAENOTSOCK occurs when closesocket( .. ) has been called on mListenSocket.
						{
							std::cerr << "sendto( .. ) failed: " << WSAGetLastError( ) << std::endl;
							exit( -1 );
						}
					}
					break;
			}
		}

		uint32_t receive(  uint8_t * & xMemoryOut )
		{
			switch ( mArch )
			{
				case eSocketConnectionArchitecture_NonblockingSelect:
					{
						if ( (mReceiveLength = recv( mListenSocket, mBuffer, XBufferSize, 0 )) == SOCKET_ERROR )
						{
							std::cerr << "recv( .. ) failed: " << WSAGetLastError( ) << std::endl;
							exit( -1 );
						}
					}
					break;
				default:
					{
						if ( ( mReceiveLength = recvfrom( mListenSocket, mBuffer, XBufferSize, 0, (struct sockaddr *)&mOtherSocketIP, &mSendLength ) ) == SOCKET_ERROR )
						{
							int32_t const error = WSAGetLastError( );

						     	if( error != WSAEINTR && error != WSAENOTSOCK )// WSAENOTSOCK occurs when closesocket( .. ) has been called on mListenSocket.
							{
								std::cerr << "recvfrom( .. ) failed: " << WSAGetLastError( ) << std::endl;
								exit( -1 );
							}

							mReceiveLength = 0;
						}

					}
					break;
			}

			xMemoryOut = (uint8_t*)mBuffer;

			return mReceiveLength;

		}
};

static uint32_t const sBufferSize = 512;
static uint32_t const sPort	  = 8888;

} // namespace

TEST(libopenssl_UT, createConnectorObjectOnBackgroundThreadWithoutFaults)
{
	NamedThread( [ & ] ( const char * xThreadName ) -> uint32_t { SocketConnector<sBufferSize> winsockServer(sPort); })( "server" );
}

TEST(libopenssl_UT, createConnectorObjectWithoutFaults)
{
	SocketConnector<sBufferSize> winsockServer(sPort);
}

TEST(libopenssl_UT, createOpenSSLCertificateProgramatically)
{
	OpenSSLCertificate<2048, RSA_F4> cert;
	cert.initialize( );
	cert.write("test");
}

// Ensures writes are stateless ( at least for one iteration ) ..
TEST(libopenssl_UT, runSynchronousBlockingSocketServerAndClient)
{
	{
		event quit;

		SocketConnector<sBufferSize> winsockServer(sPort);

		SocketConnector<sBufferSize> winsockClient( "127.0.0.1", sPort );

		NamedThread serverThread( [ & ] ( const char * xThreadName ) -> uint32_t
		{
			winsockServer.initialize( );

			while ( !quit.isSet( ) )
			{
				uint8_t * message = NULL;
				std::cout << "Server: Waiting for data .. " << std::endl;
				uint32_t const bytes = winsockServer.receive( message );
				if ( bytes != 0 )
				{
					std::cout << "Server: received " << bytes << " bytes .." << std::endl;
					if ( message == NULL )
					{
						std::cerr << "OpenUdpSocketConnector< .. >::receive failed( .. )." << std::endl;
						exit( -1 );
					}

					sockaddr_in * info = winsockServer.getLastConnectedClientInfo( );

					std::cout << "in -> " << inet_ntoa( info->sin_addr ) << ":" << ntohs( info->sin_port ) << std::endl;
					std::cout << ccolor( eTTYCRed, eTTYCMagenta, eModNone ) << memblock( message, bytes ) << std::endl;

					std::cout << "Server: sending " << bytes << " bytes .." << std::endl;
					winsockServer.send( message, bytes );
					std::cout << "Server: sent " << bytes << " bytes." << std::endl;
				}
			}

			std::cout << "Out thread " << xThreadName << std::endl;

			return 0;
		});

		NamedThread clientThread( [ & ] ( const char * xThreadName ) -> uint32_t
		{
			static const char * sJabberwocky[] = {  "And, as in uffish thought he stood, "
							      , " The Jabberwock, with eyes of flame, "
							      , "Came whiffling through the tulgey wood, "
							      , " And burbled as it came!"
							      , "One, two! One, two! And through and through "
							      , "      The vorpal blade went snicker-snack! "
							      , "He left it dead, and with its head "
							      , "      He went galumphing back. "
							      , "“And hast thou slain the Jabberwock? "
							      , "      Come to my arms, my beamish boy! "
							      , "O frabjous day! Callooh! Callay!” "
							      , "      He chortled in his joy. " };

			std::cout << "In thread " << xThreadName << std::endl;

			winsockClient.initialize( );

			uint8_t * buff = NULL;

			uint8_t message[ sBufferSize ];

			uint8_t index = 0;

			while ( !quit.isSet( ) )
			{
				if ( ++index > ( sizeof( sJabberwocky ) / sizeof( const char * ) ) - 1 )
				{
					index = 0;
				}

				memcpy( message, sJabberwocky[ index ], strlen(sJabberwocky[ index ] ) + 1 );

				winsockClient.send( message, strlen((char*)message));

				uint32_t const bytes = winsockClient.receive( buff );

				if ( bytes != 0 )
				{

					if ( buff == NULL )
					{
						//std::cerr << "SocketConnector< .. >::receive failed." << std::endl;
						exit( -1 );
					}

					std::cout << memblock( buff, bytes ) << std::endl;;
				}
			}

			std::cout << "Out thread " << xThreadName << std::endl;
			return 0;

		});

		serverThread( "server" );

		clientThread( "client" );

		sleep::ms( 3000 );

		quit.set( );
		winsockServer.disconnect( );
		winsockClient.disconnect( );
	}
}

TEST(libopenssl_UT, runNonBlockingSelectServerAndClient)
{
	static uint32_t const sBufferSize = 512;
	static uint32_t const sPort	  = 888;


	event quit;

	const char * reply = "Arrr!";


	SocketConnector<sBufferSize> winsockServer( sPort
					          , eDatagramProtocol_TCP
						  , eSocketConnectionArchitecture_NonblockingSelect );

	winsockServer.registerSelectCallback( [ & ] ( SelectData<sBufferSize> const & xSelectData )
	{
		std::cout << ccolor( eTTYCBlack, eTTYCMagenta, eModBold )
			  << "Server received " << xSelectData.size <<  " bytes."
			  << memblock( xSelectData.data, xSelectData.size )
			  << " from socket # " << xSelectData.socket->socketID
			  << " and did nothing with it."
			  << std::endl;
	});

	winsockServer.initialize( );

	// Server thread should now be running.

	auto getRandomColor = [&]()->eTTYColor
	{
		static eTTYColor sColors[] = {  eTTYCBlack
        				      , eTTYCRed
        				      , eTTYCGreen
        				      , eTTYCYellow
        				      , eTTYCBlue
        				      , eTTYCMagenta
        				      , eTTYCCyan
        				      , eTTYCWhite
       					      , eTTYCMax  };

		return sColors[ rand( ) % ( sizeof( sColors ) / sizeof( eTTYColor ) )];
	};


	NamedThread clientThread( [ & ] ( const char * xName ) -> uint32_t
	{
		static const char * sJabberwocky[] = {  "And, as in uffish thought he stood, "
						      , " The Jabberwock, with eyes of flame, "
						      , "Came whiffling through the tulgey wood, "
						      , " And burbled as it came!"
						      , "One, two! One, two! And through and through "
						      , "      The vorpal blade went snicker-snack! "
						      , "He left it dead, and with its head "
						      , "      He went galumphing back. "
						      , "“And hast thou slain the Jabberwock? "
						      , "      Come to my arms, my beamish boy! "
						      , "O frabjous day! Callooh! Callay!” "
						      , "      He chortled in his joy. " };


		std::cout << "In thread " << xName << std::endl;

		SocketConnector<sBufferSize> winsockClient( "127.0.0.1"
							      , sPort
							      , eDatagramProtocol_TCP
							      , eSocketConnectionArchitecture_NonblockingSelect );

		winsockClient.initialize( );

		uint8_t * buff = NULL;

		uint8_t message[ sBufferSize ];

		uint8_t index = 0;

		bool once = false;

		eTTYColor fg = getRandomColor( ), bg = getRandomColor( );
		while ( !quit.isSet( ) )
		{
			if ( ++index > ( sizeof( sJabberwocky ) / sizeof( const char * ) ) - 1 )
			{
				index = 0;
			}

			memcpy( message, sJabberwocky[ index ], strlen(sJabberwocky[ index ] ) + 1 );
			if (!once)
			{
				winsockClient.send( message, strlen((char*)message));

				once = true;
			}

			//static int32_t times = 0;
			//std::cout << "Client: sent " << ++times << std::endl;

			uint32_t const bytes = winsockClient.receive( buff );

			if ( buff == NULL )
			{
				std::cerr << "SocketConnector< .. >::receive failed." << std::endl;
				exit( -1 );
			}

			if ( bytes == 0 )
			{
				continue;
			}

			std::cout << ccolor( bg, fg, eModBold )
				   << "Client received " << bytes << std::endl
				   <<memblock( buff, sBufferSize ) << std::endl;
		}

		winsockClient.disconnect( );

		std::cout << "Out thread " << xName << std::endl;
	});

	clientThread( "Client thread 01" );
	clientThread( "Client thread 02" );
	clientThread( "Client thread 03" );

	sleep::ms( 10000 );

	std::cout << "Quitting from main thread." << std::endl;
	quit.set( );

	winsockServer.disconnect( );

	std::cout << "Main thread is out, next is dtors." << std::endl;

	BLOCK_EXECUTION;
}
#endif
