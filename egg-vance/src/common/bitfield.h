#pragma once

#include <utility>

#include "common/integer.h"

template<typename T, unsigned int position, unsigned int size>
class BitField
{
public:
    BitField(T& data)
        : data(data)
    {
        static_assert(std::is_same<T, u16>::value || std::is_same<T, u32>::value, "Invalid type");
        static_assert((position + size) <= (8 * sizeof(T)), "Invalid parameters");
    }

    inline BitField& operator=(int value)
    {
        data = (data & ~(mask << position)) | ((value & mask) << position);
        return *this;
    }

    inline operator unsigned int() const
    {
        return (data >> position) & mask;
    }

private:
    enum { mask = (static_cast<u64>(1) << size) - 1 };
    T& data;
};
