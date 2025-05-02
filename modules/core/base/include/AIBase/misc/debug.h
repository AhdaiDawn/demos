#pragma once
#include "AIBase/config.h"
#include "AIBase/config/platform.h"
#include "AIBase/misc/internal/debugbreak.h"
#include <stdio.h>

#define AI_BREAK() debug_break()
#define AI_HALT() debug_break()

#define AI_TRACE_MSG(msg)                                                                                              \
    printf(msg);                                                                                                       \
    printf("\n");

#define AI_TRACE_ASSERT(msg)                                                                                           \
    {                                                                                                                  \
        AI_TRACE_MSG(msg);                                                                                             \
        AI_HALT();                                                                                                     \
    }
#define AI_UNIMPLEMENTED_FUNCTION() AI_TRACE_ASSERT(__FILE__ ":  Function not implemented!\n")
#define AI_UNREACHABLE_CODE() AI_TRACE_ASSERT(__FILE__ ": Unreachable code encountered!\n")

#define AI_HEADER_SCOPE_DEFINING_PLATFORM_DEBUG

#ifdef _DEBUG
    #include <assert.h>
#else
    #ifndef assert
        #define assert(expr) (void)(expr);
    #endif
#endif

#ifndef AI_ASSERT
    #define AI_ASSERT(cond)                                                                                            \
        do                                                                                                             \
        {                                                                                                              \
            if (!(cond))                                                                                               \
            {                                                                                                          \
                AI_TRACE_MSG("AI Assert fired: " #cond " (" AI_FILE_LINE ")");                                         \
                assert((cond));                                                                                        \
            }                                                                                                          \
        } while (0)
#endif

// validate
#define AI_VERIFY(cond)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            AI_TRACE_MSG("AI Validate fired: " #cond " (" AI_FILE_LINE ")");                                           \
            AI_BREAK();                                                                                                \
        }                                                                                                              \
    } while (0)

#undef AI_HEADER_SCOPE_DEFINING_PLATFORM_CONFIGURE
