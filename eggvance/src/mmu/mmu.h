#pragma once

#include "bios.h"
#include "palette.h"
#include "vram.h"
#include "oam.h"
#include "io.h"
#include "gamepak/gamepak.h"

class Core;

class MMU
{
public:
    MMU(Core& core);

    void reset();

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    u32 readUnused(u32 addr);

    BIOS bios;
    GamePak gamepak;
    Palette palette;
    VRAM vram;
    OAM oam;
    Io io;

private:
    u8 readSave(u32 addr);
    void writeSave(u32 addr, u8 byte);

    Core& core;
    MirroredRAM<0x40000> ewram;
    MirroredRAM<0x08000> iwram;
};
