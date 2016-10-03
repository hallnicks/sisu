#Deficiencies: YOu have to do a make clean to rebuild binaries. Make links shared and creates static.

SISU := recipe/

include $(SISU)/before.mk

LINKMODES    := static shared exe

#Arbitrary number of "main" entry points 
#Need to optimize it such that test library links only to test 
#and runtime to runtime vice versa etc etc
#TARGET_NAME  := sisu sisuTest client server
TARGET_NAME := sisuTest client

TARGET_LIBS  += pthread # rt # for when renabled test

TARGET_LIBS  += ssl crypto

TARGET_LIBS  += png z freetype

#This is now windows specific. Need to test on linux again. For example, winsock, opengl32, glew32, etc, linked differently.
TARGET_LIBS  += opengl32 freeglut glew32 Dbghelp ws2_32 Shlwapi

TARGET_LIBS += tbb

#TARGET_LIBS += ncurses

#ffmpeg
TARGET_LIBS += avformat avcodec avdevice avfilter avutil swscale x264 swresample log m z iconv

#Extra's for fun (TODO: integrate)
#TARGET_LIBS += curl twitcurl utp x264

# for SDL
SDL_CONFIG   := sdl2-config --prefix=/usr
CXXFLAGS     += $(shell $(SDL_CONFIG) --cflags)
LDFLAGS      += $(shell $(SDL_CONFIG) --libs)

# for freetype
FREETYPE_CONFIG := freetype-config
CXXFLAGS     	+= $(shell $(FREETYPE_CONFIG) --cflags)
LDFLAGS      	+= $(shell $(FREETYPE_CONFIG) --libs)

#for freeglut (Again, note windows dependency.)
FREEGLUT_FLAGS  := -Wl,--subsystem,windows
CXXFLAGS 	+= $(FREEGLUT_FLAGS)
LDFLAGS		+= $(FREEGLUT_FLAGS)

#Turn on max warnings (TODO: Fix these.) 
#CXXFLAGS += -Wall -Wextra
#LDFLAGS  += -Wall -Wextra
#CXXFLAGS += -Werror -O3
CXXFLAGS += -Wno-deprecated-declarations


# for GLEW
CXXFLAGS     += -I/usr/include

# for GLM, ssl, libavcodec
CXXFLAGS     += -I/usr/local/include

# for libPNG, libavocdec
LDFLAGS      += -L/usr/local/lib

SOURCE_DIRS   := ../Source ../Tests

include $(SISU)/after.mk
