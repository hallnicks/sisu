#include "test.hpp"
#include "SDLShaderTest.hpp"

#include <Windows.h>
#include <dirent.h>
#include <Shlwapi.h>

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



	void doPerFile( const char * xPath
		        , std::function<bool(const char*)> xFilter
			, std::function<void(const char*)> xPerFile )
	{
		DIR * directory = NULL;

		struct dirent * entry;

		if ( ( directory = opendir( xPath ) ) == NULL )
		{
			return;
		}

		while ( ( entry = readdir( directory ) ) != NULL )
		{
			if ( ( strcmp( entry->d_name, "."  ) == 0 ) ||
			     ( strcmp( entry->d_name, ".." ) == 0 ) )
			{
				continue;
			}

			std::stringstream ss;

			ss << xPath << "\\" << entry->d_name;

			DWORD fileAttributes = GetFileAttributes( ss.str( ).c_str( ) );

			if ( fileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				doPerFile( ss.str( ).c_str( ), xFilter, xPerFile );
				continue;
			}

			if ( xFilter( entry->d_name ) )
			{
				xPerFile( ss.str( ).c_str( ) );
			}
		}

		closedir( directory );
	}


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
			queueFilesMatchingExtension< MessageQueue, std::string >( "E:\\data", ".png", q);
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

