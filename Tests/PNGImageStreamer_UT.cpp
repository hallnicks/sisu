
#include "test.hpp"
#include "SDLShaderTest.hpp"

#include <sstream>
#include <functional>

#include <concurrentqueue.h>

#include "threadgears.hpp"
#include "PNGImage.hpp"
#include "ioassist.hpp"

using namespace sisu;

namespace
{
	class PNGImageStreamer_UT : public context
	{
		public:
			PNGImageStreamer_UT( ) : context( ) { }
			void Up( ) { }
			void Down( ) { }
	};


	void listFilesMatchingExtension( const char * xPath
					, const char * xExtension )
	{

		auto isExtension = [ & ]( const char* xPath )
		{
			PTSTR extension = PathFindExtension( xPath );
			return strcmp( extension, xExtension ) == 0;
		};

		doPerFile( xPath, isExtension, [ & ] ( const char * xName ) { std::cout << xName << ' '; } );
	}

	template< typename XQueueType, typename XQueueDataType >
	void queueFilesMatchingExtension( const char * xPath
				        , const char * xExtension
				        , XQueueType & xUpdate )
	{
		auto isExtension = [ & ]( const char* xPath )
		{
			PTSTR extension = PathFindExtension( xPath );

			return strcmp( extension, xExtension ) == 0;
		};

		doPerFile( xPath, isExtension, [ & ]( const char * xName ) { xUpdate.enqueue( XQueueDataType( xName ) ); } );
	}


} // namespace

TEST(PNGImageStreamer_UT, listPNGFiles)
{
	listFilesMatchingExtension( "E:\\data", ".png" );
}

typedef moodycamel::ConcurrentQueue< std::string > MessageQueue;

typedef gear< uint8_t, uint8_t > Thread;

TEST(PNGImageStreamer_UT, loadPNGFiles)
{
	event quit;

	MessageQueue q;

	SpriteShader window( true );

	Thread loadFilesThread( [ & ]( uint8_t )-> uint8_t
	{
		do
		{
			queueFilesMatchingExtension< MessageQueue, std::string >( "resources/", ".png", q);

		} while (!quit.isSet( ) );
		return 0;
	} );

	Thread loadPNGsThread( [ & ]( uint8_t )-> uint8_t
	{
		while ( !quit.isSet( ) )
		{
			std::string file;

			if ( !q.try_dequeue( file ) )
			{
				continue;
			}

			PNGImage image( file.c_str( ) );

			if ( image.getIsValid( ) && image.getWidth( ) <= 1920 && image.getHeight( ) <= 1080 )
			{
				GLubyte * allocatedRawTextureData = image.toGLTextureBuffer( );
				window.enqueue( allocatedRawTextureData, image.getWidth( ), image.getHeight( ) );
			}
		}
		return 0;
	} );

	loadFilesThread( 0 );

	window.initialize( { 4, 4, 0, false, false } );

	loadPNGsThread( 0 );

	window.run( );

	quit.set( );
}
