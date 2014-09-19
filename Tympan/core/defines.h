#ifndef TY_C_DEFINES
#define TY_C_DEFINES

/* From this threshold (in meters), two elements are considered as at the same
 * position
 * */
#define TYSEUILCONFONDUS        0.001        // 1 mm

/* Initial platform/compiler-related stuff to set.
 */
#define TY_PLATFORM_WIN32 1
#define TY_PLATFORM_WIN64 2
#define TY_PLATFORM_LINUX 3

#define TY_COMPILER_MSVC 1
#define TY_COMPILER_GNUC 2

#define TY_ARCHITECTURE_32 1
#define TY_ARCHITECTURE_64 2

/* Finds the compiler type and version.
 */
#if defined( _MSC_VER )
    #   define TY_COMPILER TY_COMPILER_MSVC
    #   define TY_COMP_VER _MSC_VER

    #elif defined( __GNUC__ )
    #   define TY_COMPILER TY_COMPILER_GNUC
    #   define TY_COMP_VER (((__GNUC__)*100) + \
            (__GNUC_MINOR__*10) + \
            __GNUC_PATCHLEVEL__)

    #else
    #   pragma error "No known compiler. Abort! Abort!"

    #endif

    /* Finds the current platform */

    #if defined( __WIN32__ ) || defined( _WIN32 )
    #   define TY_PLATFORM TY_PLATFORM_WIN32

    #elif defined( _WIN64_ ) || defined( _WIN64 )
    #   define TY_PLATFORM TY_PLATFORM_WIN64

    #else
    #   define TY_PLATFORM TY_PLATFORM_LINUX
    #endif

    /* Find the arch type */
    #if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
    #   define TY_ARCH_TYPE TY_ARCHITECTURE_64
    #else
    #   define TY_ARCH_TYPE TY_ARCHITECTURE_32
    #endif

    // Integer formats of fixed bit width
    typedef unsigned char uint8;
    typedef unsigned short uint16;
    typedef unsigned int uint32;
    // define uint64 type
    #if TY_COMPILER == TY_COMPILER_MSVC
    typedef unsigned __int64 uint64;
    #else
    typedef unsigned long long uint64;
    #endif


#endif // TY_C_DEFINES
