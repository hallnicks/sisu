SRC_DIR		:= $(abspath ${CURDIR}/src)

GLM_BUILD 	:= ${SRC_DIR}/glm-0.9.3.4
OPENFST_BUILD	:= ${SRC_DIR}/openfst-1.3.2
OPENGRM_BUILD   := ${SRC_DIR}/opengrm-ngram-1.0.3
SDL_BUILD 	:= ${SRC_DIR}/SDL/build
TWITCURL_BUILD	:= ${SRC_DIR}/twitcurl-read-only/libtwitcurl

PREFIX 	  	?= $(abspath ${CURDIR}/x86_64)
LIB_DIR		:= ${PREFIX}/lib
INC_DIR		:= ${PREFIX}/include

GLM_LIB	  	:= ${INC_DIR}/glm
OPENFST_LIB	:= ${LIB_DIR}/libfst.la
SDL_LIB   	:= ${LIB_DIR}/libSDL2.a
TWITCURL_LIB 	:= ${LIB_DIR}/libtwitcurl.a
OPENGRM_LIB	:= ${LIB_DIR}/libngram.a

$(GLM_LIB):
	mkdir -p $(@D)
	cp -R ${GLM_BUILD}/glm $@

$(OPENFST_LIB):
	mkdir -p $(OPENFST_BUILD)/build
	cd $(OPENFST_BUILD)/build && \
	$(OPENFST_BUILD)/configure --enable-static --enable-compact-fsts --enable-const-fsts --enable-far \
			--enable-ngram-fsts --enable-lookahead-fsts --enable-pdt \
			--prefix=${PREFIX} && \
	make && \
	make install

$(OPENGRM_LIB): $(OPENFST_LIB)
	mkdir -p $(OPENGRM_BUILD)/build
	cd $(OPENGRM_BUILD)/build && \
	LDFLAGS="-L${PREFIX}/lib -L${PREFIX}/lib/fst" \
	CPPFLAGS="-I${PREFIX}/include" \
	$(OPENGRM_BUILD)/configure --enable-static --prefix=${PREFIX} && \
	make && \
	make install

$(SDL_LIB):
	mkdir -p ${SDL_BUILD}
	cd ${SDL_BUILD} && \
	../configure --prefix=${PREFIX} && \
	make && \
	make install

$(TWITCURL_LIB):
	cd $(TWITCURL_BUILD) && \
	make && \
	cp libtwitcurl.a $@

.PHONY: all sdl glm openfst opengrm sdl twiturl 

all: glm openfst opengrm sdl twitcurl
glm: $(GLM_LIB)
openfst: ${OPENFST_LIB}
opengrm: $(OPENGRM_LIB)
sdl: $(SDL_LIB)
twitcurl: $(TWITCURL_LIB)

clean:
	rm -rf ${PREFIX}
