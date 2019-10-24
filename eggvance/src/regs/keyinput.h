#pragma once

#include "common/macros.h"
#include "common/utility.h"

class KeyInput
{
public:
    void reset();

    inline operator int() const;
    inline KeyInput& operator|=(int value);
    inline KeyInput& operator&=(int value);

    template<int index>
    inline u8 readByte();
    template<int index>
    inline void writeByte(u8 byte);

    int keys;
};

#include "keyinput.inl"
