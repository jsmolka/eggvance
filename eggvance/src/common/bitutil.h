#pragma once

#include <type_traits>

namespace bitutil
{
    template<typename T>
    constexpr unsigned bits()
    {
        return sizeof(T) * CHAR_BIT;
    }

    template<unsigned size>
    constexpr unsigned ones()
    {
        return (1ull << size) - 1;
    }

    template<unsigned index, unsigned size, typename T>
    constexpr T get(T value)
    {
        return (value >> index) & ones<size>();
    }

    template<unsigned index, unsigned size, typename T>
    constexpr T set(T value, T data)
    {
        constexpr T mask = ones<size>() << index;

        return (value & ~mask) | ((data << index) & mask);
    }

    template<unsigned size, typename T>
    constexpr T signExtend(T value)
    {
        constexpr T mask = 1ull << (size - 1);

        return (value ^ mask) - mask;
    }

    template<typename T>
    constexpr T rotateRight(T value, unsigned amount)
    {
        amount %= bits<T>();

        if (amount == 0)
            return value;

        auto x = static_cast<std::make_unsigned_t<T>>(value);

        return (x >> amount) | (x << (bits<T>() - amount));
    }

    constexpr unsigned popcount(unsigned value)
    {
        value = value - ((value >> 1) & 0x5555'5555);
        value = (value & 0x3333'3333) + ((value >> 2) & 0x3333'3333);
        return ((value + (value >> 4) & 0x0F0F'0F0F) * 0x0101'0101) >> 24;
    }

    template<typename T>
    constexpr unsigned scanForward(T value)
    {
        if (value == 0)
            return bits<T>() - 1;

        auto x = static_cast<std::make_unsigned_t<T>>(value);

        x = (x ^ (x - 1)) >> 1;

        unsigned index = 0;
        while (x >>= 1)
            index++;
        
        return index;
    }

    template<typename T>
    constexpr unsigned scanReverse(T value)
    {
        if (value == 0)
            return 0;

        auto v = static_cast<std::make_unsigned_t<T>>(value);

        unsigned index = 0;
        while (v >>= 1)
            index++;

        return index;
    }
}
