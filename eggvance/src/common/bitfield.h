#pragma once

template<typename T, int position, int size>
class BitField
{
public:
    BitField(T& data)
        : data(data)
    {
        static_assert((position + size) <= (8 * sizeof(T)), "Invalid parameters");
    }

    inline BitField& operator=(int value)
    {
        data = (data & ~(mask << position)) | ((value & mask) << position);
        return *this;
    }

    inline operator int() const
    {
        return (data >> position) & mask;
    }

private:
    enum { mask = (1ull << size) - 1 };
    T& data;
};
