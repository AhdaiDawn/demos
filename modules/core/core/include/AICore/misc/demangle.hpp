// sol2

// The MIT License (MIT)

// Copyright (c) 2013-2022 Rapptz, ThePhD and contributors

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once
#include <cctype>
#include <string>

namespace AI
{
namespace detail
{
inline constexpr std::string_view removals[] = {
    "{anonymous}",           "(anonymous namespace)", "public:", "private:", "protected:", "struct ", "class ",
    "`anonymous-namespace'", "`anonymous namespace'"};

// check compiler is gcc or clang
#if defined(__GNUC__) || defined(__clang__)
inline std::string ctti_get_type_name_from_sig(std::string name)
{
    // cardinal sins from MINGW
    size_t start = name.find_first_of('[');
    start = name.find_first_of('=', start);
    size_t end = name.find_last_of(']');
    if (end == std::string::npos)
        end = name.size();
    if (start == std::string::npos)
        start = 0;
    if (start < name.size() - 1)
        start += 1;
    name = name.substr(start, end - start);
    start = name.rfind("seperator_mark");
    if (start != std::string::npos)
    {
        name.erase(start - 2, name.length());
    }
    while (!name.empty() && isblank(name.front()))
        name.erase(name.begin());
    while (!name.empty() && isblank(name.back()))
        name.pop_back();

    for (auto removal : removals)
    {
        auto found = name.find(removal.data(), 0, removal.size());
        while (found != std::string::npos)
        {
            name.erase(found, removal.size());
            found = name.find(removal.data(), 0, removal.size());
        }
    }

    return name;
}

template <typename T, class seperator_mark = int> inline std::string ctti_get_type_name()
{
    return ctti_get_type_name_from_sig(__PRETTY_FUNCTION__);
}
#elif defined(_MSC_VER)
inline std::string ctti_get_type_name_from_sig(std::string name)
{
    size_t start = name.find("get_type_name");
    if (start == std::string::npos)
        start = 0;
    else
        start += 13;
    if (start < name.size() - 1)
        start += 1;
    size_t end = name.find_last_of('>');
    if (end == std::string::npos)
        end = name.size();
    name = name.substr(start, end - start);
    if (name.find("struct", 0) == 0)
        name.replace(0, 6, "", 0);
    if (name.find("class", 0) == 0)
        name.replace(0, 5, "", 0);
    while (!name.empty() && isblank(name.front()))
        name.erase(name.begin());
    while (!name.empty() && isblank(name.back()))
        name.pop_back();

    for (auto removal : removals)
    {
        auto found = name.find(removal.data(), 0, removal.size());
        while (found != std::string::npos)
        {
            name.erase(found, removal.size());
            found = name.find(removal.data(), 0, removal.size());
        }
    }

    return name;
}

template <typename T> std::string ctti_get_type_name()
{
    return ctti_get_type_name_from_sig(__FUNCSIG__);
}
#else
    #error Compiler not supported for demangling
#endif // compilers

template <typename T> std::string demangle_once()
{
    std::string realname = ctti_get_type_name<T>();
    return realname;
}

inline std::string short_demangle_from_type_name(std::string realname)
{
    // This isn't the most complete but it'll do for now...?
    static const std::string ops[] = {"operator<",  "operator<<",  "operator<<=", "operator<=", "operator>",
                                      "operator>>", "operator>>=", "operator>=",  "operator->", "operator->*"};
    int level = 0;
    size_t idx = 0;
    for (idx = static_cast<size_t>(realname.empty() ? 0 : realname.size() - 1); idx > 0; --idx)
    {
        if (level == 0 && realname[idx] == ':')
        {
            break;
        }
        bool isleft = realname[idx] == '<';
        bool isright = realname[idx] == '>';
        if (!isleft && !isright)
            continue;
        bool earlybreak = false;
        for (const auto &op : ops)
        {
            size_t nisop = realname.rfind(op, idx);
            if (nisop == std::string::npos)
                continue;
            size_t nisopidx = idx - op.size() + 1;
            if (nisop == nisopidx)
            {
                idx = static_cast<size_t>(nisopidx);
                earlybreak = true;
            }
            break;
        }
        if (earlybreak)
        {
            continue;
        }
        level += isleft ? -1 : 1;
    }
    if (idx > 0)
    {
        realname.erase(0, realname.length() < static_cast<size_t>(idx) ? realname.length() : idx + 1);
    }
    return realname;
}

template <typename T> std::string short_demangle_once()
{
    std::string realname = ctti_get_type_name<T>();
    return short_demangle_from_type_name(realname);
}

template <typename T> const std::string &demangle()
{
    static const std::string d = demangle_once<T>();
    return d;
}

template <typename T> const std::string &short_demangle()
{
    static const std::string d = short_demangle_once<T>();
    return d;
}
} // namespace detail
using detail::demangle;
using detail::short_demangle;
} // namespace AI
