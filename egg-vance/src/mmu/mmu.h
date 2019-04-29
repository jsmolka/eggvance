#pragma once

/**
 * Todo:
 * - split memory into actually used parts
 */

#include <array>
#include <string>

#include "common/integer.h"
#include "registers/io.h"

class MMU
{
public:
    MMU();

    void reset();

    bool loadRom(const std::string& file);

    u8 readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    void writeByte(u32 addr, u8 byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    Dispcnt dispcnt;
    Dispstat dispstat;
    union
    {
        struct
        {
            Bgcnt bg0cnt;
            Bgcnt bg1cnt;
            Bgcnt bg2cnt;
            Bgcnt bg3cnt;
        };
        Bgcnt bgcnt[4];
    };

private:
    u16& registerData(u32 addr);

    std::array<u8, 0x10000000> memory;
};
