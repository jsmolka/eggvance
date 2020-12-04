#pragma once

#include "bios.h"
#include "mmio.h"
#include "oam.h"
#include "paletteram.h"
#include "videoram.h"

class Mmu
{
public:
    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    PaletteRam pram;
    VideoRam vram;
    Oam oam;

private:
    u8 readSave(u32 addr);
    void writeSave(u32 addr, u8 byte);

    Bios bios;
    Ram<0x40000> ewram;
    Ram<0x08000> iwram;
    Mmio mmio;
};

inline Mmu mmu;
