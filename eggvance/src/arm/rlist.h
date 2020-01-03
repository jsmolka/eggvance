#pragma once

#include "common/bits.h"

class RList
{
public:
    RList(unsigned data)
        : data(data) {};

    class Iterator
    {
    public:
        Iterator(unsigned data)
            : data(data) {}

        Iterator operator++()
        {
            data &= data - 1;
            return *this;
        }

        unsigned operator*() const
        {
            return bitScanForward(data);
        }

        bool operator!=(const Iterator& other) const
        {
            return data != other.data;
        }

    private:
        unsigned data;
    };

    Iterator begin() const
    {
        return Iterator(data);
    }

    Iterator end() const
    {
        return Iterator(0);
    }

private:
    unsigned data;
};
