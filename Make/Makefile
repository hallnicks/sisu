#Deficiencies: YOu have to do a make clean to rebuild binaries. Make links shared and creates static.

SISU := recipe/

include $(SISU)/before.mk

#TODO: Fix D_TEXTREL error in shared library caused bny -fpic (?)
#LINKMODES    := static shared exe
LINKMODES    := static exe

#Arbitrary number of "main" entry points
#Need to optimize it such that test library links only to test
#and runtime to runtime vice versa etc etc
TARGET_NAME  := sisu sisuTest client server

TARGET_LIBS  += pthread # rt # for when renabled test

TARGET_LIBS  += ssl crypto

TARGET_LIBS  += png z freetype

#This is now windows specific. Need to test on linux again. For example, winsock, opengl32, glew32, gdi, etc, linked differently.
ifeq ($(OS),Windows_NT)
    TARGET_LIBS  += opengl32 freeglut glew32 Dbghelp ws2_32 Shlwapi gdi32
else
    TARGET_LIBS += GL GLEW GLU
endif

TARGET_LIBS += tbb
#TARGET_LIBS += ncurses

#ffmpeg
ifeq ($(OS),Windows_NT)
    TARGET_LIBS += iconv log
endif

TARGET_LIBS += avformat avcodec avdevice avfilter avutil swscale x264 swresample m z

#opencv
ifeq ($(OS),Windows_NT)
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
 opencv_world310
# opencv_videostab310
else
TARGET_LIBS += \
 opencv_core\
 opencv_calib3d\
 opencv_features2d\
 opencv_flann\
 opencv_highgui\
 opencv_imgcodecs\
 opencv_imgproc\
 opencv_ml\
 opencv_objdetect\
 opencv_photo\
 opencv_shape\
 opencv_stitching\
 opencv_superres\
 opencv_video\
 opencv_videoio
# opencv_videostab
endif

# comment the line below to disable OpenCV 
CXXFLAGS += -D SISU_OPENCV

#Extra's for fun (TODO: integrate)
#TARGET_LIBS += curl twitcurl utp x264

# OpenAL
ifeq ($(OS),Windows_NT)
    TARGET_LIBS += OpenAL32
else
    TARGET_LIBS += openal
endif
# Comment the line below to disable OpenAL
CXXFLAGS += -D SISU_OPENAL

# for SDL
SDL_CONFIG   := sdl2-config --prefix=/usr
CXXFLAGS     += $(shell $(SDL_CONFIG) --cflags)
LDFLAGS      += $(shell $(SDL_CONFIG) --libs)

# for freetype
FREETYPE_CONFIG := freetype-config
CXXFLAGS     	+= $(shell $(FREETYPE_CONFIG) --cflags)
LDFLAGS      	+= $(shell $(FREETYPE_CONFIG) --libs)

##for freeglut (Again, note windows dependency.)
ifeq ($(OS),Windows_NT)
    FREEGLUT_FLAGS := -Wl,--subsystem,windows
    CXXFLAGS 	   += $(FREEGLUT_FLAGS)
    LDFLAGS	   += $(FREEGLUT_FLAGS)
endif

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

#For OpenGLES... TODO: Make conditional or as config option.
# For now, comment out if you are not running OpenGLES build
#CXXFLAGS += -D OPENGLES

include $(SISU)/after.mk
