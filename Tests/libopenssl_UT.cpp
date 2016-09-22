#if 0
#include <fstream>
#ifndef __linux__
#include <winsock2.h>
#else
#error Socket port is sorely needed.
#endif

#include "test.hpp"


// C style IO is necessary to talk to libpng. This could be seen as a wrapper between stl-style c++ and libpng

#include "memblock.hpp"
#include "ttyc.hpp"
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


template< uint32_t XBufferSize>
class UDPSocketConnector
{
	enum _eUDPSocketConnectorRole
	{
		_eUDPSocketConnectorRole_Server,
		_eUDPSocketConnectorRole_Client,
	} mRole;

	enum _eUDPSocketConnectorArchitecture
	{
		_eUDPSocketConnectorRole_Synchronous,
		_eUDPSocketConnectorRole_NonnblockingSelect,
		_eUDPSocketConnectorRole_Asynchronous,
	} mArch;

	SOCKET mSocket;
	struct sockaddr_in mSocketServer, mSocketOther;
	int32_t mSendLength, mReceiveLength;
	char mBuffer[ XBufferSize ];
	uint32_t const mPort;
	bool mInitialized;

	public:
		UDPSocketConnector( uint32_t const xPort )
			: mRole ( _eUDPSocketConnectorRole_Server )
			, mSocket( )
			, mSocketServer( )
			, mSocketOther( )
			, mSendLength( sizeof( mSocketOther ) )
			, mReceiveLength( 0 )
			, mBuffer( )
			, mPort( xPort )
			, mInitialized( false )
			, mArch( _eUDPSocketCOnnectionArchitecture_Synchronous )
		{
				;
		}

		~UDPSocketConnector( )
		{
			if ( mInitialized )
			{
				closesocket( mSocket );
			}
		}

		void initialize( )
		{
			if ( mInitialized )
			{
				std::cerr << "Behavior of UDPSocketConnector::initialize( .. ) is undefined when called twice." << std::endl;
				exit( -1 );
			}

			if ( ( mSocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) == INVALID_SOCKET )
			{
				std::cerr << "socket( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}

			std::cout << "Server socket created." << std::endl;

			mSocketServer.sin_family 	= AF_INET;
			mSocketServer.sin_addr.s_addr	= INADDR_ANY;
			mSocketServer.sin_port 		= htons( mPort );

			// Bind socket
			if ( bind( mSocket, (struct sockaddr * ) &mSocketServer , sizeof( mSocketServer ) ) == SOCKET_ERROR)
			{
				std::cerr << "bind( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}

			std::cout << "Server socket bound." << std::endl;

			mInitialized = true;
		}


		void send( uint8_t * const xMemory, uint32_t const xSize)
		{
			if ( sendto( mSocket, (char*)xMemory, xSize, 0, (struct sockaddr*) &mSocketOther, sSendLength ) == SOCKET_ERROR )
			{
				std::cerr << "sendto( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}
		}


		uin32_t receive(  uint8_t * & xMemoryOut )
		{
			// clear buffer (eliminate step )
			memset( mBuffer, '\0', XBufferSize );

			if ( ( mReceiveLength = recvfrom( mSocket, mBuffer, XBufferSize, 0, (struct sockaddr *)&mSocketOther, &mSendLength) ) == SOCKET_ERROR )
			{
				std::cerr << "recvfrom( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}

			xMemoryOut = (uint8_t*)mBuffer;

			return mReceiveLength;
		}
};

} // namespace


// Ensures writes are stateless ( at least for one iteration ) ..
TEST(libopenssl_UT, runSynchronousBlockingServerAndCLient)
{
	OpenSSLCertificate<2048, RSA_F4> cert;
	cert.initialize( );
	cert.write("test");


	OpenSSLConnection server(true);
	OpenSSLConnection client;

	gear<uint32_t, OpenSSLConnection* > serverThread( [ & ] ( OpenSSLConnection * xSSLConnection ) -> uint32_t
	{
#if 0
		static uint32_t const sBufferSize = 512;
		static uint32_t const sPort	  = 888;
		SOCKET sock;
		struct sockaddr_in socketServer, socketOther;
		int32_t sLen, recvLen;
		char buff[ sBufferSize ]; // todo - make buff length configurable

		sLen = sizeof( socketOther);

		if ( ( sock = socket( AF_INET, SOCK_DGRAM, 0 ) ) == INVALID_SOCKET )
		{
			std::cerr << "socket( .. ) failed: " << WSAGetLastError( ) << std::endl;
			exit( -1 );
		}

		std::cout << "Server socket created." << std::endl;

		socketServer.sin_family 	= AF_INET;
		socketServer.sin_addr.s_addr	= INADDR_ANY;
		socketServer.sin_port 		= htons( sPort );

		// Bind socket
		if ( bind( sock, (struct sockaddr * )&socketServer, sizeof(socketServer) ) == SOCKET_ERROR)
		{
			std::cerr << "bind( .. ) failed: " << WSAGetLastError( ) << std::endl;
			exit( -1 );
		}

		std::cout << "Server socket bound." << std::endl;

		while (1)
		{
			std::cout << std::flush;


			if ( ( recvLen = recvfrom( sock, buff, sBufferSize, 0, (struct sockaddr *)&socketOther, &sLen) ) == SOCKET_ERROR )
			{
				std::cerr << "recvfrom( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}

			std::cout << "in -> " << inet_ntoa( socketOther.sin_addr ) << ":" << ntohs( socketOther.sin_port ) << std::endl;
			std::cout << "[" << buff << "]" << std::endl;

			// echo back to client.
			if ( sendto( sock, buff, recvLen, 0, (struct sockaddr*) &socketOther, sLen ) == SOCKET_ERROR )
			{
				std::cerr << "sendto( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}

		}

		closesocket( sock );
#endif

		OpenUDPSocketConnector<512> winsockServer(888);
	});

	serverThread( &server );

	gear<uint32_t, OpenSSLConnection* > clientThread( [ & ] ( OpenSSLConnection * xSSLConnection ) -> uint32_t
	{
		static uint32_t const sBufferSize = 512;
		static uint32_t const sPort	  = 888;

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

		struct sockaddr_in sockOther;

		SOCKET sock;

		int sLen = sizeof( sockOther );

		char buff[ sBufferSize ], message[ sBufferSize ];

		if ( ( sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) == SOCKET_ERROR )
		{
			std::cerr << "socket( .. ) failed: " << WSAGetLastError( ) << std::endl;
			exit( -1 );
		}

		memset( ( char* )&sockOther, 0, sizeof(sockOther));

		sockOther.sin_family 		= AF_INET;
		sockOther.sin_port   		= htons( sPort );
		sockOther.sin_addr.S_un.S_addr 	= inet_addr( "127.0.0.1" );

		uint8_t index = 0;

		while ( 1 )
		{
			if ( ++index >= 10)
			{
				index = 0;
			}

			// Copy the message into the buffer
			memcpy( message, sJabberwocky[ index ], strlen(sJabberwocky[ index ] ) + 1 );

			if ( sendto( sock, message, strlen(message), 0, (struct sockaddr*)&sockOther, sLen ) == SOCKET_ERROR )
			{
				std::cerr << "sendto( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}

			memset( buff, '\0', sBufferSize );

			if ( recvfrom( sock, buff, sBufferSize, 0, (struct sockaddr *)&sockOther, &sLen ) == SOCKET_ERROR )
			{

				std::cerr << "recvfrom( .. ) failed: " << WSAGetLastError( ) << std::endl;
				exit( -1 );
			}

			//std::cout << buff;
		}

		closesocket( sock );

	});

	clientThread( &client );

	while(1) {
		std::cout << "Main thread still active." << std::endl;
		sleep::ms( 1000 );
	};
}
#endif
