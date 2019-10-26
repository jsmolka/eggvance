#pragma once

#include <cstring>

#include "common/integer.h"

template<int size>
class Register
{
public:
    virtual ~Register() = default;

    inline void reset()
    {
        std::memset(data, 0, sizeof(data));
    }

protected:
    template<typename T>
    static inline u8* bcast(T& data)
    {
        return reinterpret_cast<u8*>(&data);
    }

    u8 data[size];
    u8 bytes[size];
};
