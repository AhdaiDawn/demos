#pragma once
// platform headers
#ifndef __cplusplus
    #include <stdbool.h>
#endif

#if __has_include("sys/types.h")
    #include <sys/types.h>
#endif

#if __has_include("stdint.h")
    #include <stdint.h>
#endif

#if __has_include("float.h")
    #include <float.h>
#endif

// platform & compiler marcos
#include "AIBase/config/compiler.h"
#include "AIBase/config/platform.h"

// keywords
#include "AIBase/config/key_words.h"

// character
#include "AIBase/config/character.h"
