#pragma once

#include "bitutil.h"

template<typename T, std::size_t position, std::size_t size>
struct BitField
{
    constexpr operator T() const
    {
        return bitutil::get<position, size>(value);
    }

    constexpr BitField& operator=(T data)
    {
        value = bitutil::set<position, size>(value, data);

        return *this;
    }

    T value;
};
