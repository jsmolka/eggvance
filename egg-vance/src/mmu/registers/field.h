#pragma once

#include "common/integer.h"

template<int position, int size>
class Field
{
public:
    Field(u16& data) 
        : data(data) 
    { };

    inline Field& operator=(int value)
    {
        data = (data & ~(mask << position)) | ((value & mask) << position);
        return *this;
    }

    inline operator unsigned() const
    {
        return (data >> position) & mask;
    }

private:
    enum { mask = (1 << size) - 1 };
    u16& data;
};
