#pragma once
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

// windows platform
#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
    #ifdef _WIN64
        #define AI_PLAT_WIN64 1
    #else
        #define AI_PLAT_WIN32 1
    #endif
#endif

// ios platform
#if defined(__APPLE__) && defined(__MACH__)
    /* Apple OSX and iOS (Darwin). */
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
    /* iOS in Xcode simulator */
        #define AI_PLAT_IPHONE_SIMULATOR 1
    #elif TARGET_OS_IPHONE == 1
    /* iOS */
        #define AI_PLAT_IPHONE 1
    #elif TARGET_OS_MAC == 1
        /* macOS */
        #define AI_PLAT_MACOSX 1
    #endif
#endif

#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    #define AI_PLAT_UNIX 1
#endif

// architecture
#ifndef AI_MANUAL_CONFIG_CPU_ARCHITECTURE
    #if defined(__x86_64__) || defined(_M_X64) || defined(_AMD64_) || defined(_M_AMD64)
        #define AI_ARCH_X86_64 1
        #define AI_ARCH_64BIT 1
        #define AI_ARCH_LITTLE_ENDIAN 1
        #define AI_ARCH_SSE 1
        #define AI_ARCH_SSE2 1
    #elif defined(__i386) || defined(_M_IX86) || defined(_X86_)
        #define AI_ARCH_X86 1
        #define AI_ARCH_32BIT 1
        #define AI_ARCH_LITTLE_ENDIAN 1
        #define AI_ARCH_SSE 1
        #define AI_ARCH_SSE2 1
    #elif defined(__aarch64__) || defined(__AARCH64) || defined(_M_ARM64)
        #define AI_ARCH_ARM64 1
        #define AI_ARCH_64BIT 1
        #define AI_ARCH_LITTLE_ENDIAN 1
        #define AI_ARCH_SSE 1
        #define AI_ARCH_SSE2 1
    #elif defined(__arm__) || defined(_M_ARM)
        #define AI_ARCH_ARM32 1
        #define AI_PLATFORM_32BIT 1
        #define AI_ARCH_LITTLE_ENDIAN 1
    #elif defined(__POWERPC64__) || defined(__powerpc64__)
        #define AI_ARCH_POWERPC64 1
        #define AI_ARCH_64BIT 1
        #define AI_ARCH_BIG_ENDIAN 1
    #elif defined(__POWERPC__) || defined(__powerpc__)
        #define AI_ARCH_POWERPC32 1
        #define AI_PLATFORM_32BIT 1
        #define AI_ARCH_BIG_ENDIAN 1
    #elif defined(__wasm64__)
        #define AI_ARCH_WA64 1
        #define AI_ARCH_64BIT 1
        #define AI_ARCH_LITTLE_ENDIAN 1
    #elif defined(__wasm__) || defined(__EMSCRIPTEN__) || defined(__wasi__)
        #define AI_ARCH_WA32 1
        #define AI_PLATFORM_32BIT 1
        #define AI_ARCH_LITTLE_ENDIAN 1
    #else
        #error Unrecognized CPU was used.
    #endif
#endif

// SIMD
#if defined(__AVX__)
    #define AI_ARCH_AVX 1
#endif
#if defined(__AVX2__)
    #define AI_ARCH_AVX2 1
#endif

// fallback
#include "platform_fallback.inc"

// other platform def
#define AI_PLAT_WINDOWS AI_PLAT_WIN32 || AI_PLAT_WIN64
#define AI_ARCH_WA AI_ARCH_WA32 || AI_ARCH_WA64

// Platform Specific Configure
#define AI_HEADER_SCOPE_DEFINING_PLATFORM_CONFIGURE

#ifdef __APPLE__
    #include "apple/configure.inc"
#endif

#ifdef _WIN32
    #define AI_RUNTIME_USE_MIMALLOC
    #define OS_DPI 96
#endif

#ifndef OS_DPI
    #define OS_DPI 72
#endif

#undef AI_HEADER_SCOPE_DEFINING_PLATFORM_CONFIGURE
