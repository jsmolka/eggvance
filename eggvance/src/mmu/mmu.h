#pragma once

#include <array>
#include <string>

#include "common/integer.h"
#include "registers/all.h"
#include "timer.h"

class MMU
{
public:
    MMU();

    void reset();

    bool readFile(const std::string& file, u32 addr);

    u8  readByte(u32 addr) const;
    u16 readHalf(u32 addr) const;
    u32 readWord(u32 addr) const;

    u8  readByteFast(u32 addr);
    u16 readHalfFast(u32 addr);
    u32 readWordFast(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    void writeByteFast(u32 addr, u8  byte);
    void writeHalfFast(u32 addr, u16 half);
    void writeWordFast(u32 addr, u32 word);

    template<typename T>
    T& ref(u32 addr);

    DisplayControl dispcnt;
    DisplayStatus dispstat;
    u8& vcount;

    BackgroundControl bgcnt[4];
    BackgroundOffset bghofs[4];
    BackgroundOffset bgvofs[4];
    BackgroundReference bgx[2];
    BackgroundReference bgy[2];
    BackgroundParameter bgpa[2];
    BackgroundParameter bgpb[2];
    BackgroundParameter bgpc[2];
    BackgroundParameter bgpd[2];

    WindowRange winh[2];
    WindowRange winv[2];
    WindowInside winin;
    WindowOutside winout;

    Mosaic mosaic;
    BlendControl bldcnt;
    BlendAlpha bldalpha;
    BlendFade bldy;

    KeyControl keycnt;
    WaitControl waitcnt;

    bool intr_master;
    u16& intr_enabled;
    u16& intr_request;

    TimerControl timer_control[4];
    TimerData timer_data[4];
    Timer timer[4];

    bool halt;

private:

    void demirror(u32& addr) const;

    std::array<u8, 0x10000000> memory;
};

template<typename T>
inline T& MMU::ref(u32 addr)
{
    return *reinterpret_cast<T*>(&memory[addr]);
}
