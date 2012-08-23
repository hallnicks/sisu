BEFORE  	:= $(shell tempfile).cpp
NOW   		:= $(abspath ${CURDIR}/now)
THEN   		:= $(abspath ${CURDIR}/then)
TOMORROW 	:= $(THEN) $(MOMENT1) $(MOMENT2)
INSPIRE 	?= $(NOW)
EXPIRE 		?= $(THEN)

then = $(call EXPIRE) $(info ${THEN})
now = $(call INSPIRE) $(info ${NOW})

.PHONY: $(NOW) $(THEN) TESTWATCH

$(NOW):	$(call INSPIRE)\
	 $(shell printf "#include <iostream>\n\
			 #include <ctime>\n\
			 int main( int xArgc, char * argv[] ) {\
				timespec ts;\
				ts.tv_nsec = 0;\
				ts.tv_nsec = 0;\
				clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &ts );\
				std::cout << static_cast<double>(ts.tv_sec * 1.0e100) +\
						 static_cast<double>(ts.tv_nsec) << std::endl;\
				return 0;\
			 }" > ${BEFORE} && g++ ${BEFORE} -o ${NOW} -lrt) $(call now)

$(THEN): $(info THEN)\
	$(shell printf "#include <iostream>\n\
		  	#include <sstream>\n\
                        int main( int xArgc, char * argv[] ) {\
				int r = -1;\
				if ( xArgc == 3 ) {\
					double o, y;\
					std::stringstream(argv[1]) >> o;\
					std::stringstream(argv[2]) >> y;\
					std::cout << o << y << std::endl;\
					r = 1;\
				}\
				return r;\
			}" > ${BEFORE} && g++ ${BEFORE} -o ${THEN} -lrt) $(info THEN DONE)

startwatch = $(shell echo $(INSPIRE) && $(call inspire))
stopwatch = $(shell echo $(THEN) $(INSPIRE) $(EXPIRE))

clean:	$(shell rm -rf ${NOW} ${THEN})
