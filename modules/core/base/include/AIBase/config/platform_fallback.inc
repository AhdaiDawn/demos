#ifdef __AI_PLATFORM_FALLBACK_INC__
    #error "platform_fallback.inc included twice"
#endif

//-------------------------------------------------------------------------------
// -> platform
//      AI_PLAT_WIN32
//      AI_PLAT_WIN64
//      AI_PLAT_WINDOWS
//      AI_PLAT_MACOSX
//      AI_PLAT_UNIX
//      AI_PLAT_IPHONE
//      AI_PLAT_IPHONE_SIMULATOR
//
// -> architecture
//      AI_ARCH_X86
//      AI_ARCH_X86_64
//      AI_ARCH_ARM32
//      AI_ARCH_ARM64
//      AI_ARCH_POWERPC32
//      AI_ARCH_POWERPC64
//      AI_ARCH_WA32
//      AI_ARCH_WA64
//      AI_ARCH_32BIT
//      AI_ARCH_64BIT
//      AI_ARCH_LITTLE_ENDIAN
//      AI_ARCH_BIG_ENDIAN
//
// -> SIMD
//      AI_ARCH_SSE
//      AI_ARCH_SSE2
//      AI_ARCH_AVX
//      AI_ARCH_AVX2
//-------------------------------------------------------------------------------

// platform
#ifndef AI_PLAT_WIN32
    #define AI_PLAT_WIN32 0
#endif
#ifndef AI_PLAT_WIN64
    #define AI_PLAT_WIN64 0
#endif
#ifndef AI_PLAT_MACOSX
    #define AI_PLAT_MACOSX 0
#endif
#ifndef AI_PLAT_UNIX
    #define AI_PLAT_UNIX 0
#endif
#ifndef AI_PLAT_IPHONE
    #define AI_PLAT_IPHONE 0
#endif
#ifndef AI_PLAT_IPHONE_SIMULATOR
    #define AI_PLAT_IPHONE_SIMULATOR 0
#endif

// architecture
#ifndef AI_ARCH_X86
    #define AI_ARCH_X86 0
#endif
#ifndef AI_ARCH_X86_64
    #define AI_ARCH_X86_64 0
#endif
#ifndef AI_ARCH_ARM32
    #define AI_ARCH_ARM32 0
#endif
#ifndef AI_ARCH_ARM64
    #define AI_ARCH_ARM64 0
#endif
#ifndef AI_ARCH_POWERPC32
    #define AI_ARCH_POWERPC32 0
#endif
#ifndef AI_ARCH_POWERPC64
    #define AI_ARCH_POWERPC64 0
#endif
#ifndef AI_ARCH_WA32
    #define AI_ARCH_WA32 0
#endif
#ifndef AI_ARCH_WA64
    #define AI_ARCH_WA64 0
#endif
#ifndef AI_ARCH_32BIT
    #define AI_ARCH_32BIT 0
#endif
#ifndef AI_ARCH_64BIT
    #define AI_ARCH_64BIT 0
#endif
#ifndef AI_ARCH_LITTLE_ENDIAN
    #define AI_ARCH_LITTLE_ENDIAN 0
#endif
#ifndef AI_ARCH_BIG_ENDIAN
    #define AI_ARCH_BIG_ENDIAN 0
#endif

// SIMD
#ifndef AI_ARCH_SSE
    #define AI_ARCH_SSE 0
#endif
#ifndef AI_ARCH_SSE2
    #define AI_ARCH_SSE2 0
#endif
#ifndef AI_ARCH_AVX
    #define AI_ARCH_AVX 0
#endif
#ifndef AI_ARCH_AVX2
    #define AI_ARCH_AVX2 0
#endif

#define __AI_PLATFORM_FALLBACK_INC__