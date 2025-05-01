#pragma once
//-------------------------------------------------------------------------------
// unused
// alignas
// assume
// enable/disable optimization
// inline
// forceinline
// extern c
// export/import/static API
// ptr size
// no vtable
// noexcept
//-------------------------------------------------------------------------------

// UNUSED
#if defined(__cplusplus)
    #define AI_UNUSED [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
    #define AI_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
    #define AI_UNUSED
#endif

#ifdef __cplusplus
    #define AI_IF_CPP(...) __VA_ARGS__
#else
    #define AI_IF_CPP(...)
#endif

#if defined(__cplusplus)
    #define AI_CONSTEXPR constexpr
#else
    #define AI_CONSTEXPR const
#endif

// ALIGNAS
#if defined(_MSC_VER)
    #define AI_ALIGNAS(x) __declspec(align(x))
#else
    #define AI_ALIGNAS(x) __attribute__((aligned(x)))
#endif

// ASSUME
#ifndef AI_ASSUME
    #if defined(__clang__)
        #define AI_ASSUME(x) __builtin_assume(x)
    #elif defined(_MSC_VER)
        #define AI_ASSUME(x) __assume(x)
    #else
        #define AI_ASSUME(x)
    #endif
#endif

// OPTIMIZATION
#if defined(_MSC_VER)
    #define AI_DISABLE_OPTIMIZATION __pragma(optimize("", off))
    #define AI_ENABLE_OPTIMIZATION __pragma(optimize("", on))
#elif defined(__clang__)
    #define AI_DISABLE_OPTIMIZATION #pragma clang optimize off
    #define AI_ENABLE_OPTIMIZATION #pragma clang optimize on
#endif

// INLINE
#if defined(__cplusplus)
    #define AI_INLINE inline
#else
    #define AI_INLINE
#endif

// FORCEINLINE
#if defined(_MSC_VER) && !defined(__clang__)
    #define AI_FORCEINLINE __forceinline
#else
    #define AI_FORCEINLINE inline __attribute__((always_inline))
#endif

#if defined(_MSC_VER)
    #define AI_NOINLINE __declspec(noinline)
#else
    #define AI_NOINLINE __attribute__((noinline))
#endif

// EXTERN_C
#ifdef __cplusplus
    #define AI_EXTERN_C extern "C"
#else
    #define AI_EXTERN_C extern
#endif

// IMPORT
#ifndef AI_IMPORT
    #if defined(_MSC_VER)
        #define AI_IMPORT __declspec(dllimport)
    #else
        #define AI_IMPORT __attribute__((visibility("default")))
    #endif
#endif

// EXPORT
#ifndef AI_EXPORT
    #if defined(_MSC_VER)
        // MSVC linker trims symbols, the 'dllexport' attribute prevents this.
        // But we are not archiving DLL files with SHIPPING_ONE_ARCHIVE mode.
        #define AI_EXPORT __declspec(dllexport)
    #else
        #define AI_EXPORT __attribute__((visibility("default")))
    #endif
#endif

// STATIC
#define AI_STATIC_API

// NO_VTABLE
#ifdef _MSC_VER
    #define AI_NO_VTABLE __declspec(novtable)
#else
    #define AI_NO_VTABLE
#endif

// NOEXCEPT
#ifdef __cplusplus
    // By Default we use cpp-standard above 2011XXL
    #define AI_NOEXCEPT noexcept
#else
    #define AI_NOEXCEPT
#endif

#if defined(_MSC_VER)
    #if defined(__clang__)
        #define AI_UNREF_PARAM(x) (void)x
    #else
        #define AI_UNREF_PARAM(x) (x)
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define AI_UNREF_PARAM(x) ((void)(x))
#endif

#if defined(_MSC_VER)
    #define AI_CALLCONV __cdecl
#elif defined(__GNUC__) || defined(__clang__)
    #define AI_CALLCONV
#endif

#if defined(__cplusplus)
    #define AI_DECLARE_ZERO(type, var)                                                                \
        static_assert(std::is_trivially_constructible<type>::value, "not trival, 0 init is invalid!"); \
        type var = {};
#else
    #define AI_DECLARE_ZERO(type, var) type var = { 0 };
#endif

// VLA
#ifndef __cplusplus
    #if defined(_MSC_VER) && !defined(__clang__)
        #define AI_DECLARE_ZERO_VLA(type, var, num)          \
            type* var = (type*)_alloca(sizeof(type) * (num)); \
            memset((void*)(var), 0, sizeof(type) * (num));
    #else
        #define AI_DECLARE_ZERO_VLA(type, var, num) \
            type var[(num)];                         \
            memset((void*)(var), 0, sizeof(type) * (num));
    #endif
#endif

#pragma region stringizing

#ifndef AI_STRINGIZING
    #define AI_STRINGIZING(...) #__VA_ARGS__
#endif

#ifndef AI_MAKE_STRING
    #define AI_MAKE_STRING(...) AI_STRINGIZING(__VA_ARGS__)
#endif

#ifndef AI_FILE_LINE
    #define AI_FILE_LINE __FILE__ ":" AI_MAKE_STRING(__LINE__)
#endif

#pragma endregion

#pragma region utf-8

#if __cplusplus >= 201100L
    #define AI_UTF8(str) u8##str
#else
    #define AI_UTF8(str) str
#endif

#if __cpp_char8_t
    #define CHAR8_T_DEFINED
    #define OSTR_USE_CXX20_CHAR8_TYPE
#endif

#ifndef CHAR8_T_DEFINED // If the user hasn't already defined these...
    #if defined(EA_PLATFORM_APPLE)
        #define char8_t char // The Apple debugger is too stupid to realize char8_t is typedef'd to char, so we #define it.
    #else
typedef char char8_t;
    #endif
    #define CHAR8_T_DEFINED
#endif

#pragma endregion

#pragma region typedecl

#ifdef __cplusplus
    #define AI_DECLARE_TYPE_ID_FWD(ns, type, ctype) \
        namespace ns                                 \
        {                                            \
        struct type;                                 \
        }                                            \
        using ctype##_t  = ns::type;                 \
        using ctype##_id = ns::type*;
#else
    #define AI_DECLARE_TYPE_ID_FWD(ns, type, ctype) \
        typedef struct ctype##_t ctype##_t;          \
        typedef struct ctype*    ctype##_id;
#endif

#ifdef __cplusplus
    #define AI_DECLARE_TYPE_ID(type, ctype) \
        typedef struct type ctype##_t;       \
        typedef type*       ctype##_id;
#else
    #define AI_DECLARE_TYPE_ID(type, ctype) \
        typedef struct ctype##_t ctype##_t;  \
        typedef ctype##_t*       ctype##_id;
#endif

#pragma endregion

#if defined(_MSC_VER) // msvc always little endian
    #define AI_LITTLE_ENDIAN 1
    #define AI_BIG_ENDIAN 0
#else
    #if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define AI_LITTLE_ENDIAN 1
        #define AI_BIG_ENDIAN 0
    #elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define AI_LITTLE_ENDIAN 0
        #define AI_BIG_ENDIAN 1
    #endif
#endif

#if AI_BIG_ENDIAN
    #error "big endian is not supported"
#endif


#pragma region deprecated

#if defined(__has_cpp_attribute)
    #if __has_cpp_attribute(deprecated)
        #define AI_DEPRECATED(msg) [[deprecated(msg)]]
    #endif
#endif

#if !defined(AI_DEPRECATED)
    #if defined(__GNUC__) || defined(__clang__)
        #define AI_DEPRECATED(msg) __attribute__((deprecated))
    #elif defined(_MSC_VER)
        #define AI_DEPRECATED(msg) __declspec(deprecated)
    #else
        #define AI_DEPRECATED(msg)
    #endif
#endif

#pragma endregion

#define DISABLE_COPY_AND_MOVE(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete; \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;
