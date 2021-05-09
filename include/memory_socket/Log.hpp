//
//  Log.hpp
//  RealTimeThreadCommunicator
//
//  Created by Matthew Good on 24/3/21.
//

#ifndef Log_hpp
#define Log_hpp

#include <mutex>
#include <memory.h>

extern std::mutex lock;

int LOG_INFO(const char* format);
int LOG_ERROR(const char* format);
void LOG_ALWAYS_FATAL(const char* format);

int LOG_INFO_TAG(const char * tag, const char* format);
int LOG_ERROR_TAG(const char * tag, const char* format);
void LOG_ALWAYS_FATAL_TAG(const char * tag, const char* format);

template <typename ... Args>
int LOG_INFO(const char* format, Args ... args) {
    lock.lock();
    int len = fprintf(stdout, format, args...);
    len += fprintf(stdout, "\n");
    fflush_unlocked(stdout);
    lock.unlock();
    return len;
}

template <typename ... Args>
int LOG_ERROR(const char* format, Args ... args) {
    lock.lock();
    int len = fprintf(stderr, format, args...);
    len += fprintf(stdout, "\n");
    fflush_unlocked(stderr);
    lock.unlock();
    return len;
}

template <typename ... Args>
int LOG_ALWAYS_FATAL(const char* format, Args ... args) {
    lock.lock();
    fprintf(stderr, format, args...);
    fprintf(stdout, "\n");
    fflush_unlocked(stderr);
    lock.unlock();
    abort();
}

template <typename ... Args>
int LOG_INFO_TAG(const char * tag, const char* format, Args ... args) {
    lock.lock();
    int len = fprintf(stdout, "%s: ", tag);
    len += fprintf(stdout, format, args...);
    len += fprintf(stdout, "\n");
    fflush_unlocked(stdout);
    lock.unlock();
    return len;
}

template <typename ... Args>
int LOG_ERROR_TAG(const char * tag, const char* format, Args ... args) {
    lock.lock();
    int len = fprintf(stderr, "%s: ", tag);
    len += fprintf(stderr, format, args...);
    len += fprintf(stderr, "\n");
    fflush_unlocked(stderr);
    lock.unlock();
    return len;
}

template <typename ... Args>
int LOG_ALWAYS_FATAL_TAG(const char * tag, const char* format, Args ... args) {
    lock.lock();
    fprintf(stderr, "%s: ", tag);
    fprintf(stderr, format, args...);
    fprintf(stderr, "\n");
    fflush_unlocked(stderr);
    lock.unlock();
    abort();
}

#endif /* Log_hpp */
