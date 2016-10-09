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

#This is now windows specific. Need to test on linux again. For example, winsock, opengl32, glew32, gdi, etc, linked differently.
TARGET_LIBS  += opengl32 freeglut glew32 Dbghelp ws2_32 Shlwapi gdi32

TARGET_LIBS += tbb

#TARGET_LIBS += ncurses

#ffmpeg
TARGET_LIBS += avformat avcodec avdevice avfilter avutil swscale x264 swresample log m z iconv

#opencv
TARGET_LIBS += \
 opencv_core310\
 opencv_calib3d310\
 opencv_features2d310\
 opencv_flann310\
 opencv_highgui310\
 opencv_imgcodecs310\
 opencv_imgproc310\
 opencv_ml310\
 opencv_objdetect310\
 opencv_photo310\
 opencv_shape310\
 opencv_stitching310\
 opencv_superres310\
 opencv_video310\
 opencv_videoio310\
 opencv_videostab310\
 opencv_world310


#Extra's for fun (TODO: integrate)
#TARGET_LIBS += curl twitcurl utp x264

# OpenAL
TARGET_LIBS += OpenAL32

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

#for FFMPEG :( TODO: Use non-deprecated approachj
CXXFLAGS += -Wno-deprecated-declarations 

#for OpenAL:( TODO: Fix bugs in OpenAL example code
CXXFLAGS += -Wno-pointer-arith -Wno-attributes

#for Redbook 
CXXFLAGS += -Wno-int-to-pointer-cast

# for GLEW
CXXFLAGS     += -I/usr/include

# for GLM, ssl, libavcodec
CXXFLAGS     += -I/usr/local/include

# for libPNG, libavocdec
LDFLAGS      += -L/usr/local/lib

SOURCE_DIRS   := ../Source ../Tests

include $(SISU)/after.mk
