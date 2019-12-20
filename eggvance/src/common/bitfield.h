#pragma once

#include "bitutil.h"

template<typename T, unsigned index, unsigned size>
struct BitField
{
    constexpr operator T() const
    {
        return bitutil::get<index, size>(value);
    }

    constexpr BitField& operator=(T data)
    {
        value = bitutil::set<index, size>(value, data);

        return *this;
    }

    T value;
};
