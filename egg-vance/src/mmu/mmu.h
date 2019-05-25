#pragma once

#include <array>
#include <string>

#include "common/integer.h"
#include "registers/include.h"

class MMU
{
public:
    MMU();

    void reset();

    bool readFile(const std::string& file, u32 addr);

    void dump(u32 start, u32 size);

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    u8  readByteFast(u32 addr);
    u16 readHalfFast(u32 addr);
    u32 readWordFast(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    bool oam_changed;

    Dispcnt dispcnt;
    Dispstat dispstat;
    Vcount vcount;
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
    union
    {
        struct
        {
            Bghofs bg0hofs;
            Bghofs bg1hofs;
            Bghofs bg2hofs;
            Bghofs bg3hofs;
        };
        Bghofs bghofs[4];
    };
    union
    {
        struct
        {
            Bgvofs bg0vofs;
            Bgvofs bg1vofs;
            Bgvofs bg2vofs;
            Bgvofs bg3vofs;
        };
        Bgvofs bgvofs[4];
    };
    Keycnt keycnt;
    Keyinput keyinput;
    Waitcnt waitcnt;

private:
    template<typename T>
    T& ref(u32 addr);

    void checkAddress(u32 addr);

    std::array<u8, 0x10000000> memory;
};
