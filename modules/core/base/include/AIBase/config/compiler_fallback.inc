#ifdef __AI_COMPILER_FALLBACK_INC__
    #error "compiler_fallback.inc included twice"
#endif
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
//      AI_CXX_VER
//-------------------------------------------------------------------------------

// compiler
#ifndef AI_COMPILER_GCC
    #define AI_COMPILER_GCC 0
#endif
#ifndef AI_COMPILER_MSVC
    #define AI_COMPILER_MSVC 0
#endif
#ifndef AI_COMPILER_CLANG
    #define AI_COMPILER_CLANG 0
#endif
#ifndef AI_COMPILER_CLANG_CL
    #define AI_COMPILER_CLANG_CL 0
#endif

// cxx version
#ifndef AI_CXX_11
    #define AI_CXX_11 0
#endif
#ifndef AI_CXX_14
    #define AI_CXX_14 0
#endif
#ifndef AI_CXX_17
    #define AI_CXX_17 0
#endif
#ifndef AI_CXX_20
    #define AI_CXX_20 0
#endif

#define __AI_COMPILER_FALLBACK_INC__