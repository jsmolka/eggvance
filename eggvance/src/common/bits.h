#pragma once

#include <climits>
#include <type_traits>

#include "integer.h"

template<typename T>
constexpr uint bitSize()
{
    return sizeof(T) * CHAR_BIT;
}

template<uint index, uint size, typename T>
constexpr T bits(T value)
{
    static_assert(std::is_integral_v<T>);
    static_assert(index + size <= bitSize<T>());

    return (value >> index) & ((1ull << size) - 1);
}

template<typename T>
constexpr T shr(T value, uint amount)
{
    static_assert(std::is_integral_v<T>);

    return static_cast<std::make_unsigned_t<T>>(value) >> amount;
}

template<typename T>
constexpr T sar(T value, uint amount)
{
    static_assert(std::is_integral_v<T>);

    return static_cast<std::make_signed_t<T>>(value) >> amount;
}

template<uint size, typename T>
constexpr T signExtend(T value)
{
    static_assert(std::is_integral_v<T>);
    static_assert(size <= bitSize<T>());

    constexpr T mask = 1ull << (size - 1);

    return (value ^ mask) - mask;
}

template<typename T>
constexpr T rotateLeft(T value, uint amount)
{
    static_assert(std::is_integral_v<T>);

    constexpr T mask = bitSize<T>() - 1;

    amount &= mask;
    return (value << amount) | shr(value, -amount & mask);
}

template<typename T>
constexpr T rotateRight(T value, uint amount)
{
    static_assert(std::is_integral_v<T>);

    constexpr T mask = bitSize<T>() - 1;

    amount &= mask;
    return shr(value, amount) | (value << (-amount & mask));
}

template<typename T>
constexpr uint popcount(T value)
{
    static_assert(std::is_integral_v<T>);

    using U = std::make_unsigned_t<T>;

    U x = static_cast<U>(value);

    x = x - ((x >> 1) & (U) ~(U)0 / 3);
    x = (x & (U) ~(U)0 / 15 * 3) + ((x >> 2) & (U) ~(U)0 / 15 * 3);
    x = (x + (x >> 4)) & (U) ~(U)0 / 255 * 15;
    return (U)(x * ((U) ~(U)0 / 255)) >> (sizeof(U) - 1) * 8;
}

template<typename T>
constexpr uint bitScanForward(T value)
{
    static_assert(std::is_integral_v<T>);

    return popcount((value ^ (value - 1)) >> 1);
}

template<typename T>
class SetBits
{
public:
    static_assert(std::is_integral_v<T>);

    constexpr SetBits(T value)
        : value(value) {}

    class Iterator
    {
    public:
        constexpr Iterator(T value)
            : value(value) {}

        constexpr Iterator& operator++()
        {
            value &= value - 1;
            return *this;
        }

        constexpr uint operator*() const
        {
            return bitScanForward(value);
        }

        constexpr bool operator==(const Iterator& other) const
        {
            return value == other.value;
        }

        constexpr bool operator!=(const Iterator& other) const
        {
            return value != other.value;
        }

    private:
        T value;
    };

    constexpr Iterator begin() const
    {
        return Iterator(value);
    }

    constexpr Iterator end() const
    {
        return Iterator(0);
    }

private:
    T value;
};
