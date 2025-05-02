#pragma once
#include "AIBase/config.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AI_DEFAULT_HASH_SEED_32 1610612741
#define AI_DEFAULT_HASH_SEED_64 8053064571610612741
#if UINTPTR_MAX == UINT32_MAX
    #define AI_DEFAULT_HASH_SEED AI_DEFAULT_HASH_SEED_32
#else
    #define AI_DEFAULT_HASH_SEED AI_DEFAULT_HASH_SEED_64
#endif

    AI_EXTERN_C AI_STATIC_API size_t ai_hash(const void *buffer, size_t size, size_t seed);

    AI_EXTERN_C AI_STATIC_API uint64_t ai_hash64(const void *buffer, uint64_t size, uint64_t seed);

    AI_EXTERN_C AI_STATIC_API uint32_t ai_hash32(const void *buffer, uint32_t size, uint32_t seed);

#ifdef __cplusplus
}
#endif
