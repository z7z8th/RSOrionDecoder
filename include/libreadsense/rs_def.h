#ifndef RS_DEF_H_
#define RS_DEF_H_

typedef char                RSint8;
typedef short               RSint16;
typedef int                 RSint32;
typedef long long           RSint64;

typedef unsigned char       RSuint8;
typedef unsigned short      RSuint16;
typedef unsigned int        RSuint32;
typedef unsigned long long  RSuint64;

typedef float               RSfloat32;
typedef double              RSfloat64;

typedef void*               RShandle;
typedef void*               handle; // reserved for compatibility

#ifndef MIN
#define MIN(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a, b) ((a)>(b)?(a):(b))
#endif

#ifndef ABS
#define ABS(n) ((n)<0?-(n):(n))
#endif

#ifdef __cplusplus

#define BEGIN_NAMESPACE(ns) namespace ns {
#define END_NAMESPACE(ns)   } // ns

#define EXTERN_C            extern "C"
#define BEGIN_EXTERN_C      extern "C" {
#define END_EXTERN_C        } // extern "C"

#define ENUM(e)     enum e
#define STRUCT(s)   struct s
#define DEFAULT(x)  = x

#else

#define BEGIN_NAMESPACE(ns)
#define END_NAMESPACE(ns)

#define EXTERN_C
#define BEGIN_EXTERN_C
#define END_EXTERN_C

#define ENUM(e)\
typedef enum e e;\
enum e

#define STRUCT(s)\
typedef struct s s;\
struct s

#define DEFAULT(x)

#endif // __cplusplus

#ifdef _MSC_VER
#else
#endif

#ifdef _WIN32
    #define WIN32_LEAN_ADN_MEAN
    #include <WinSock2.h>
    #include <Windows.h>

    #ifdef USE_STATIC_LIB
        #define RS_API
    #else
        #define RS_API  __declspec(dllexport)
    #endif // USE_STATIC_LIB

    //#define CALLSPEC    __stdcall
    #define CALLSPEC
#else
    #define RS_API __attribute__((visibility("default")))
    #define CALLSPEC
#endif // _WIN32

#ifdef __GNUC__
    #define ALIGN(n) __attribute__((align(n)))
#else
    #define ALIGN(n)
#endif


#define BEGIN_NAMESPACE_RS  BEGIN_NAMESPACE(rs)
#define END_NAMESPACE_RS    END_NAMESPACE(rs)

#define RESERVED_FIELD(n)   int reserved[n];

#define TINY_STRING_LEN     16
#define SMALL_STRING_LEN    32
#define STRING_LEN          64
#define BIG_STRING_LEN      256
#define HUGE_STRING_LEN     512

#define UUID_LEN            68  // 68 = 64 + 4, ALIGN(4)
#define PATH_LEN            256

#endif // RS_DEF_H_

