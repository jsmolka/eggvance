#pragma once

#include <climits>
#include <type_traits>

#include "defines.h"
#include "integer.h"
#include "macros.h"

#if COMPILER_MSVC
#include <intrin.h>
#elif !COMPILER_EMSCRIPTEN
#include <x86intrin.h>
#endif

namespace bits
{
    template<uint index, uint size, typename T>
    T seq(T value)
    {
        static_assert(std::is_integral_v<T>);
        static_assert(index + size <= CHAR_BIT * sizeof(T));

        return (value >> index) & ((1ull << size) - 1);
    }

    template<typename T>
    T sar(T value, uint amount)
    {
        static_assert(std::is_integral_v<T>);

        return static_cast<std::make_signed_t<T>>(value) >> amount;
    }

    template<typename T>
    T shr(T value, uint amount)
    {
        static_assert(std::is_integral_v<T>);

        return static_cast<std::make_unsigned_t<T>>(value) >> amount;
    }

    template<uint size, typename T>
    T sx(T value)
    {
        static_assert(std::is_integral_v<T>);
        static_assert(size <= CHAR_BIT * sizeof(T));

        constexpr T mask = 1ull << (size - 1);

        return (value ^ mask) - mask;
    }

    template<typename T>
    T ror(T value, uint amount)
    {
        #if COMPILER_MSVC
        if constexpr (sizeof(T) == 1) return _rotr8 (value, amount);
        if constexpr (sizeof(T) == 2) return _rotr16(value, amount);
        if constexpr (sizeof(T) == 4) return _rotr  (value, amount);
        if constexpr (sizeof(T) == 8) return _rotr64(value, amount);
        UNREACHABLE;
        #elif COMPILER_CLANG
        if constexpr (sizeof(T) == 1) return __builtin_rotateright8 (value, amount);
        if constexpr (sizeof(T) == 2) return __builtin_rotateright16(value, amount);
        if constexpr (sizeof(T) == 4) return __builtin_rotateright32(value, amount);
        if constexpr (sizeof(T) == 8) return __builtin_rotateright64(value, amount);
        UNREACHABLE;
        #else
        constexpr T mask = CHAR_BIT * sizeof(T) - 1;
        amount &= mask;
        return (value >> amount) | (value << (-amount & mask));
        #endif
    }

    template<typename T>
    T rol(T value, uint amount)
    {
        #if COMPILER_MSVC
        if constexpr (sizeof(T) == 1) return _rotl8 (value, amount);
        if constexpr (sizeof(T) == 2) return _rotl16(value, amount);
        if constexpr (sizeof(T) == 4) return _rotl  (value, amount);
        if constexpr (sizeof(T) == 8) return _rotl64(value, amount);
        UNREACHABLE;
        #elif COMPILER_CLANG
        if constexpr (sizeof(T) == 1) return __builtin_rotateleft8 (value, amount);
        if constexpr (sizeof(T) == 2) return __builtin_rotateleft16(value, amount);
        if constexpr (sizeof(T) == 4) return __builtin_rotateleft32(value, amount);
        if constexpr (sizeof(T) == 8) return __builtin_rotateleft64(value, amount);
        UNREACHABLE;
        #else
        constexpr T mask = CHAR_BIT * sizeof(T) - 1;
        amount &= mask;
        return (value << amount) | (value >> (-amount & mask));
        #endif
    }

    template<typename T>
    uint clz(T value)
    {
        #if COMPILER_MSVC
        unsigned long index;
        if constexpr (sizeof(T) <= 4)
            _BitScanReverse(&index, value);
        else
            _BitScanReverse64(&index, value);
        return static_cast<uint>(index);
        #else
        if constexpr (sizeof(T) <= 4)
            return __builtin_clz(value);
        else
            return __builtin_clzll(value);
        #endif
    }

    template<typename T>
    uint ctz(T value)
    {
        #if COMPILER_MSVC
        unsigned long index;
        if constexpr (sizeof(T) <= 4)
            _BitScanForward(&index, value);
        else
            _BitScanForward64(&index, value);
        return static_cast<uint>(index);
        #else
        if constexpr (sizeof(T) <= 4)
            return __builtin_ctz(value);
        else
            return __builtin_ctzll(value);
        #endif
    }

    template<typename T>
    uint popcnt(T value)
    {
        #ifdef COMPILER_MSVC
        if constexpr (sizeof(T) <= 2) return __popcnt16(value);
        if constexpr (sizeof(T) == 4) return __popcnt  (value);
        if constexpr (sizeof(T) == 8) return __popcnt64(value);
        UNREACHABLE;
        #else
        if constexpr (sizeof(T) <= 4)
            return __builtin_popcount(value);
        else
            return __builtin_popcountll(value);
        #endif
    }

    template<typename T>
    class SetBitsIterator
    {
    public:
        class Iterator
        {
        public:
            explicit Iterator(T value)
                : value(value) {}

            Iterator& operator++()
            {
                value &= value - 1;
                return *this;
            }

            uint operator*() const
            {
                return bits::ctz(value);
            }

            bool operator!=(const Iterator& other) const
            {
                return value != other.value;
            }

        private:
            T value;
        };

        explicit SetBitsIterator(T value)
            : value(value) {}

        Iterator begin()
        {
            return Iterator(value);
        }

        Iterator end()
        {
            return Iterator(0);
        }

    private:
        T value;
    };

    template<typename T>
    SetBitsIterator<T> iter(T value)
    {
        return SetBitsIterator<T>(value);
    }
}
