#pragma once

#include "common/integer.h"

template<unsigned int position, unsigned int size, typename T = u16>
class Field
{
public:
    Field(T& data) 
        : data(data) 
    {
    
    }

    inline Field& operator=(int value)
    {
        data = (data & ~(mask << position)) | ((value & mask) << position);
        return *this;
    }

    inline operator unsigned int() const
    {
        return (data >> position) & mask;
    }

private:
    enum { mask = (1 << size) - 1 };
    T& data;
};
