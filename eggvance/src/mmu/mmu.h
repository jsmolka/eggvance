#pragma once

#include "gamepak/gamepak.h"
#include "bios.h"
#include "memmap.h"

class MMU
{
public:
    void reset();

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    BIOS bios;
    GamePak gamepak;

    RAM<0x00400> palette;
    RAM<0x18000> vram;
    RAM<0x00400> oam;

private:
    void writeOAM(u32 addr, u16 half);

    RAM<0x40000> ewram;
    RAM<0x08000> iwram;
};

extern MMU mmu;
