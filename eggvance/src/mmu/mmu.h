#pragma once

#include <array>
#include <memory>
#include <vector>

#include "common/integer.h"
#include "gamepak/gamepak.h"
#include "ppu/oamentry.h"
#include "bios.h"
#include "dma.h"
#include "memory.h"
#include "mmio.h"
#include "ram.h"
#include "timer.h"

class MMU
{
public:
    MMU(std::unique_ptr<BIOS> bios);

    void reset();

    void setGamePak(std::unique_ptr<GamePak> gamepak);

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    void signalDMA(DMA::Timing timing);

    MMIO mmio;

    Timer timers[4];
    std::vector<DMA*> dmas_active;
    std::array<OAMEntry, 128> oam_entries;
    std::unique_ptr<GamePak> gamepak;

    RAM<0x00400> io;
    RAM<0x00400> palette;
    RAM<0x18000> vram;
    RAM<0x00400> oam;

private:
    u8 readIO(u32 addr);
    void writeIO(u32 addr, u8 byte);
    void writeOAM(u32 addr, u16 half);

    void writeBackgroundControlLower(BackgroundControl& control, u8 byte);
    void writeBackgroundControlUpper(BackgroundControl& control, u8 byte);
    void writeWindow(Window& window, u8 byte);
    void writeBlendLayer(BlendControl::Layer& layer, u8 byte);
    void writeTimerControl(Timer& timer, u8 byte);

    DMA dmas[4];

    std::unique_ptr<BIOS> bios;

    RAM<0x40000> ewram;
    RAM<0x08000> iwram;
};
