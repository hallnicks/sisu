LOCAL_PATH := $(call my-dir)

# libpng
include $(CLEAR_VARS)

LOCAL_MODULE 	  := libpng
EXTERNAL_LIB_PATH := ../../../../lib/android
LOCAL_SRC_FILES   := $(EXTERNAL_LIB_PATH)/libpng.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

# libfreetype
include $(CLEAR_VARS)

LOCAL_MODULE 	  := libfreetype
EXTERNAL_LIB_PATH := ../../../../lib/android
LOCAL_SRC_FILES   := $(EXTERNAL_LIB_PATH)/libfreetype.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

# libassimp
include $(CLEAR_VARS)

LOCAL_MODULE 	  := libassimp
EXTERNAL_LIB_PATH := ../../../../lib/android
LOCAL_SRC_FILES   := $(EXTERNAL_LIB_PATH)/libassimp.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

# libzip
include $(CLEAR_VARS)

LOCAL_MODULE 	  := libzip
EXTERNAL_LIB_PATH := ../../../../lib/android
LOCAL_SRC_FILES   := $(EXTERNAL_LIB_PATH)/libzip.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

# libunzip
include $(CLEAR_VARS)

LOCAL_MODULE 	  := libunzip
EXTERNAL_LIB_PATH := ../../../../lib/android
LOCAL_SRC_FILES   := $(EXTERNAL_LIB_PATH)/libunzip.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

# libILU
include $(CLEAR_VARS)

LOCAL_MODULE 	  := libILU
EXTERNAL_LIB_PATH := ../../../../lib/android
LOCAL_SRC_FILES   := $(EXTERNAL_LIB_PATH)/libILU.so

include $(PREBUILT_SHARED_LIBRARY)

# libILUT
include $(CLEAR_VARS)

LOCAL_MODULE 	  := libILUT
EXTERNAL_LIB_PATH := ../../../../lib/android
LOCAL_SRC_FILES   := $(EXTERNAL_LIB_PATH)/libILUT.so

include $(PREBUILT_SHARED_LIBRARY)

# libDevIL
include $(CLEAR_VARS)

LOCAL_MODULE 	  := libDevIL
EXTERNAL_LIB_PATH := ../../../../lib/android
LOCAL_SRC_FILES   := $(EXTERNAL_LIB_PATH)/libDevIL.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

# main sisu module
LOCAL_MODULE := main

SDL_PATH  	      := ../SDL
SISU_PATH 	      := ../../../../Source
EXTERNAL_INCLUDE_PATH := ../../../../include
MAIN_PATH 	      := ./appMain

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(SISU_PATH) $(EXTERNAL_INCLUDE_PATH)

LOCAL_CFLAGS += -O2 -D OPENGLES

#Core source code
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
		   $(SISU_PATH)/DevILInit.cpp			   \
	  	   $(SISU_PATH)/PNGImage.cpp 		   	   \
		   $(SISU_PATH)/SDLColorSwitcher.cpp		   \
	  	   $(SISU_PATH)/SDLQuadShader.cpp 		   \
	  	   $(SISU_PATH)/SDLShader.cpp 		           \
	  	   $(SISU_PATH)/SDLTest.cpp 		           \
	  	   $(SISU_PATH)/SDLTestShaderWindow.cpp 	   \
	  	   $(SISU_PATH)/beep.cpp 	   		   \
	  	   $(SISU_PATH)/memblock.cpp 	   		   \
	  	   $(SISU_PATH)/Mouse.cpp 	   		   \
	  	   $(SISU_PATH)/signal.cpp 	   		   \
	  	   $(SISU_PATH)/threadgears.cpp 		   \
		   $(MAIN_PATH)/main_sisu.cpp


LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_STATIC_LIBRARIES := png zip ILU ILUT DeVIL freetype assimp unzip

#LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -lz
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lGLESv3 -llog -lz

include $(BUILD_SHARED_LIBRARY)

