#include "io.h"

#include "arm/arm.h"
#include "ppu/ppu.h"
#include "sys/keypad.h"
#include "memmap.h"
#include "mmu.h"

#define CASE1(label) case label + 0:
#define CASE2(label) case label + 0: case label + 1:
#define CASE4(label) case label + 0: case label + 1: case label + 2: case label + 3:

#define READ1(label, reg) CASE1(label) return reg.read(addr - label)
#define READ2(label, reg) CASE2(label) return reg.read(addr - label)
#define READ4(label, reg) CASE4(label) return reg.read(addr - label)

#define READ1_UNIMP(label) CASE1(label) return data.readByte(addr)
#define READ2_UNIMP(label) CASE2(label) return data.readByte(addr)
#define READ4_UNIMP(label) CASE4(label) return data.readByte(addr)

#define WRITE1(label, reg) CASE1(label) reg.write(addr - label, byte); break
#define WRITE2(label, reg) CASE2(label) reg.write(addr - label, byte); break
#define WRITE4(label, reg) CASE4(label) reg.write(addr - label, byte); break

#define WRITE1_UNIMP(label) CASE1(label) data.writeByte(addr, byte); break
#define WRITE2_UNIMP(label) CASE2(label) data.writeByte(addr, byte); break
#define WRITE4_UNIMP(label) CASE4(label) data.writeByte(addr, byte); break

IO::IO()
{
    std::memset(masks, 0xFF, sizeof(masks));

    masks[REG_DISPCNT     + 0] = 0xF7;
    masks[REG_DISPSTAT    + 0] = 0x38;
    masks[REG_BG0CNT      + 1] = 0xDF;
    masks[REG_BG1CNT      + 1] = 0xDF;
    masks[REG_BG0HOFS     + 1] = 0x01;
    masks[REG_BG0VOFS     + 1] = 0x01;
    masks[REG_BG1HOFS     + 1] = 0x01;
    masks[REG_BG1VOFS     + 1] = 0x01;
    masks[REG_BG2HOFS     + 1] = 0x01;
    masks[REG_BG2VOFS     + 1] = 0x01;
    masks[REG_BG3HOFS     + 1] = 0x01;
    masks[REG_BG3VOFS     + 1] = 0x01;
    masks[REG_BG2X        + 3] = 0x0F;    
    masks[REG_BG2Y        + 3] = 0x0F;    
    masks[REG_BG3X        + 3] = 0x0F;    
    masks[REG_BG3Y        + 3] = 0x0F;    
    masks[REG_WININ       + 0] = 0x3F;
    masks[REG_WININ       + 1] = 0x3F;
    masks[REG_WINOUT      + 0] = 0x3F;
    masks[REG_WINOUT      + 1] = 0x3F;
    masks[REG_BLDCNT      + 1] = 0x3F;
    masks[REG_BLDALPHA    + 0] = 0x1F;
    masks[REG_BLDALPHA    + 1] = 0x1F;
    masks[REG_BLDY        + 0] = 0x1F;
    masks[REG_BLDY        + 1] = 0x00;
    masks[REG_DMA0SAD     + 3] = 0x07;
    masks[REG_DMA1SAD     + 3] = 0x0F;
    masks[REG_DMA2SAD     + 3] = 0x0F;
    masks[REG_DMA3SAD     + 3] = 0x0F;
    masks[REG_DMA0DAD     + 3] = 0x07;
    masks[REG_DMA1DAD     + 3] = 0x07;
    masks[REG_DMA2DAD     + 3] = 0x07;
    masks[REG_DMA3DAD     + 3] = 0x0F;
    masks[REG_DMA0CNT_L   + 1] = 0x3F;
    masks[REG_DMA0CNT_H   + 0] = 0xE0;
    masks[REG_DMA0CNT_H   + 1] = 0xF7;
    masks[REG_DMA1CNT_L   + 1] = 0x3F;
    masks[REG_DMA1CNT_H   + 0] = 0xE0;
    masks[REG_DMA1CNT_H   + 1] = 0xF7;
    masks[REG_DMA2CNT_L   + 1] = 0x3F;
    masks[REG_DMA2CNT_H   + 0] = 0xE0;
    masks[REG_DMA2CNT_H   + 1] = 0xF7;
    masks[REG_DMA3CNT_H   + 0] = 0xE0;
    masks[REG_TM0CNT_H    + 0] = 0xC7;
    masks[REG_TM0CNT_H    + 1] = 0x00;
    masks[REG_TM1CNT_H    + 0] = 0xC7;
    masks[REG_TM1CNT_H    + 1] = 0x00;
    masks[REG_TM2CNT_H    + 0] = 0xC7;
    masks[REG_TM2CNT_H    + 1] = 0x00;
    masks[REG_TM3CNT_H    + 0] = 0xC7;
    masks[REG_TM3CNT_H    + 1] = 0x00;
    masks[REG_SOUND1CNT_L + 0] = 0x7F;
    masks[REG_SOUND1CNT_L + 1] = 0x00;
    masks[REG_SOUND1CNT_H + 0] = 0xC0;
    masks[REG_SOUND1CNT_X + 0] = 0x00;
    masks[REG_SOUND1CNT_X + 1] = 0x40;
    masks[REG_SOUND2CNT_L + 0] = 0xC0;
    masks[REG_SOUND2CNT_H + 0] = 0x00;
    masks[REG_SOUND2CNT_H + 1] = 0x40;
    masks[REG_SOUND3CNT_L + 0] = 0xE0;
    masks[REG_SOUND3CNT_L + 1] = 0x00;
    masks[REG_SOUND3CNT_H + 0] = 0x00;
    masks[REG_SOUND3CNT_H + 1] = 0xE0;
    masks[REG_SOUND3CNT_X + 0] = 0x00;
    masks[REG_SOUND3CNT_X + 1] = 0x40;
    masks[REG_SOUND4CNT_L + 0] = 0x00;
    masks[REG_SOUND4CNT_H + 1] = 0x40;
    masks[REG_SOUNDCNT_L  + 0] = 0x77;
    masks[REG_SOUNDCNT_H  + 0] = 0x0F;
    masks[REG_SOUNDCNT_H  + 1] = 0x77;
    masks[REG_SOUNDCNT_X  + 0] = 0x80;
    masks[REG_SOUNDCNT_X  + 1] = 0x00;
    masks[REG_KEYCNT      + 1] = 0xC3;
    masks[REG_IE          + 1] = 0x3F;
    masks[REG_IF          + 1] = 0x3F;
    masks[REG_WAITCNT     + 1] = 0x5F;
    masks[REG_IME         + 0] = 0x01;
    masks[REG_IME         + 1] = 0x00;
}

void IO::reset()
{
    data.fill(0);

    memcontrol[0] = 0;
    memcontrol[1] = 0;
    memcontrol[2] = 0;
    memcontrol[3] = 0;
}

u8 IO::readByte(u32 addr)
{
    u32 unused = addr;

    if (addr >= 0x400'0400)
    {
        if ((addr & 0xFFFC) == 0x800)
            return memcontrol[addr & 0x3];
        else
            return mmu.readUnused(unused);
    }
    addr &= 0x3FF;

    switch (addr)
    {
    READ2(REG_DISPCNT,  ppu.io.dispcnt);
    READ2(REG_DISPSTAT, ppu.io.dispstat);
    READ2(REG_VCOUNT,   ppu.io.vcount);
    READ2(REG_BG0CNT,   ppu.io.bgcnt[0]);
    READ2(REG_BG1CNT,   ppu.io.bgcnt[1]);
    READ2(REG_BG2CNT,   ppu.io.bgcnt[2]);
    READ2(REG_BG3CNT,   ppu.io.bgcnt[3]);
    READ2(REG_WININ,    ppu.io.winin);
    READ2(REG_WINOUT,   ppu.io.winout);
    READ2(REG_BLDCNT,   ppu.io.bldcnt);
    READ2(REG_BLDALPHA, ppu.io.bldalpha);
    READ2(REG_IE,       arm.io.irq_enabled);
    READ2(REG_IF,       arm.io.irq_request);
    READ2(REG_IME,      arm.io.irq_master);
    READ2(REG_WAITCNT,  arm.io.waitcnt);
    READ2(REG_KEYINPUT, keypad.io.keyinput);
    READ2(REG_KEYCNT,   keypad.io.keycnt);

    READ2_UNIMP(REG_GREENSWAP);
    READ2_UNIMP(REG_SOUND1CNT_L);
    READ2_UNIMP(REG_SOUND1CNT_H);
    READ2_UNIMP(REG_SOUND1CNT_X);
    READ2_UNIMP(REG_SOUND2CNT_L);
    READ2_UNIMP(REG_SOUND2CNT_H);
    READ2_UNIMP(REG_SOUND3CNT_L);
    READ2_UNIMP(REG_SOUND3CNT_H);
    READ2_UNIMP(REG_SOUND3CNT_X);
    READ2_UNIMP(REG_SOUND4CNT_L);
    READ2_UNIMP(REG_SOUND4CNT_H);
    READ2_UNIMP(REG_SOUNDCNT_L);
    READ2_UNIMP(REG_SOUNDCNT_H);
    READ2_UNIMP(REG_SOUNDCNT_X);
    READ2_UNIMP(REG_SOUNDBIAS);
    READ2_UNIMP(REG_WAVE_RAM_0);
    READ2_UNIMP(REG_WAVE_RAM_1);
    READ2_UNIMP(REG_WAVE_RAM_2);
    READ2_UNIMP(REG_WAVE_RAM_3);
    READ2_UNIMP(REG_WAVE_RAM_4);
    READ2_UNIMP(REG_WAVE_RAM_5);
    READ2_UNIMP(REG_WAVE_RAM_6);
    READ2_UNIMP(REG_WAVE_RAM_7);
    READ2_UNIMP(REG_SIOMULTI0);
    READ2_UNIMP(REG_SIOMULTI1);
    READ2_UNIMP(REG_SIOMULTI2);
    READ2_UNIMP(REG_SIOMULTI3);
    READ2_UNIMP(REG_SIOCNT);
    READ2_UNIMP(REG_SIOMLT_SEND);
    READ2_UNIMP(REG_RCNT);
    READ2_UNIMP(REG_JOYCNT);
    READ4_UNIMP(REG_JOY_RECV);
    READ4_UNIMP(REG_JOY_TRANS);
    READ2_UNIMP(REG_JOYSTAT);
    READ1_UNIMP(REG_POSTFLG);

    CASE2(REG_DMA0CNT_H)
    CASE2(REG_DMA1CNT_H)
    CASE2(REG_DMA2CNT_H)
    CASE2(REG_DMA3CNT_H)
        return arm.dma.readByte(addr);

    CASE2(REG_TM0CNT_L)
    CASE1(REG_TM0CNT_H)
    CASE2(REG_TM1CNT_L)
    CASE1(REG_TM1CNT_H)
    CASE2(REG_TM2CNT_L)
    CASE1(REG_TM2CNT_H)
    CASE2(REG_TM3CNT_L)
    CASE1(REG_TM3CNT_H)
        return arm.timer.readByte(addr);

    CASE1(REG_TM0CNT_H + 1)
    CASE1(REG_TM1CNT_H + 1)
    CASE1(REG_TM2CNT_H + 1)
    CASE1(REG_TM3CNT_H + 1)
    CASE2(REG_DMA0CNT_L)
    CASE2(REG_DMA1CNT_L)
    CASE2(REG_DMA2CNT_L)
    CASE2(REG_DMA3CNT_L)
    CASE2(0x066)
    CASE2(0x06E)
    CASE2(0x076)
    CASE2(0x07A)
    CASE2(0x07E)
    CASE2(0x086)
    CASE2(0x08A)
    CASE2(0x136)
    CASE2(0x142)
    CASE2(0x15A)
    CASE2(0x206)
        return 0;
    }
    return mmu.readUnused(unused);
}

u16 IO::readHalf(u32 addr)
{
    addr &= ~0x1;
    return readByte(addr) | readByte(addr + 1) << 8;
}

u32 IO::readWord(u32 addr)
{
    addr &= ~0x3;
    return readHalf(addr) | readHalf(addr + 2) << 16;
}

void IO::writeByte(u32 addr, u8 byte)
{
    if (addr >= 0x400'0400)
    {
        if ((addr & 0xFFFC) == 0x800)
            memcontrol[addr & 0x3] = byte;
        return;
    }
    addr &= 0x3FF;
    byte &= masks[addr];

    switch (addr)
    {
    WRITE2(REG_DISPCNT,  ppu.io.dispcnt);
    WRITE2(REG_DISPSTAT, ppu.io.dispstat);
    WRITE2(REG_BG0CNT,   ppu.io.bgcnt[0]);
    WRITE2(REG_BG1CNT,   ppu.io.bgcnt[1]);
    WRITE2(REG_BG2CNT,   ppu.io.bgcnt[2]);
    WRITE2(REG_BG3CNT,   ppu.io.bgcnt[3]);
    WRITE2(REG_BG0HOFS,  ppu.io.bghofs[0]);
    WRITE2(REG_BG0VOFS,  ppu.io.bgvofs[0]);
    WRITE2(REG_BG1HOFS,  ppu.io.bghofs[1]);
    WRITE2(REG_BG1VOFS,  ppu.io.bgvofs[1]);
    WRITE2(REG_BG2HOFS,  ppu.io.bghofs[2]);
    WRITE2(REG_BG2VOFS,  ppu.io.bgvofs[2]);
    WRITE2(REG_BG3HOFS,  ppu.io.bghofs[3]);
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
    WRITE2(REG_IE,       arm.io.irq_enabled);
    WRITE2(REG_IF,       arm.io.irq_request);
    WRITE2(REG_IME,      arm.io.irq_master);
    WRITE1(REG_HALTCNT,  arm.io.haltcnt);
    WRITE2(REG_WAITCNT,  arm.io.waitcnt);
    WRITE2(REG_KEYCNT,   keypad.io.keycnt);

    WRITE2_UNIMP(REG_GREENSWAP);
    WRITE2_UNIMP(REG_SOUND1CNT_L);
    WRITE2_UNIMP(REG_SOUND1CNT_H);
    WRITE2_UNIMP(REG_SOUND1CNT_X);
    WRITE2_UNIMP(REG_SOUND2CNT_L);
    WRITE2_UNIMP(REG_SOUND2CNT_H);
    WRITE2_UNIMP(REG_SOUND3CNT_L);
    WRITE2_UNIMP(REG_SOUND3CNT_H);
    WRITE2_UNIMP(REG_SOUND3CNT_X);
    WRITE2_UNIMP(REG_SOUND4CNT_L);
    WRITE2_UNIMP(REG_SOUND4CNT_H);
    WRITE2_UNIMP(REG_SOUNDCNT_L);
    WRITE2_UNIMP(REG_SOUNDCNT_H);
    WRITE2_UNIMP(REG_SOUNDCNT_X);
    WRITE2_UNIMP(REG_SOUNDBIAS);
    WRITE2_UNIMP(REG_WAVE_RAM_0);
    WRITE2_UNIMP(REG_WAVE_RAM_1);
    WRITE2_UNIMP(REG_WAVE_RAM_2);
    WRITE2_UNIMP(REG_WAVE_RAM_3);
    WRITE2_UNIMP(REG_WAVE_RAM_4);
    WRITE2_UNIMP(REG_WAVE_RAM_5);
    WRITE2_UNIMP(REG_WAVE_RAM_6);
    WRITE2_UNIMP(REG_WAVE_RAM_7);
    WRITE4_UNIMP(REG_FIFO_A);
    WRITE4_UNIMP(REG_FIFO_B);
    WRITE2_UNIMP(REG_SIOMULTI0);
    WRITE2_UNIMP(REG_SIOMULTI1);
    WRITE2_UNIMP(REG_SIOMULTI2);
    WRITE2_UNIMP(REG_SIOMULTI3);
    WRITE2_UNIMP(REG_SIOCNT);
    WRITE2_UNIMP(REG_SIOMLT_SEND);
    WRITE2_UNIMP(REG_RCNT);
    WRITE2_UNIMP(REG_JOYCNT);
    WRITE4_UNIMP(REG_JOY_RECV);
    WRITE4_UNIMP(REG_JOY_TRANS);
    WRITE2_UNIMP(REG_JOYSTAT);
    WRITE1_UNIMP(REG_POSTFLG);

    CASE4(REG_DMA0SAD)
    CASE4(REG_DMA0DAD)
    CASE4(REG_DMA1SAD)
    CASE4(REG_DMA1DAD)
    CASE4(REG_DMA2SAD)
    CASE4(REG_DMA2DAD)
    CASE4(REG_DMA3SAD)
    CASE4(REG_DMA3DAD)
    CASE2(REG_DMA0CNT_L)
    CASE2(REG_DMA0CNT_H)
    CASE2(REG_DMA1CNT_L)
    CASE2(REG_DMA1CNT_H)
    CASE2(REG_DMA2CNT_L)
    CASE2(REG_DMA2CNT_H)
    CASE2(REG_DMA3CNT_L)
    CASE2(REG_DMA3CNT_H)
        arm.dma.writeByte(addr, byte);
        break;

    CASE2(REG_TM0CNT_L)
    CASE1(REG_TM0CNT_H)
    CASE2(REG_TM1CNT_L)
    CASE1(REG_TM1CNT_H)
    CASE2(REG_TM2CNT_L)
    CASE1(REG_TM2CNT_H)
    CASE2(REG_TM3CNT_L)
    CASE1(REG_TM3CNT_H)
        arm.timer.writeByte(addr, byte);
        break;
    }
}

void IO::writeHalf(u32 addr, u16 half)
{
    addr &= ~0x1;
    writeByte(addr + 0, (half >> 0) & 0xFF);
    writeByte(addr + 1, (half >> 8) & 0xFF);
}

void IO::writeWord(u32 addr, u32 word)
{
    addr &= ~0x3;
    writeHalf(addr + 0, (word >>  0) & 0xFFFF);
    writeHalf(addr + 2, (word >> 16) & 0xFFFF);
}
