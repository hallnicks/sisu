HEADER := $(SISU)/makefiles.hpp

.PHONY: HEADER

HEADER:
	$(info hi!!!! )

$(BETWEEN): $(BIN_DIR)
                $(shell printf "#include <iostream>\n\
                        #include <sstream>\n\
                        int main( int xArgc, char * argv[] )\
                        {       int r = -1;\
                                if ( xArgc == 3 )\
                                {       double w, e;\
                                        std::stringstream(argv[1]) >> w;\
                                        std::stringstream(argv[2]) >> e;\
                                        std::cout << w - e << std::endl;\
                                        r = 0;\
                                }\
                                return r;\
                        }" > ${BEFORE} && g++ ${BEFORE} -o ${BETWEEN})

	
