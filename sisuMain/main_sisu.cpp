//    This file is part of sisu.

//    sisu is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    sisu is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with sisu.  If not, see <http://www.gnu.org/licenses/>.
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

#include <SDL2/SDL.h>

namespace
{
#if 0

	class Directory
	{
		std::string const & mPath;

		int const mStatus;

		public:
			// TODO -- Fix
			Directory( std::string const & xPath )
				: mPath( xPath )
				, mStatus( mkdir( mPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) )
			{
				;
			}

			std::string const & getPath( ) const { return mPath; }

			int const & getStatus( ) const { return mStatus; }

			template<size_t XSize>
			static void createDirectories( std::string const (& sDirectories )[ XSize ] )
			{
				for ( unsigned i = 0; i < XSize; ++i )
				{
					Directory dir( sDirectories[ i ].c_str( ) );

					if ( dir.getStatus( ) != 0 )
					{
						std::cerr << "Failed to create directory \"" << dir.getPath( ).c_str( ) << "\"" << std::endl;

						exit( -1 );
					}
					else
					{
						std::cout << "Persisted " << dir.getPath( ).c_str( ) << std::endl;
					}
				}
			}
	}; // class Directory
#endif
} // namespace


int main( int xArgc, char * xArgv[] )
{
	std::cout << "Hello, User." << std::endl;
#if 0
	if ( xArgc > 1 )
	{
		std::string param( xArgv[ 1 ]  );

		param.erase( remove_if( param.begin( ), param.end( ), isspace ), param.end( ) );

		std::string const sDirectories[] = { param
						   , param + "/Make"
						   , param + "/" + param + "Main"
						   , param + "/testMain"
						   , param + "/Source"
						   , param + "/Tests" };

		std::cout << "Creating directories..." << std::endl; 
		for (int i = 0; i < sizeof(sDirectories); ++i) 
		{
			std::cout << sDirectories[i] <<std::endl;
		}
		Directory::createDirectories( sDirectories );
	}

#endif
	return 0;
}

