#include "io.h"

#include "arm/arm.h"
#include "ppu/ppu.h"
#include "sys/keypad.h"
#include "memmap.h"

void IO::reset()
{
    data.fill(0);
}

#define READ2(label, reg)                    \
    case label + 0: return reg.readByte(0);  \
    case label + 1: return reg.readByte(1)

#define READ4(label, reg)                    \
    case label + 0: return reg.readByte(0);  \
    case label + 1: return reg.readByte(1);  \
    case label + 2: return reg.readByte(2);  \
    case label + 3: return reg.readByte(3)

u8 IO::readByte(u32 addr)
{
    addr &= 0x3FF;

    if (addr >= 0xB0 && addr < 0xE0)
    {
        return arm.dma.readByte(addr);
    }

    switch (addr)
    {
    READ2(REG_DISPCNT,  ppu.io.dispcnt);
    READ2(REG_DISPSTAT, ppu.io.dispstat);
    READ2(REG_VCOUNT,   ppu.io.vcount);
    READ2(REG_BG0CNT,   ppu.io.bgcnt[0]);
    READ2(REG_BG1CNT,   ppu.io.bgcnt[1]);
    READ2(REG_BG2CNT,   ppu.io.bgcnt[2]);
    READ2(REG_BG3CNT,   ppu.io.bgcnt[3]);
    READ2(REG_BG0HOFS,  ppu.io.bghofs[0]);
    READ2(REG_BG1HOFS,  ppu.io.bghofs[1]);
    READ2(REG_BG2HOFS,  ppu.io.bghofs[2]);
    READ2(REG_BG3HOFS,  ppu.io.bghofs[3]);
    READ2(REG_BG0VOFS,  ppu.io.bgvofs[0]);
    READ2(REG_BG1VOFS,  ppu.io.bgvofs[1]);
    READ2(REG_BG2VOFS,  ppu.io.bgvofs[2]);
    READ2(REG_BG3VOFS,  ppu.io.bgvofs[3]);
    READ2(REG_BG2PA,    ppu.io.bgpa[0]);
    READ2(REG_BG2PB,    ppu.io.bgpb[0]);
    READ2(REG_BG2PC,    ppu.io.bgpc[0]);
    READ2(REG_BG2PD,    ppu.io.bgpd[0]);
    READ2(REG_BG3PA,    ppu.io.bgpa[1]);
    READ2(REG_BG3PB,    ppu.io.bgpb[1]);
    READ2(REG_BG3PC,    ppu.io.bgpc[1]);
    READ2(REG_BG3PD,    ppu.io.bgpd[1]);
    READ4(REG_BG2X,     ppu.io.bgx[0]);
    READ4(REG_BG2Y,     ppu.io.bgy[0]);
    READ4(REG_BG3X,     ppu.io.bgx[1]);
    READ4(REG_BG3Y,     ppu.io.bgy[1]);
    READ2(REG_WIN0H,    ppu.io.winh[0]);
    READ2(REG_WIN0V,    ppu.io.winv[0]);
    READ2(REG_WIN1H,    ppu.io.winh[1]);
    READ2(REG_WIN1V,    ppu.io.winv[1]);
    READ2(REG_WININ,    ppu.io.winin);
    READ2(REG_WINOUT,   ppu.io.winout);
    READ2(REG_MOSAIC,   ppu.io.mosaic);
    READ2(REG_BLDCNT,   ppu.io.bldcnt);
    READ2(REG_BLDALPHA, ppu.io.bldalpha);
    READ2(REG_BLDY,     ppu.io.bldy);
    READ2(REG_IME,      arm.io.irq_master);
    READ2(REG_IE,       arm.io.irq_enabled);
    READ2(REG_IF,       arm.io.irq_request);
    READ2(REG_WAITCNT,  arm.io.waitcnt);
    READ4(REG_TM0CNT,   arm.timers[0]);
    READ4(REG_TM1CNT,   arm.timers[1]);
    READ4(REG_TM2CNT,   arm.timers[2]);
    READ4(REG_TM3CNT,   arm.timers[3]);
    READ2(REG_KEYINPUT, keypad.io.keyinput);
    READ2(REG_KEYCNT,   keypad.io.keycnt);

    case REG_HALTCNT:
        return 0;
    }
    return data.readByte(addr);
}

#undef READ4
#undef READ2

u16 IO::readHalf(u32 addr)
{
    addr &= 0x3FE;
    return readByte(addr) | readByte(addr + 1) << 8;
}

u32 IO::readWord(u32 addr)
{
    addr &= 0x3FC;
    return readHalf(addr) | readHalf(addr + 2) << 16;
}

#define WRITE2(label, reg)                           \
    case label + 0: reg.writeByte(0, byte); return;  \
    case label + 1: reg.writeByte(1, byte); return

#define WRITE4(label, reg)                           \
    case label + 0: reg.writeByte(0, byte); return;  \
    case label + 1: reg.writeByte(1, byte); return;  \
    case label + 2: reg.writeByte(2, byte); return;  \
    case label + 3: reg.writeByte(3, byte); return

void IO::writeByte(u32 addr, u8 byte)
{
    addr &= 0x3FF;

    if (addr >= 0xB0 && addr < 0xE0)
    {
        arm.dma.writeByte(addr, byte);
        return;
    }

    switch (addr)
    {
    WRITE2(REG_DISPCNT,  ppu.io.dispcnt);
    WRITE2(REG_DISPSTAT, ppu.io.dispstat);
    WRITE2(REG_VCOUNT,   ppu.io.vcount);
    WRITE2(REG_BG0CNT,   ppu.io.bgcnt[0]);
    WRITE2(REG_BG1CNT,   ppu.io.bgcnt[1]);
    WRITE2(REG_BG2CNT,   ppu.io.bgcnt[2]);
    WRITE2(REG_BG3CNT,   ppu.io.bgcnt[3]);
    WRITE2(REG_BG0HOFS,  ppu.io.bghofs[0]);
    WRITE2(REG_BG1HOFS,  ppu.io.bghofs[1]);
    WRITE2(REG_BG2HOFS,  ppu.io.bghofs[2]);
    WRITE2(REG_BG3HOFS,  ppu.io.bghofs[3]);
    WRITE2(REG_BG0VOFS,  ppu.io.bgvofs[0]);
    WRITE2(REG_BG1VOFS,  ppu.io.bgvofs[1]);
    WRITE2(REG_BG2VOFS,  ppu.io.bgvofs[2]);
    WRITE2(REG_BG3VOFS,  ppu.io.bgvofs[3]);
    WRITE2(REG_BG2PA,    ppu.io.bgpa[0]);
    WRITE2(REG_BG2PB,    ppu.io.bgpb[0]);
    WRITE2(REG_BG2PC,    ppu.io.bgpc[0]);
    WRITE2(REG_BG2PD,    ppu.io.bgpd[0]);
    WRITE2(REG_BG3PA,    ppu.io.bgpa[1]);
    WRITE2(REG_BG3PB,    ppu.io.bgpb[1]);
    WRITE2(REG_BG3PC,    ppu.io.bgpc[1]);
    WRITE2(REG_BG3PD,    ppu.io.bgpd[1]);
    WRITE4(REG_BG2X,     ppu.io.bgx[0]);
    WRITE4(REG_BG2Y,     ppu.io.bgy[0]);
    WRITE4(REG_BG3X,     ppu.io.bgx[1]);
    WRITE4(REG_BG3Y,     ppu.io.bgy[1]);
    WRITE2(REG_WIN0H,    ppu.io.winh[0]);
    WRITE2(REG_WIN0V,    ppu.io.winv[0]);
    WRITE2(REG_WIN1H,    ppu.io.winh[1]);
    WRITE2(REG_WIN1V,    ppu.io.winv[1]);
    WRITE2(REG_WININ,    ppu.io.winin);
    WRITE2(REG_WINOUT,   ppu.io.winout);
    WRITE2(REG_MOSAIC,   ppu.io.mosaic);
    WRITE2(REG_BLDCNT,   ppu.io.bldcnt);
    WRITE2(REG_BLDALPHA, ppu.io.bldalpha);
    WRITE2(REG_BLDY,     ppu.io.bldy);
    WRITE2(REG_IME,      arm.io.irq_master);
    WRITE2(REG_IE,       arm.io.irq_enabled);
    WRITE2(REG_IF,       arm.io.irq_request);
    WRITE2(REG_WAITCNT,  arm.io.waitcnt);
    WRITE4(REG_TM0CNT,   arm.timers[0]);
    WRITE4(REG_TM1CNT,   arm.timers[1]);
    WRITE4(REG_TM2CNT,   arm.timers[2]);
    WRITE4(REG_TM3CNT,   arm.timers[3]);
    WRITE2(REG_KEYINPUT, keypad.io.keyinput);
    WRITE2(REG_KEYCNT,   keypad.io.keycnt);

    case REG_HALTCNT:
        arm.io.halt = true;
        break;
    }
    data.writeByte(addr, byte);
}

#undef WRITE4
#undef WRITE2

void IO::writeHalf(u32 addr, u16 half)
{
    addr &= 0x3FE;
    writeByte(addr + 0, (half >> 0) & 0xFF);
    writeByte(addr + 1, (half >> 8) & 0xFF);
}

void IO::writeWord(u32 addr, u32 word)
{
    addr &= 0x3FC;
    writeHalf(addr + 0, (word >>  0) & 0xFFFF);
    writeHalf(addr + 2, (word >> 16) & 0xFFFF);
}
