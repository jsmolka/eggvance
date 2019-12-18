#pragma once

#include <cstddef>

template<typename T, std::size_t position, std::size_t size>
struct BitField
{
    constexpr operator T() const
    {
        return (value >> position) & ((1ull << size) - 1);
    }

    constexpr BitField& operator=(T v)
    {
        constexpr T mask = ((1ull << size) - 1) << position;

        value = (value & ~mask) | ((v << position) & mask);

        return *this;
    }

    T value;
};
