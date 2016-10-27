LOCAL_PATH := $(call my-dir)

# libpng

include $(CLEAR_VARS)

LOCAL_MODULE 	  := libpng
EXTERNAL_LIB_PATH := ../../../../lib/android
LOCAL_SRC_FILES   := $(EXTERNAL_LIB_PATH)/libpng.a

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
	  	   $(SISU_PATH)/PNGImage.cpp 		   	   \
	  	   $(SISU_PATH)/SDLQuadShader.cpp 		   \
	  	   $(SISU_PATH)/SDLShader.cpp 		           \
	  	   $(SISU_PATH)/SDLTest.cpp 		           \
	  	   $(SISU_PATH)/SDLTestShaderWindow.cpp 	   \
	  	   $(SISU_PATH)/beep.cpp 	   		   \
	  	   $(SISU_PATH)/mouse.cpp 	   		   \
	  	   $(SISU_PATH)/signal.cpp 	   		   \
	  	   $(SISU_PATH)/threadgears.cpp 		   \
		   $(MAIN_PATH)/main_sisu.cpp


LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_STATIC_LIBRARIES := png

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -lz

include $(BUILD_SHARED_LIBRARY)

