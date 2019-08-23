#pragma once

#include "common/integer.h"
#include "registers/include.h"
#include "ram.h"

class MMIO
{
public:
    void reset();

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    DisplayControl dispcnt;
    DisplayStatus dispstat;
    int vcount;

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

    int intr_master;
    int intr_enabled;
    int intr_request;
    int halt;

    int keyinput;
    KeyControl keycnt;
    WaitControl waitcnt;

private:
    RAM<0x400> data;
};
