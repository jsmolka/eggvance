#pragma once

#include <climits>
#include <type_traits>

#ifdef _MSC_VER
#include <intrin.h>
#elif not defined(__EMSCRIPTEN__)
#include <x86intrin.h>
#endif

#include <base/integer.h>
#include <base/iterator.h>
#include <base/macros.h>

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
        static_assert(std::is_integral_v<T>);

        #ifdef _MSC_VER
        if constexpr (sizeof(T) == 1) return _rotr8 (value, amount);
        if constexpr (sizeof(T) == 2) return _rotr16(value, amount);
        if constexpr (sizeof(T) == 4) return _rotr  (value, amount);
        if constexpr (sizeof(T) == 8) return _rotr64(value, amount);
        UNREACHABLE;
        #elif defined (__clang__)
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
        static_assert(std::is_integral_v<T>);

        #ifdef _MSC_VER
        if constexpr (sizeof(T) == 1) return _rotl8 (value, amount);
        if constexpr (sizeof(T) == 2) return _rotl16(value, amount);
        if constexpr (sizeof(T) == 4) return _rotl  (value, amount);
        if constexpr (sizeof(T) == 8) return _rotl64(value, amount);
        UNREACHABLE;
        #elif defined (__clang__)
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
        static_assert(std::is_integral_v<T>);

        #ifdef _MSC_VER
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
        static_assert(std::is_integral_v<T>);

        #ifdef _MSC_VER
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
        static_assert(std::is_integral_v<T>);

        #ifdef _MSC_VER
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
    class BitIterator
    {
        static_assert(std::is_integral_v<T>);

    public:
        explicit BitIterator(T value)
            : value(value) {}

        uint operator*() const
        {
            return ctz(value);
        }

        BitIterator& operator++()
        {
            value &= value - 1;
            return *this;
        }

        bool operator==(BitIterator other) const { return value == other.value; }
        bool operator!=(BitIterator other) const { return value != other.value; }

    private:
        T value;
    };

    template<typename T>
    auto iter(T value)
    {
        static_assert(std::is_integral_v<T>);

        return IteratorRange(
            BitIterator<T>(value),
            BitIterator<T>(0)
        );
    }
}
