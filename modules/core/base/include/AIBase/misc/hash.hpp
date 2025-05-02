#pragma once
#include "AIBase/config.h"
#include "AIBase/misc/traits.hpp"
#include "xxhash/constexpr-xxh3.h"
#include <concepts>
#include <string_view>

namespace AI
{
template <constexpr_xxh3::BytesType Bytes> consteval uint64_t consteval_hash(const Bytes &input)
{
    return constexpr_xxh3::XXH3_64bits_const(input);
}
} // namespace AI

namespace AI
{
template <typename T> struct Hash;
namespace detail
{
template <typename T> using has_AI_hash = decltype(T::_AI_hash(std::declval<const T &>()));
template <typename T> using AI_hashable = decltype(std::declval<Hash<T>>().operator()(std::declval<const T &>()));
} // namespace detail
template <typename T> inline constexpr bool has_AI_hash_v = is_detected_v<detail::has_AI_hash, T>;
template <typename T> inline constexpr bool AI_hashable_v = is_detected_v<detail::AI_hashable, T>;

template <typename T>
concept EmbeddedHasher = requires(const T &t) {
    { t._AI_hash() } -> std::same_as<size_t>;
};
template <typename T>
concept Hashable = requires(const T &t) {
    { Hash<T>{}(t) } -> std::same_as<size_t>;
};

namespace detail
{
template <typename T, typename = void> struct HashSelector
{
};
template <typename T> struct HashSelector<T, std::enable_if_t<std::is_enum_v<T>>>
{
    size_t operator()(const T &p) const
    {
        return static_cast<size_t>(p);
    }
};
template <typename T> struct HashSelector<T, std::enable_if_t<has_AI_hash_v<T>>>
{
    size_t operator()(const T &p) const
    {
        return T::_AI_hash(p);
    }
    template <typename U>
        requires requires(const U &u) {
            { T::_AI_hash(u) } -> std::same_as<size_t>;
        }
    size_t operator()(const U &u) const
    {
        return T::_AI_hash(u);
    }
};
} // namespace detail

template <typename T> struct Hash : detail::HashSelector<T>
{
};

// hash combine
AI_INLINE constexpr size_t hash_combine(size_t seed, size_t combine)
{
    return seed ^ (combine + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

} // namespace AI

// primitive type hash
namespace AI
{
#define AI_IMPL_HASH_CAST(type)                                                                                        \
    template <> struct Hash<type>                                                                                      \
    {                                                                                                                  \
        AI_INLINE constexpr Hash()                                                                                     \
        {                                                                                                              \
        }                                                                                                              \
        AI_INLINE constexpr size_t operator()(type val) const                                                          \
        {                                                                                                              \
            return static_cast<size_t>(val);                                                                           \
        }                                                                                                              \
    };

// helper
AI_INLINE size_t fold_if_needed(uint64_t v)
{
    if constexpr (sizeof(size_t) == 4)
    {
        return static_cast<size_t>(v ^ (v >> 32));
    }
    else if constexpr (sizeof(size_t) == 8)
    {
        return static_cast<size_t>(v);
    }
}

// impl for pointer
template <typename T> struct Hash<T *>
{
    AI_INLINE size_t operator()(const T *val) const
    {
        return static_cast<size_t>(reinterpret_cast<const uintptr_t>(val));
    }
};

// impl for boolean
AI_IMPL_HASH_CAST(bool)

// impl for char
AI_IMPL_HASH_CAST(char)
AI_IMPL_HASH_CAST(signed char)
AI_IMPL_HASH_CAST(unsigned char)
AI_IMPL_HASH_CAST(wchar_t)
AI_IMPL_HASH_CAST(char8_t)
AI_IMPL_HASH_CAST(char16_t)
AI_IMPL_HASH_CAST(char32_t)

// impl for int
AI_IMPL_HASH_CAST(int16_t)
AI_IMPL_HASH_CAST(uint16_t)
AI_IMPL_HASH_CAST(int32_t)
AI_IMPL_HASH_CAST(uint32_t)
AI_IMPL_HASH_CAST(int64_t)
AI_IMPL_HASH_CAST(uint64_t)

// impl for float
template <> struct Hash<float>
{
    AI_INLINE size_t operator()(float val) const noexcept
    {
        // -0.0 and 0.0 should return same hash
        uint32_t *as_int = reinterpret_cast<uint32_t *>(&val);
        return (val == 0) ? static_cast<size_t>(0) : static_cast<size_t>(*as_int);
    }
};

// impl for double
template <> struct Hash<double>
{
    AI_INLINE size_t operator()(double val) const noexcept
    {
        // -0.0 and 0.0 should return same hash
        uint64_t *as_int = reinterpret_cast<uint64_t *>(&val);
        return (val == 0) ? static_cast<size_t>(0) : fold_if_needed(*as_int);
    }
};

#undef AI_IMPL_HASH_CAST
} // namespace AI

// impl for string
namespace AI
{
// char 8
template <> struct Hash<char *>
{
    AI_INLINE size_t operator()(const char *p) const
    {
        uint32_t c, result = 2166136261U; // FNV1 hash. Perhaps the best string hash. Intentionally uint32_t instead of
                                          // size_t, so the behavior is the same regardless of size.
        while ((c = (uint8_t)*p++) != 0)  // Using '!=' disables compiler warnings.
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};
template <> struct Hash<const char *>
{
    AI_INLINE size_t operator()(const char *p) const
    {
        return Hash<char *>()(p);
    }
};

// char 16
template <> struct Hash<char16_t *>
{
    AI_INLINE size_t operator()(const char16_t *p) const
    {
        uint32_t c, result = 2166136261U; // Intentionally uint32_t instead of size_t, so the behavior is the same
                                          // regardless of size.
        while ((c = (uint16_t)*p++) != 0) // cast to unsigned 16 bit.
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};
template <> struct Hash<const char16_t *>
{
    AI_INLINE size_t operator()(const char16_t *p) const
    {
        return Hash<char16_t *>()(p);
    }
};

// char 32
template <> struct Hash<char32_t *>
{
    AI_INLINE size_t operator()(const char32_t *p) const
    {
        uint32_t c, result = 2166136261U; // Intentionally uint32_t instead of size_t, so the behavior is the same
                                          // regardless of size.
        while ((c = (uint32_t)*p++) != 0) // cast to unsigned 32 bit.
            result = (result * 16777619) ^ c;
        return (size_t)result;
    }
};
template <> struct Hash<const char32_t *>
{
    AI_INLINE size_t operator()(const char32_t *p) const
    {
        return Hash<char32_t *>()(p);
    }
};
} // namespace AI
