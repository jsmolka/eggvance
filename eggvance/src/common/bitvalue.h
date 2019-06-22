#pragma once

template<unsigned int position, unsigned int size>
class BitValue
{
public:
    BitValue(unsigned int data)
        : value((data >> position) & mask)
    {

    }

    inline operator unsigned int() const
    {
        return value;
    }

private:
    enum { mask = (1ull << size) - 1 };
    unsigned int value;
};
