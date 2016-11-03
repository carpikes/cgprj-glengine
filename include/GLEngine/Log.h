#ifndef GLENGINE_LOG_H
#define GLENGINE_LOG_H

#include <cstdio>
#include <cstdarg>
#include "Singleton.h"

namespace GLEngine {

class Log : public Singleton<Log>{
    friend class Singleton;
public:
    void print(const char *tag, const char *str, ...) {
        if(!tag || !str) return;

        printf("[%s] ", tag);
        va_list a;
        va_start(a, str);
        vprintf(str, a);
        va_end(a);
        printf("\n");
    }
private:
    Log() { }
};
    
} /* GLEngine */ 

#define sLog            GLEngine::Log::instance()
#define LOG(x)          sLog.print(__TAG(), x)
#define LOGP(x, ...)    sLog.print(__TAG(), x, __VA_ARGS__)
#define ERR(x)          LOG(x)
#define ERRP(x, ...)    LOGP(x, __VA_ARGS__)
//#define DEB(x)          LOG(x)
//#define DEBP(x, ...)    LOGP(x, __VA_ARGS__)
#define DEB(x)
#define DEBP(x, ...)
#define TAG_DEF(x)      static const char * __TAG() { return x; }

#endif /* ifndef GLENGINE_LOG_H */
