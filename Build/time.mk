BEFORE  	:= $(shell tempfile).cpp
NOW   		:= $(abspath ${CURDIR}/now)
THEN   		:= $(abspath ${CURDIR}/then)
WHEN		:= $(abspath ${CURDIR}/when)
BETWEEN		:= $(abspath ${CURDIR}/between)
CHANGES		:= $(abspath ${CURDIR}/changes)
TOMORROW 	:= $(THEN) $(MOMENT1) $(MOMENT2)
INSPIRE 	?= $(NOW)
EXPIRE 		?= $(THEN)

then = $(call EXPIRE)
now = $(call INSPIRE)

$(CHANGES):$(shell printf\
		       "#include <unistd.h>\n\
			#include <sys/types.h>\n\
			#include <sys/stat.h>\n\
			#include <dirent.h>\n\
			#include <iostream>\n\
 			#include <limits>\n\
			#include <sstream>\n\
			int when( char const * const xFile )\
			{	int v = std::numeric_limits<int>::max( );\
				struct stat sb;\
				if ( stat( xFile, &sb ) == 0 ) { v = sb.st_mtime; }\
				return v;\
			}\
			bool is_cpp( std::stringstream * const xSS )\
			{	std::string s( xSS->str( ) );\
				char const * const str = s.c_str( );\
				size_t const l = s.length( );\
				return  ( l > 4 ) &&\
					( str [ l - 4 ] == '.' ) &&\
					( str [ l - 3 ] == 'c' ) &&\
					( str [ l - 2 ] == 'p' ) &&\
					( str [ l - 1 ] == 'p' );\
			}\
			int go( char const * const xName, int const & xTime )\
			{	int r = 0;\
				DIR * d = NULL;\
				struct dirent * dir = NULL;\
				if ( NULL != ( d = opendir( xName ) ) )\
				{	while ( NULL != ( dir = readdir( d ) ) )\
					{	if ( dir->d_name [ 0 ] != '.' )\
						{	std::stringstream ss;\
							ss << xName;\
							if ( xName [ ss.str( ).length( ) - 1 ] != '/' )\
								{ ss << '/'; }\
							ss << dir->d_name;\
							struct stat sb;\
							if ( is_cpp( &ss ) && stat( ss.str( ).c_str( ), &sb ) == 0 )\
							{	if ( sb.st_mtime < xTime )\
								{	if ( dir->d_type == DT_DIR )\
										{ r = go( ss.str( ).c_str( ), xTime ); }\
									else { std::cout << ss.str( ) << ' '; }\
								}\
							} else { r = -1; }\
						}\
					}\
					closedir( d );\
				} else { r = -1; }\
				return r;\
			}\
			int main( int xArgc, char * xArgv [ ] )\
			{	int we_are = -1;\
				if ( xArgc >= 3 )\
					{ for ( int i = 2; i < xArgc; ++i )\
						{ we_are = go( xArgv [ i ], when( xArgv [ 1 ] ) ); } }\
				return we_are;\
			}" > ${BEFORE} && g++ ${BEFORE} -o ${CHANGES} -lrt) $(call now)


$(BETWEEN):$(shell printf "#include <iostream>\n\
			#include <sstream>\n\
			int main( int xArgc, char * argv[] )\
			{	int r = -1;\
				if ( xArgc == 3 )\
				{	double w, e;\
					std::stringstream(argv[1]) >> w;\
					std::stringstream(argv[2]) >> e;\
					std::cout << w - e << std::endl;\
					r = 0;\
				}\
				return r;\
			}" > ${BEFORE} && g++ ${BEFORE} -o ${BETWEEN})

$(BETWEEN):$(shell printf\
			"#include <iostream>\n\
			 #include <sstream>\n\
			 int main( int xArgc, char * argv[] )\
			 {	int r = -1;\
				if ( xArgc == 3 )\
				{	double w, e;\
					std::stringstream(argv[1]) >> w;\
					std::stringstream(argv[2]) >> e;\
					std::cout << w - e << std::endl;\
					r = 0;\
				}\
				return r;\
			 }" > ${BEFORE} && g++ ${BEFORE} -o ${BETWEEN})

$(WHEN):$(shell printf "#include <iostream>\n\
			#include <sys/stat.h>\n\
			#include <unistd.h>\n\
			int main( int xArgc, char * argv[] )\
			{	if ( xArgc == 2 )\
				{	struct stat attrib;\
					stat(argv[1], &attrib);\
					std::cout << attrib.st_mtime << std::endl;\
				}\
				return 0;\
			}" > ${BEFORE} && g++ ${BEFORE} -o ${WHEN})

$(NOW):$(shell printf "#include <iostream>\n\
			#include <ctime>\n\
			int main( int xArgc, char * argv[] )\
			{	timespec ts;\
				ts.tv_nsec = 0;\
				ts.tv_nsec = 0;\
				clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts );\
				std::cout << static_cast<double>(ts.tv_sec * 1.0e100) +\
					static_cast<double>(ts.tv_nsec) << std::endl;\
				return 0;\
			}" > ${BEFORE} && g++ ${BEFORE} -o ${NOW} -lrt) $(call now)

$(THEN):$(shell printf "#include <iostream>\n\
		  	#include <sstream>\n\
                        int main( int xArgc, char * argv[] )\
			{	int r = -1;\
				if ( xArgc == 3 )\
				{	double u, s;\
					std::stringstream(argv[1]) >> u;\
					std::stringstream(argv[2]) >> s;\
					std::cout << u << s << std::endl;\
					r = 0;\
				}\
				return r;\
			}" > ${BEFORE} && g++ ${BEFORE} -o ${THEN})

startwatch = $(shell echo $(INSPIRE) && $(call inspire))
stopwatch = $(shell echo $(THEN) $(INSPIRE) $(EXPIRE))
#clean: $(shell rm -rf ${NOW} ${THEN} ${WHEN} ${BETWEEN})

