#ifndef IO_ASSIST74351C5C55AF4767BDEB912C3A472466_HPP_
#define IO_ASSIST74351C5C55AF4767BDEB912C3A472466_HPP_

#include <string>
#include <fstream>
#include <streambuf>

namespace sisu { 


static inline std::string fileToString( const char * xPathToFile ) {
	std::ifstream ifs( xPathToFile );
	return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

}

#endif // IO_ASSIST74351C5C55AF4767BDEB912C3A472466_HPP_



