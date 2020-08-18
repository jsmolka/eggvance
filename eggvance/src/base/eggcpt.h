#pragma once

// Temporary until eggcpt is public

#include <cassert>
#include <type_traits>
#include <fmt/format.h>

#include "base/bits.h"
#include "base/integer.h"
#include "base/fs.h"

namespace bit = bits;

namespace eggcpt
{

template<typename T, typename... Args>
void reconstruct(T& instance, Args&&... args)
{
    static_assert(std::is_constructible_v<T, Args...>);

    instance.~T();
    new(&instance)T(std::forward<Args>(args)...);
}

template<uint Size, uint Unsigned>
struct stdint {};

template<> struct stdint<1, 0> { using type =  s8; };
template<> struct stdint<1, 1> { using type =  u8; };
template<> struct stdint<2, 0> { using type = s16; };
template<> struct stdint<2, 1> { using type = u16; };
template<> struct stdint<4, 0> { using type = s32; };
template<> struct stdint<4, 1> { using type = u32; };
template<> struct stdint<8, 0> { using type = s64; };
template<> struct stdint<8, 1> { using type = u64; };

template<uint Size, uint Unsigned>
using stdint_t = typename stdint<Size, Unsigned>::type;

}  // namespace eggcpt

#define EGGCPT_CC_MSVC _MSC_VER
#define EGGCPT_CC_EMSCRIPTEN __EMSCRIPTEN__

#define EGGCPT_ASSERT(condition, ...) assert((condition) && (__VA_ARGS__""))
