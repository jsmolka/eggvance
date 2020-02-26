#pragma once

#include "common/integer.h"

template<typename T, uint size>
class Register
{
public:
    virtual ~Register() = default;

    inline void reset()
    {
        static_cast<T&>(*this) = T();
    }

    template<uint index>
    inline u8 read() const
    {
        static_assert(index < size);

        return data[index];
    }

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < size);

        data[index] = byte;
    }

protected:
    template<typename U>
    inline U& cast()
    {
        static_assert(sizeof(U) <= size);

        return *reinterpret_cast<U*>(data);
    }

    u8 data[size] = { 0 };
};
