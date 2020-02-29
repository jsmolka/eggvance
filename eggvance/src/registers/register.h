#pragma once

#include "common/integer.h"

template<uint size>
class RegisterRW
{
public:
    virtual ~RegisterRW() = default;

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
    template<typename T>
    inline T& cast()
    {
        static_assert(sizeof(T) <= size);

        return *reinterpret_cast<T*>(data);
    }

    u8 data[size] = {};
};

template<uint size>
class RegisterR : public RegisterRW<size>
{
public:
    template<uint index>
    inline void write(u8) = delete;
};

template<uint size>
class RegisterW : public RegisterRW<size>
{
public:
    template<uint index>
    inline u8 read() const = delete;
};
