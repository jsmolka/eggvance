#pragma once

#include "common/ram.h"
#include "palette.h"

class VRAM
{
public:
    void reset();

    template<typename T>
    inline T* ptr(std::size_t index)
    {
        return data.data<T>(index);
    }

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    u16 readPixel(u32 addr, int x, int y, Palette::Format format);

private:
    class Data : public RAM<0x2'0000>
    {
    private:
        inline u32 mirror(u32 addr) const override final
        {
            if (addr >= 0x1'8000)
                addr -= 0x8000;
            return addr;
        }
    } data;
};
