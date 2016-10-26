LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH  := ../SDL
SISU_PATH := ../../../../Source
MAIN_PATH := ./appMain

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include

LOCAL_CFLAGS += -O2 -D OPENGLES

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	  	   $(SISU_PATH)/threadgears.cpp 		   \
	  	   $(SISU_PATH)/SDLTest.cpp 		           \
		   $(MAIN_PATH)/main_sisu.cpp

LOCAL_SHARED_LIBRARIES := SDL2

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)

