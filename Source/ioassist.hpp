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

static inline char* fileToBuffer(const char* xFilename, size_t * xOutSize ) {

	if ( xFilename == NULL || xOutSize == NULL )
	{
		SISULOG("Bad parameters passed to fileToBuffer");
		exit( -1 );
	}

        SDL_RWops *rw = SDL_RWFromFile( xFilename, "rb" );

        if (rw == NULL)
	{
		SISULOG( "SDL_RwFromFile failed." );
		exit( -1 );
	}

        *xOutSize = SDL_RWsize(rw);

        char* res = (char*)malloc( *xOutSize  + 1 );

        Sint64 nb_read_total = 0, nb_read = 1;

        char* buf = res;

        while (nb_read_total < *xOutSize && nb_read != 0)
	{
                nb_read = SDL_RWread(rw, buf, 1, ( *xOutSize - nb_read_total));
                nb_read_total += nb_read;
                buf += nb_read;
        }

        SDL_RWclose(rw);

        if (nb_read_total != *xOutSize)
	{
                free(res);
		SISULOG( "SDL_Rwread had unexpected results." );
                exit( -1 );
        }

        res[nb_read_total] = '\0';

        return res;
}


template <typename XByte>
static inline std::vector<XByte> fileToMemory( const char * xPathToFile )
{
	size_t out;

	char * buff = fileToBuffer( xPathToFile, &out );

	std::vector<XByte> buffer;

	buffer.assign( buff, buff + out );

	return buffer;
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



