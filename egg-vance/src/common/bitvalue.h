#pragma once

template<unsigned int position, unsigned int size>
class BitValue
{
public:
    BitValue(int data)
        : value((data >> position) & mask)
    {

    }

    inline operator unsigned int() const
    {
        return value;
    }

private:
    enum { mask = (1 << size) - 1 };
    int value;
};
