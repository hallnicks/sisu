#ifndef ANDROID_LOG_WRAPPER_868F6EB48A114F4BBE635918668BDAE2_HPP_
#define ANDROID_LOG_WRAPPER_868F6EB48A114F4BBE635918668BDAE2_HPP_

#ifdef ANDROID
#include <android/log.h>

#define SISULOG(xMsg) __android_log_print( ANDROID_LOG_VERBOSE \
                                         , "sisu" \
                                         , "%s" \
                                         , xMsg );

#else
#include <iostream>

#define SISULOG(xMsg) std::cerr << xMsg << std::endl;

#endif

#endif // ANDROID_LOG_WRAPPER_868F6EB48A114F4BBE635918668BDAE2_HPP_
