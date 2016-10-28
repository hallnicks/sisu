#ifndef IO_ASSIST74351C5C55AF4767BDEB912C3A472466_HPP_
#define IO_ASSIST74351C5C55AF4767BDEB912C3A472466_HPP_

#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <iostream>
#include <functional>
#include <sstream>

#ifdef WIN32
#include <Windows.h>
#include <dirent.h>
#include <Shlwapi.h>
#endif

#ifdef ANDROID
#include <zip.h>
#include <sstream>
#include "memblock.hpp"
#endif

#include <SDL2/SDL.h>
#include "AndroidLogWrapper.hpp"

namespace sisu {

static inline bool fileExists( const char * xPathToFile ) { return std::ifstream( xPathToFile ).good( ); }

static inline std::string fileToString( const char * xPathToFile ) {
	std::ifstream ifs( xPathToFile );

	if ( !ifs.good( ) )
	{
		std::cerr << xPathToFile << " is not a path to a valid file." << std::endl;
		exit( -1 );
	}

	return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

static inline char* file_read(const char* filename, size_t * outSize ) {
        SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
        if (rw == NULL) return NULL;

        Sint64 res_size = SDL_RWsize(rw);
	*outSize = res_size;
        char* res = (char*)malloc(res_size + 1);

        Sint64 nb_read_total = 0, nb_read = 1;
        char* buf = res;
        while (nb_read_total < res_size && nb_read != 0) {
                nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
                nb_read_total += nb_read;
                buf += nb_read;
        }
        SDL_RWclose(rw);
        if (nb_read_total != res_size) {
                free(res);
                return NULL;
        }

        res[nb_read_total] = '\0';
        return res;
}


#ifdef ANDROID
template <typename XByte>
static inline std::vector<XByte> fileToMemory( const char * xPathToFile, bool xDecompress = false )
#else
template <typename XByte>
static inline std::vector<XByte> fileToMemory( const char * xPathToFile )
#endif
{
//#ifdef ANDROID
// TODO: Test for SDL
#if 1
	/*
	SDL_RWops * file = SDL_RWFromFile(xPathToFile, "rb");

	if ( file == NULL )
	{
		SISULOG(" SDL_RWFromFile failed." );
		exit( -1 );
	}

	Sint64 const length = SDL_RWseek( file, 0, RW_SEEK_END );

	if ( length < 0 )
	{
		SISULOG("SDL_RWseek( .., 0, RW_SEEK_END ) failed.");
	}

	if ( SDL_RWseek( file, 0, RW_SEEK_SET ) < 0 )
	{
		SISULOG( "SDL_RWseek failed." );
		exit( -1 );
	}

	std::vector<XByte> buffer( length );

	if ( SDL_RWread( file, reinterpret_cast<char*>( buffer.data( ) ), length, 1 ) <= 0 )
	{
		SISULOG("Could not read file into memory.");
		exit( -1 );
	}

	SDL_RWclose( file );
	*/
	size_t out;
	char * buff = file_read( xPathToFile, &out );
	std::vector<XByte> buffer;

	buffer.assign( buff, buff + out );

	return buffer;
#ifdef ANDROID
#if 0
	// Decompress asset file in memory if it is a zip.
	// TODO: Test for zip header.
	if ( xDecompress )
	{
 		//Open the ZIP archive
		zip_error    zerr;
		zip_source * zs = zip_source_buffer_create( buffer.data( ), length, 0, &zerr );

		if ( zs == NULL )
		{
			SISULOG("zip_source_buffer_create failed.");
			exit( -1 );
		}

		zip * z	= zip_open_from_source( zs, ZIP_RDONLY, &zerr );

		if ( z == NULL )
		{
			__android_log_print( ANDROID_LOG_VERBOSE
                                         , "sisu"
                                         , "zip_open_from_source failed with error %d"
                                         , (int)zip_error_code_zip( &zerr ) );
			exit( -1 );
		}

		//Search for the file of given name
		struct zip_stat st;
		zip_stat_init(&st);
		zip_stat(z, xPathToFile, 0, &st);

		//Alloc memory for its uncompressed contents
		//char *contents = new char[st.size];
		std::vector<XByte> decompressedBuffer( st.size );

		//Read the compressed file
		zip_file *f = zip_fopen(z, xPathToFile, 0);
		zip_fread(f, decompressedBuffer.data( ), st.size);
		zip_fclose(f);

		//And close the archive
		zip_close(z);

		//Do something with the contents
		//delete allocated memory
		return decompressedBuffer;
	}

	return buffer;
#endif
#endif

#else
        std::ifstream file( xPathToFile, std::ios::binary | std::ios::ate );

        std::streamsize const size = file.tellg( );

        file.seekg( 0, std::ios::beg );

        std::vector<XByte> buffer( size );

        if ( !file.read( reinterpret_cast<char*>( buffer.data() ), size ) )
        {
                std::cerr << "Could not read file into memory." << std::endl;
                exit( -1 );
        }

	return buffer;
#endif
}

static inline bool filesAreEqual( const char * xLeft, const char * xRight )
{
	std::vector<char> const left = fileToMemory<char>( xLeft );
	std::vector<char> const right = fileToMemory<char>( xRight );

	if ( left.size( ) != right.size( ) )
	{
		return false;
	}

	for ( int ii = 0; ii < left.size( ); ++ii )
	{
		if ( left[ii] != right[ii] )
		{
			return false;
		}
	}

	return true;
}

static void doPerFile( const char * xPath
                      , std::function<bool(const char*)> xFilter
                      , std::function<void(const char*)> xPerFile )
{
#ifdef WIN32
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
#else
	std::cerr << __PRETTY_FUNCTION__ << " is not implemented on this platform." << std::endl;
	exit( -1 );
#endif
}

} // namespace

#endif // IO_ASSIST74351C5C55AF4767BDEB912C3A472466_HPP_



