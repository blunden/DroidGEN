
#ifndef COMMON_H_
#define COMMON_H_

#include <android/log.h>

#include <time.h>

#define NATIVE_OK 0x0
#define NATIVE_ERROR 0x1

#define MAX_PATH 1024

// LOGGING
//#define LOG_TAG "NESDroidNative"
#ifdef ANDROID
#  define QUOTEME_(x) #x
#  define QUOTEME(x) QUOTEME_(x)

#ifdef DEBUG_LOGGING
#  define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, " (" __FILE__ ":" QUOTEME(__LINE__) ") "__VA_ARGS__)
#else
#  define LOGD(...)
#endif
#  define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#  define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#else
#  define QUOTEME_(x) #x
#  define QUOTEME(x) QUOTEME_(x)
#  define LOGI(...) printf("I/" LOG_TAG " (" __FILE__ ":" QUOTEME(__LINE__) "): " __VA_ARGS__)
#  define LOGE(...) printf("E/" LOG_TAG "(" ")" __VA_ARGS__)
#endif

// VISIBLITY
#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif


/* return current time in milliseconds */
static double now_ms(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME_HR, &res);
    return 1000.0*res.tv_sec + (double)res.tv_nsec/1e6;
}


// thanks Maister!
static inline uint32_t next_pow2(uint32_t v)
{
   v--;
   v |= v >> 1;
   v |= v >> 2;
   v |= v >> 4;
   v |= v >> 8;
   v |= v >> 16;
   v++;
   return v;
}


#endif /* COMMON_H_ */
