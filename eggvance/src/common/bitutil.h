#pragma once

#include <bitset>
#include <cstddef>
#include <type_traits>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace bitutil
{
    template<typename T>
    constexpr std::size_t bits()
    {
        return sizeof(T) * CHAR_BIT;
    }

    template<std::size_t position, std::size_t size, typename T>
    constexpr T get(T value)
    {
        static_assert(position + size <= bits<T>(), "Invalid parameters");

        return (value >> position) & ((1ull << size) - 1);
    }

    template<std::size_t position, std::size_t size, typename T>
    constexpr T set(T value, T data)
    {
        static_assert(position + size <= bits<T>(), "Invalid parameters");

        constexpr T mask = ((1ull << size) - 1) << position;

        return (value & ~mask) | ((data << position) & mask);
    }

    template<std::size_t size, typename T>
    constexpr T signExtend(T value)
    {
        static_assert(size <= bits<T>(), "Invalid parameters");

        constexpr T mask = 1ull << (size - 1);

        return (value ^ mask) - mask;
    }

    template<typename T>
    constexpr std::size_t bitCount(T value)
    {
        return std::bitset<bits<T>()>(value).count();
    }

    #pragma warning(push)
    #pragma warning(disable : 4267)

    template<typename T>
    constexpr int lowestSetBit(T value)
    {
        if (value == 0)
            return bits<T>();

        #ifdef _MSC_VER

        unsigned long index = 0;

        if (bits<T>() <= 32)
            _BitScanForward(&index, value);
        else
            _BitScanForward64(&index, value);

        return index;

        #else

        for (int x = 0; x < bits<T>(); ++x)
        {
            if (value & (1 << x))
                return x;
        }
        return bits<T>();

        #endif
    }

    template<typename T>
    constexpr int highestSetBit(T value)
    {
        if (value == 0)
            return 0;

        #ifdef _MSC_VER

        unsigned long index = 0;

        if (bits<T>() <= 32)
            _BitScanReverse(&index, value);
        else
            _BitScanReverse64(&index, value);

        return index;

        #else

        for (int x = bits<T>() - 1; x > -1; --x)
        {
            if (value & (1 << x))
                return x;
        }
        return 0;

        #endif
    }

    #pragma warning(pop)
}

