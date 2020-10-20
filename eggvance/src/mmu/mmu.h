#pragma once

#include "mmu/bios.h"
#include "mmu/io.h"
#include "mmu/oam.h"
#include "mmu/pram.h"
#include "mmu/vram.h"

class Mmu
{
public:
    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    u32 readUnused(u32 addr) const;

    Bios bios;
    Pram pram;
    Vram vram;
    Oam oam;
    Io io;

private:
    bool isEepromAccess(u32 addr) const;

    u8 readSave(u32 addr);
    void writeSave(u32 addr, u8 byte);

    Ram<0x40000> ewram;
    Ram<0x08000> iwram;
};

inline Mmu mmu;
