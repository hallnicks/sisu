#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
template<size_t XSize>
void fold( int xArgc, char * xArgv [ ] )
{	char r[ XSize ], out [ XSize * 2], in [ XSize * 2 ];
	static size_t const albedo = XSize - 3;
	realpath( xArgv[ 1 ], r );
	size_t const zr = strlen( r );
	for ( int i = 2; zr <= XSize && i < xArgc; ++i )
	{
		size_t const sl = strlen( xArgv[ i ] );
		char const last_char = xArgv [ i ] [ 0 ];
		struct stat f;
		bool const exists = stat( xArgv [ i ], &f ) == 0;
		if ( !exists && sl != 1 && '\\' != last_char )
		{
			size_t const zi = strlen( xArgv [ i ] );
			realpath( xArgv [ i ], in );
			int idx = -1;
			bool horizon = false;

			for ( int z = 0; (zi < XSize && z < zi && idx != albedo); ++z )
			{
				if ( z < zr && z < zi && r [ z ] != in [ z ] )
				{
					out [ ++ idx ] = in [ z ];
				}
			}

			out [ out [ idx ] == ':' ? idx : ++idx ] = '\0';

			std::cout << r;
			if ( '/' !=  r [ zr - 1 ] )
				{ std::cout << '/'; }

			std::cout << out << ":end" << std::endl;
}	}	}
int main( int xArgc, char * xArgv [ ] )
	{ if ( xArgc > 2 )
		{ fold< 4098 >( xArgc, xArgv ); }
			return 0; }
