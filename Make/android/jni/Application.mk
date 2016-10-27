# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
APP_STL := gnustl_shared

#APP_ABI := armeabi armeabi-v7a x86
APP_ABI := armeabi armeabi-v7a

# Min SDK level
APP_PLATFORM=android-10

APP_CPPFLAGS += -std=c++11 -fexceptions -frtti
