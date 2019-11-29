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
