#pragma once

template<int position, int size>
class BitValue
{
public:
    BitValue(int data)
        : value((data >> position) & mask)
    {

    }

    inline operator int() const
    {
        return value;
    }

private:
    enum { mask = (1ull << size) - 1 };
    int value;
};
