#pragma once
//-------------------------------------------------------------------------------
// -> compiler
//      AI_COMPILER_GCC
//      AI_COMPILER_MSVC
//      AI_COMPILER_CLANG
//      AI_COMPILER_CLANG_CL
//
// -> cxx version
//      AI_CXX_11
//      AI_CXX_14
//      AI_CXX_17
//      AI_CXX_20
//      AI_CXX_VERSION
//
// -> other
//      AI_COMPILER_VERSION
//-------------------------------------------------------------------------------

#include "platform.h"

// compiler def
#if defined(__clang__) && !defined(_MSC_VER)
    #define AI_COMPILER_CLANG 1
#elif defined(__GNUC__)
    #define AI_COMPILER_GCC 1
#elif defined(_MSC_VER)
    #if defined(__clang__) && !defined(AI_COMPILER_CLANG_CL)
        #define AI_COMPILER_CLANG_CL 1
    #elif !defined(AI_COMPILER_MSVC)
        #define AI_COMPILER_MSVC 1
    #endif
#endif

// cxx 11
#if !defined(AI_CXX_11) && defined(__cplusplus)
    #if (__cplusplus >= 201103L)
        #define AI_CXX_11 1
    #elif defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)
        #define AI_CXX_11 1
    #elif defined(_MSC_VER) && _MSC_VER >= 1600
        #define AI_CXX_11 1
    #endif
#endif

// cxx 14
#if !defined(AI_CXX_14) && defined(__cplusplus)
    #if (__cplusplus >= 201402L)
        #define AI_CXX_14 1
    #elif defined(_MSC_VER) && (_MSC_VER >= 1900)
        #define AI_CXX_14 1
    #endif
#endif

// cxx 17
#if !defined(AI_CXX_17) && defined(__cplusplus)
    #if (__cplusplus >= 201703L)
        #define AI_CXX_17 1
    #elif defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)
        #define AI_CXX_17 1
    #endif
#endif

// cxx 20
#if !defined(AI_CXX_20) && defined(__cplusplus)
    #if (__cplusplus >= 202002L)
        #define AI_CXX_20 1
    #elif defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L)
        #define AI_CXX_20 1
    #endif
#endif

// cxx version
#if defined(AI_CXX_20)
    #define AI_CXX_VERSION 20
#elif defined(AI_CXX_17)
    #define AI_CXX_VERSION 17
#elif defined(AI_CXX_14)
    #define AI_CXX_VERSION 14
#elif defined(AI_CXX_11)
    #define AI_CXX_VERSION 11
#endif

// fall back
#include "compiler_fallback.inc"
