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

#for pthread (winpthread as well)
TARGET_LIBS  += pthread

#for openssl
TARGET_LIBS  += ssl crypto

#for PNG
TARGET_LIBS  += png z freetype

#For Assimp
TARGET_LIBS  += assimp

#This is now windows specific. Need to test on linux again. For example, winsock, opengl32, glew32, gdi, etc, linked differently.
ifeq ($(OS),Windows_NT)
# If using OpenGL, use this line
#    TARGET_LIBS  += opengl32 freeglut glew32
    TARGET_LIBS  += Dbghelp ws2_32 Shlwapi gdi32
else
#    TARGET_LIBS += GL GLEW GLU
endif

# If Using OpenGLES, use this line. DL is for OpenGLES extensions.
# Technically, since we are using SDL for the context, EGL may not be necessary
TARGET_LIBS  += GLESv2 EGL dl

UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
   TARGET_LIBS += X11
endif

TARGET_LIBS += tbb
#TARGET_LIBS += ncurses

#ffmpeg
TARGET_LIBS += avformat avcodec avdevice avfilter avutil swscale x264 swresample m z

ifeq ($(OS),Windows_NT)
    TARGET_LIBS += iconv log
endif

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
    CXXFLAGS += -D SISU_OPENAL
else
    TARGET_LIBS += openal
    #TODO: Integrate OpenAL on linux
    #CXXFLAGS += -D SISU_OPENAL
endif
# Comment the line below to disable OpenAL

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
CXXFLAGS     += -I/usr/local/include -Wno-pragmas

# for libPNG, libavocdec, libassimp
LDFLAGS      += -L/usr/local/lib

# unit tests
#CXXFLAGS += -D BEEP_UT
#CXXFLAGS += -D GAMECONTROLLER_UT
#CXXFLAGS += -D KEYBOARD_UT
#CXXFLAGS += -D LIBAVCODEC_UT
#CXXFLAGS += -D LIBFREETYPE_UT
#CXXFLAGS += -D LIBOPENSSL_UT
#CXXFLAGS += -D LIBPNG_UT
CXXFLAGS  += -D LOG_UT
#CXXFLAGS += -D MEMBLOCK_UT
#CXXFLAGS += -D MOUSE_UT
#CXXFLAGS += -D OPENAL_UT
#CXXFLAGS += -D OPENCV_UT
#CXXFLAGS += -D OPENGLES_HELLOTRIANGLE_UT
#CXXFLAGS  += -D OPENGLES_HELLOTEXTURE_UT
CXXFLAGS  += -D OPENGLES_HELLOINSTANCING_UT
#CXXFLAGS += -D PNG_IMAGE_STREAMER_UT
#CXXFLAGS += -D POPEN_UT
#CXXFLAGS += -D SDL_AUDIO_MIXER_UT
#CXXFLAGS += -D SDL_COLOR_SWITCHER_UT
#CXXFLAGS += -D SDL_FBO_UT
#CXXFLAGS += -D SDL_MULTITHREADED_AUDIO_UT
#CXXFLAGS += -D SDL_QUAD_UT
#CXXFLAGS += -D SDL_RANDOMIZED_AUDIO_UT
#CXXFLAGS += -D SDL_SIMPLE_WAVE_MIXER_UT
#CXXFLAGS += -D SDL_SPRITE_UT
#CXXFLAGS += -D SDL_VIEWPORT_UT
#CXXFLAGS += -D SIGNAL_UT
#CXXFLAGS += -D SINGLETON_UT
#CXXFLAGS += -D SISU_UT
#CXXFLAGS += -D STOPWATCH_UT
#CXXFLAGS += -D THREADGEARS_UT
#CXXFLAGS += -D THREAD_SAFE_QUEUE_UT
#CXXFLAGS += -D TTYC_UT
#CXXFLAGS += -D TTYCOLOR_UT
#CXXFLAGS += -D WORD_UT
#CXXFLAGS += ..


SOURCE_DIRS   := ../Source ../Tests

#For OpenGLES... TODO: Make conditional or as config option.
# For now, comment out if you are not running OpenGLES build
CXXFLAGS += -D OPENGLES

include $(SISU)/after.mk
