#pragma once

#include "register.h"
#include "common/bits.h"

class BGReference : public RegisterRW<4>
{
public:
    inline operator s32() const
    {
        return value;
    }

    template<uint index>
    inline u8 read() const = delete;

    template<uint index>
    inline void write(u8 byte)
    {
        static_assert(index < 4);

        if (index == 3)
            byte = signExtend<4>(byte);

        data[index] = byte;

        vblank();
    }

    inline void hblank(s16 parameter)
    {
        value += static_cast<s32>(parameter);
    }

    inline void vblank()
    {
        value = *reinterpret_cast<s32*>(data);
    }

private:
    s32 value = 0;
};
