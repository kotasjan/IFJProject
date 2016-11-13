#ifndef DEBUG_H
#define DEBUG_H

#include "scaner.h"     /*  Token  */

#ifndef DEBUG
#define debug(M, ...);
#define debugFunc(M, ...);
#define debugVar(M, ...);
#define debugClass(M, ...);

#else

#define NRM     "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\033[32;1m"
#define BLUE    "\033[34m" 
#define YELLOW  "\033[33m"  
#define MAGENTA "\033[35m"

#define debug(M, ...) \
    do { fprintf(stderr, "DEBUG %s%s%s:%d:<%s>: " M "", RED, __FILE__, NRM, __LINE__, __func__, ##__VA_ARGS__); } while(0)

#define debugFunc(M, ...) \
    do { fprintf(stderr, "    DEBUG %s%s%s:%d:<%s>: " M "", GREEN , __FILE__, NRM, __LINE__, __func__, ##__VA_ARGS__); } while(0)

#define debugVar(M, ...) \
    do { fprintf(stderr, "    DEBUG %s%s%s:%d:<%s>: " M "", MAGENTA , __FILE__, NRM, __LINE__, __func__, ##__VA_ARGS__); } while(0)

#define debugClass(M, ...) \
    do { fprintf(stderr, "DEBUG %s%s%s:%d:<%s>: " M "", YELLOW , __FILE__, NRM, __LINE__, __func__, ##__VA_ARGS__); } while(0)

#endif
char *printTok(tToken *token);

#endif
