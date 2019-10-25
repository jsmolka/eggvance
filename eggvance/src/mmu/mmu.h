#pragma once

#include "gamepak/gamepak.h"
#include "bios.h"
#include "palette.h"
#include "vram.h"
#include "oam.h"
#include "io.h"

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
    Palette palette;
    VRAM vram;
    OAM oam;
    IO io;

private:
    u32 readUnused(u32 addr);

    RAM<0x40000> ewram;
    RAM<0x08000> iwram;
};

extern MMU mmu;
