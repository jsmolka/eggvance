#pragma once

#include "common/integer.h"

template<int size>
class Register
{
public:
    virtual ~Register() = default;

protected:
    template<typename T>
    static inline u8* bcast(T& data)
    {
        return reinterpret_cast<u8*>(&data);
    }
    u8 data[size];
};

template<typename T, uint size>
class TRegister
{
public:
    virtual ~TRegister() = default;

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
