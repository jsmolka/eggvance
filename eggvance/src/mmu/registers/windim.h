#pragma once

#include "common/macros.h"
#include "common/utility.h"

template<int limit>
class WinDim
{
public:
    void reset();

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    bool contains(int x) const;

    int min;
    int max;

private:
    struct Register
    {
        int min;
        int max;
    } reg;

    void update();
};

#include "windim.inl"
