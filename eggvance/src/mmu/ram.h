#pragma once

#include <array>

#include "common/integer.h"

template<std::size_t N>
struct RAM : public std::array<u8, N>
{
    template<typename T>
    inline T* ptr(std::size_t index)
    {
        return reinterpret_cast<T*>(this->data() + index);
    }

    template<typename T>
    inline T& ref(std::size_t index)
    {
        return *ptr<T>(index);
    }

    template<typename T>
    inline T get(std::size_t index)
    {
        return *ptr<T>(index);
    }
};
