#ifndef IO_ASSIST74351C5C55AF4767BDEB912C3A472466_HPP_
#define IO_ASSIST74351C5C55AF4767BDEB912C3A472466_HPP_

#include "test.hpp" // TODO - delete this and clean up file. strictly for debugging.

#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <iostream>

namespace sisu { 


static inline std::string fileToString( const char * xPathToFile ) {
	std::ifstream ifs( xPathToFile );

	if ( !ifs.good( ) )
	{
		std::cerr << xPathToFile << " is not a path to a valid file." << std::endl;
		exit( -1 );
	}

	return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}



template <typename XByte>
static inline std::vector<XByte> fileToMemory( const char * xPathToFile )
{
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


} // namespace

#endif // IO_ASSIST74351C5C55AF4767BDEB912C3A472466_HPP_



