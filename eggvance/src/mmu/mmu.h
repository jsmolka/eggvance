#pragma once

#include <array>
#include <memory>
#include <vector>

#include "dma/dma.h"
#include "gamepak/gamepak.h"
#include "ppu/oamentry.h"
#include "bios.h"
#include "memmap.h"
#include "mmio.h"
#include "timer.h"

class MMU
{
public:
    MMU(std::unique_ptr<BIOS> bios);

    void reset();
    void signalDMA(DMA::Timing timing);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    MMIO mmio;

    Timer timers[4];
    std::vector<DMA*> dmas_active;
    std::array<OAMEntry, 128> oam_entries;
    
    std::unique_ptr<BIOS> bios;
    std::unique_ptr<GamePak> gamepak;

    RAM<0x00400> palette;
    RAM<0x18000> vram;
    RAM<0x00400> oam;

private:
    void writeOAM(u32 addr, u16 half);

    DMA* dmas[4];

    RAM<0x40000> ewram;
    RAM<0x08000> iwram;
};
