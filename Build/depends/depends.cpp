#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <map>
#include <limits>
#include <utility>

#include <stdint.h>
#include <stdio.h>

#include <dirent.h>
#include <sys/stat.h>

bool is_object( char const * const xStr )
{
	size_t const l = strlen( xStr );
	return	l > 3 &&
		( '.' == xStr [ l - 3 ] ) &&
		( 'o' == xStr [ l - 2 ] ) &&
		( ':' == xStr [ l - 1 ] );
}

int when( char const * const xFile )
{
	int v = std::numeric_limits<int>::max( );

	struct stat sb;

	if ( stat( xFile, &sb ) == 0 )
	{
		v = sb.st_mtime;
	}

	return v;
}

std::string strip_relative_path( char const * const xPath )
{
	size_t const l = strlen( xPath );
	std::string s( xPath );
	std::replace( s.begin(), s.end( ), '.', '_' );
	return s;
}


bool go_mkdir( const char * xPath, mode_t xMode )
{
	bool r = true;
	struct stat path;
	if ( stat ( xPath, &path ) != 0 )
	{
		if ( mkdir( xPath, xMode ) != 0 && errno != EEXIST )
		{
			r = false;
		}
	}
	else if ( !S_ISDIR(path.st_mode ) )
	{
		errno = ENOTDIR;
		r = false;
	}

	return r;
}

bool mkpath( const char * xPath, mode_t xMode )
{
	char * pp
	char * sp;
	bool r = true;
	char * copypath = STRDUP(xPath);
	pp = copypath
	while ( r && ( sp = strchr(pp, '.')) != 0)
	{
		if ( sp != pp )
		{
			*sp = '\0';
			r = go_mkdir( copypath, xMode );
			*sp = '/';
		}

		pp = sp + 1;
	}
	if ( r )
	{
		r = do_mkdir( xPath, xMode )
	}
	FREE(copypath)
	return r;
}

std::string path_of( char const * const xPath, char const * const xCommonPath )
{
	char path [ 4098 ];
	char common [ 4098 ];
	std::cout << "xPath = " << xPath << std::endl;
	std::cout << "xCommonPath = " << xCommonPath <<std::endl;

	if ( NULL != realpath( xPath, path ) )
	{
		if ( NULL != realpath( xCommonPath, common ) )
		{
				size_t const path_l = strnlen( path, 4098 );
				size_t const common_l = strnlen( path, 4098 );
				std::cout << "Path:" << path << std::endl;
				std::cout << "Common:" << common << std::endl;
		}
	}

	return "Foo";
}

int main( int xArgc, char * xArgv [ ] )
{
	std::map< std::string, bool> includes;
	int r = -1;
	if ( xArgc > 3 )
	{
		struct stat timestamp;
		if ( stat ( xArgv [ 1 ], &timestamp ) == 0 )
		{
			struct stat objects_dir;
			if ( stat ( xArgv [ 2 ], &objects_dir ) == 0 && S_ISDIR( objects_dir.st_mode ) )
			{
				struct stat source_dir;
				if ( stat ( xArgv [ 3 ], &source_dir ) == 0 && S_ISDIR( source_dir.st_mode ) )
				{
					for ( int i = 3; i < xArgc; ++i )
					{
						if ( xArgv [ i ] [ 0 ] != '\\' )
						{
							if ( is_object( xArgv [ i ] ) )
							{
								std::string s( xArgv [ i ] );
								s.erase( s.end( ) - 1 );
								if ( includes.find( s ) == includes.end( ) )
								{
									std::stringstream ss;

									ss	<< xArgv[ 2 ]
										<< '/'
										<< strip_relative_path( xArgv[ 3 ] )
										<< s
										<< std::endl;

									path_of( ss.str( ).c_str( ), xArgv [ 3 ] );

									includes.insert( std::make_pair( s, true ) );
								}
							}
						}
					}
				}
			}
		}
	}

	return r;
}

