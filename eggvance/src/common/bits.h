#pragma once

#include <climits>
#include <type_traits>

#ifdef _MSC_VER
#include <intrin.h>
#elif not defined __EMSCRIPTEN__
#include <x86intrin.h>
#endif

#include "integer.h"

template<uint index, uint size, typename T>
inline T bits(T value)
{
    static_assert(std::is_integral_v<T>);
    static_assert(index + size <= sizeof(T) * CHAR_BIT);

    return (value >> index) & ((1ull << size) - 1);
}

template<typename T>
inline T sar(T value, uint amount)
{
    static_assert(std::is_integral_v<T>);

    return static_cast<std::make_signed_t<T>>(value) >> amount;
}

template<uint size, typename T>
inline T signExtend(T value)
{
    static_assert(std::is_integral_v<T>);
    static_assert(size <= sizeof(T) * CHAR_BIT);

    constexpr T mask = 1ull << (size - 1);

    return (value ^ mask) - mask;
}

inline uint rotateRight(uint value, uint amount)
{
    #ifdef _MSC_VER
    return _rotr(value, amount);
    #elif defined __clang__
    return __builtin_rotateright32(value, amount);
    #else
    amount &= 31;
    return (value >> amount) | (value << (-amount & 31));
    #endif
}

inline uint popcount(uint value)
{
    #ifdef _MSC_VER
    return __popcnt(value);
    #else
    return __builtin_popcount(value);
    #endif
}

inline uint bitScanForward(uint value)
{
    #ifdef _MSC_VER
    unsigned long index;
    _BitScanForward(&index, value);
    return static_cast<uint>(index);
    #else
    return __builtin_ctz(value);
    #endif
}

class SetBits
{
public:
    explicit SetBits(uint value)
        : value(value) {}

    class Iterator
    {
    public:
        explicit Iterator(uint value)
            : value(value) {}

        inline Iterator& operator++()
        {
            value &= value - 1;
            return *this;
        }

        inline uint operator*() const
        {
            return bitScanForward(value);
        }

        inline bool operator!=(const Iterator& other) const
        {
            return value != other.value;
        }

    private:
        uint value;
    };

    inline Iterator begin() const
    {
        return Iterator(value);
    }

    inline Iterator end() const
    {
        return Iterator(0);
    }

private:
    uint value;
};
