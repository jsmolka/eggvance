#include "mmio.h"

#include "common/utility.h"
#include "memory.h"

MMIO::MMIO()
{
    dma.sad[0].addr_mask = 0x07FF'FFFF;
    dma.sad[1].addr_mask = 0x0FFF'FFFF;
    dma.sad[2].addr_mask = 0x0FFF'FFFF;
    dma.sad[3].addr_mask = 0x0FFF'FFFF;
    dma.dad[0].addr_mask = 0x07FF'FFFF;
    dma.dad[1].addr_mask = 0x07FF'FFFF;
    dma.dad[2].addr_mask = 0x07FF'FFFF;
    dma.dad[3].addr_mask = 0x0FFF'FFFF;

    dma.control[0].count_mask = 0x3FFF;
    dma.control[1].count_mask = 0x3FFF;
    dma.control[2].count_mask = 0x3FFF;
    dma.control[3].count_mask = 0xFFFF;

    reset();
}

void MMIO::reset()
{
    data.fill(0);

    dispcnt.reset();
    dispstat.reset();
    vcount = 0;

    for (int x = 0; x < 4; ++x)
    {
        bgcnt[x].reset();
        bghofs[x].reset();
        bgvofs[x].reset();
        dma.sad[x].reset();
        dma.dad[x].reset();
        dma.control[x].reset();
        tmcnt[x].reset();
    }

    for (int x = 0; x < 2; ++x)
    {
        bgx[x].reset();
        bgy[x].reset();
        bgpa[x].reset();
        bgpb[x].reset();
        bgpc[x].reset();
        bgpd[x].reset();
        winh[x].reset();
        winv[x].reset();
    }

    winin.reset();
    winout.reset();

    mosaic.reset();
    bldcnt.reset();
    bldalpha.reset();
    bldy.reset();

    intr_master = 0;
    intr_enabled = 0;
    intr_request = 0;
    halt = false;

    keyinput = 0x3FF;
    keycnt.reset();
    waitcnt.reset();
}

u8 MMIO::readByte(u32 addr)
{
    switch (addr)
    {
    case REG_BG0HOFS: 
    case REG_BG0HOFS+1:
    case REG_BG0VOFS: 
    case REG_BG0VOFS+1:
    case REG_BG1HOFS: 
    case REG_BG1HOFS+1:
    case REG_BG1VOFS: 
    case REG_BG1VOFS+1:
    case REG_BG2HOFS: 
    case REG_BG2HOFS+1:
    case REG_BG2VOFS: 
    case REG_BG2VOFS+1:
    case REG_BG3HOFS: 
    case REG_BG3HOFS+1:
    case REG_BG3VOFS: 
    case REG_BG3VOFS+1:
    case REG_BG2PA: 
    case REG_BG2PA+1: 
    case REG_BG2PB: 
    case REG_BG2PB+1: 
    case REG_BG2PC: 
    case REG_BG2PC+1: 
    case REG_BG2PD: 
    case REG_BG2PD+1:
    case REG_BG3PA: 
    case REG_BG3PA+1: 
    case REG_BG3PB: 
    case REG_BG3PB+1:
    case REG_BG3PC: 
    case REG_BG3PC+1: 
    case REG_BG3PD: 
    case REG_BG3PD+1:
    case REG_BG2X: 
    case REG_BG2X+1: 
    case REG_BG2X+2: 
    case REG_BG2X+3:
    case REG_BG2Y: 
    case REG_BG2Y+1: 
    case REG_BG2Y+2: 
    case REG_BG2Y+3:
    case REG_BG3X: 
    case REG_BG3X+1: 
    case REG_BG3X+2: 
    case REG_BG3X+3:
    case REG_BG3Y: 
    case REG_BG3Y+1: 
    case REG_BG3Y+2: 
    case REG_BG3Y+3:
    case REG_WIN0H: 
    case REG_WIN0H+1: 
    case REG_WIN1H: 
    case REG_WIN1H+1:
    case REG_WIN0V: 
    case REG_WIN0V+1:
    case REG_WIN1V: 
    case REG_WIN1V+1:
    case REG_MOSAIC: 
    case REG_MOSAIC+1: 
    case REG_BLDY:
    case REG_BLDY+1:
    case REG_DMA0SAD: 
    case REG_DMA0SAD+1: 
    case REG_DMA0SAD+2: 
    case REG_DMA0SAD+3:
    case REG_DMA1SAD: 
    case REG_DMA1SAD+1:
    case REG_DMA1SAD+2: 
    case REG_DMA1SAD+3:
    case REG_DMA2SAD: 
    case REG_DMA2SAD+1: 
    case REG_DMA2SAD+2: 
    case REG_DMA2SAD+3:
    case REG_DMA3SAD: 
    case REG_DMA3SAD+1: 
    case REG_DMA3SAD+2: 
    case REG_DMA3SAD+3:
    case REG_DMA0DAD: 
    case REG_DMA0DAD+1: 
    case REG_DMA0DAD+2: 
    case REG_DMA0DAD+3:
    case REG_DMA1DAD: 
    case REG_DMA1DAD+1: 
    case REG_DMA1DAD+2: 
    case REG_DMA1DAD+3:
    case REG_DMA2DAD: 
    case REG_DMA2DAD+1: 
    case REG_DMA2DAD+2: 
    case REG_DMA2DAD+3:
    case REG_DMA3DAD: 
    case REG_DMA3DAD+1: 
    case REG_DMA3DAD+2: 
    case REG_DMA3DAD+3:
    case REG_DMA0CNT_L:
    case REG_DMA0CNT_L+1:
    case REG_DMA1CNT_L:
    case REG_DMA1CNT_L+1:
    case REG_DMA2CNT_L:
    case REG_DMA2CNT_L+1:
    case REG_DMA3CNT_L:
    case REG_DMA3CNT_L+1:
    case REG_HALTCNT:
        return 0;

    case REG_DISPSTAT+0: return dispstat.read<0>();
    case REG_DISPSTAT+1: return dispstat.read<1>();

    case REG_VCOUNT+0: return vcount;
    case REG_VCOUNT+1: return 0;

    case REG_DMA0CNT_H+0: return dma.control[0].read<2>();
    case REG_DMA0CNT_H+1: return dma.control[0].read<3>();
    case REG_DMA1CNT_H+0: return dma.control[1].read<2>();
    case REG_DMA1CNT_H+1: return dma.control[1].read<3>();
    case REG_DMA2CNT_H+0: return dma.control[2].read<2>();
    case REG_DMA2CNT_H+1: return dma.control[2].read<3>();
    case REG_DMA3CNT_H+0: return dma.control[3].read<2>();
    case REG_DMA3CNT_H+1: return dma.control[3].read<3>();

    case REG_TM0CNT_L+0: return tmcnt[0].read<0>();
    case REG_TM0CNT_L+1: return tmcnt[0].read<1>();
    case REG_TM1CNT_L+0: return tmcnt[1].read<0>();
    case REG_TM1CNT_L+1: return tmcnt[1].read<1>();
    case REG_TM2CNT_L+0: return tmcnt[2].read<0>();
    case REG_TM2CNT_L+1: return tmcnt[2].read<1>();
    case REG_TM3CNT_L+0: return tmcnt[3].read<0>();
    case REG_TM3CNT_L+1: return tmcnt[3].read<1>();

    case REG_KEYINPUT+0: return bits<0, 8>(keyinput);
    case REG_KEYINPUT+1: return bits<8, 8>(keyinput);

    case REG_IF+0: return bits<0, 8>(intr_request);
    case REG_IF+1: return bits<8, 8>(intr_request);
    }
    return data[addr];
}

u16 MMIO::readHalf(u32 addr)
{
    return readByte(addr) | readByte(addr + 1) << 8;
}

u32 MMIO::readWord(u32 addr)
{
    return readHalf(addr) | readHalf(addr + 2) << 16;
}

void MMIO::writeByte(u32 addr, u8 byte)
{
    switch (addr)
    {
    case REG_VCOUNT:
    case REG_VCOUNT+1:
    case REG_KEYINPUT:
    case REG_KEYINPUT+1:
        return;

    case REG_DISPCNT+0: dispcnt.write<0>(byte); break;
    case REG_DISPCNT+1: dispcnt.write<1>(byte); break;

    case REG_DISPSTAT+0: dispstat.write<0>(byte); break;
    case REG_DISPSTAT+1: dispstat.write<1>(byte); break;

    case REG_BG0CNT+0: bgcnt[0].write<0>(byte); break;
    case REG_BG0CNT+1: bgcnt[0].write<1>(byte); break;
    case REG_BG1CNT+0: bgcnt[1].write<0>(byte); break;
    case REG_BG1CNT+1: bgcnt[1].write<1>(byte); break;
    case REG_BG2CNT+0: bgcnt[2].write<0>(byte); break;
    case REG_BG2CNT+1: bgcnt[2].write<1>(byte); break;
    case REG_BG3CNT+0: bgcnt[3].write<0>(byte); break;
    case REG_BG3CNT+1: bgcnt[3].write<1>(byte); break;

    case REG_BG0HOFS+0: bghofs[0].write<0>(byte); break;
    case REG_BG0HOFS+1: bghofs[0].write<1>(byte); break;
    case REG_BG0VOFS+0: bgvofs[0].write<0>(byte); break;
    case REG_BG0VOFS+1: bgvofs[0].write<1>(byte); break;
    case REG_BG1HOFS+0: bghofs[1].write<0>(byte); break;
    case REG_BG1HOFS+1: bghofs[1].write<1>(byte); break;
    case REG_BG1VOFS+0: bgvofs[1].write<0>(byte); break;
    case REG_BG1VOFS+1: bgvofs[1].write<1>(byte); break;
    case REG_BG2HOFS+0: bghofs[2].write<0>(byte); break;
    case REG_BG2HOFS+1: bghofs[2].write<1>(byte); break;
    case REG_BG2VOFS+0: bgvofs[2].write<0>(byte); break;
    case REG_BG2VOFS+1: bgvofs[2].write<1>(byte); break;
    case REG_BG3HOFS+0: bghofs[3].write<0>(byte); break;
    case REG_BG3HOFS+1: bghofs[3].write<1>(byte); break;
    case REG_BG3VOFS+0: bgvofs[3].write<0>(byte); break;
    case REG_BG3VOFS+1: bgvofs[3].write<1>(byte); break;

    case REG_BG2PA+0: bgpa[0].write<0>(byte); break;
    case REG_BG2PA+1: bgpa[0].write<1>(byte); break;
    case REG_BG3PA+0: bgpa[1].write<0>(byte); break;
    case REG_BG3PA+1: bgpa[1].write<1>(byte); break;
    case REG_BG2PB+0: bgpb[0].write<0>(byte); break;
    case REG_BG2PB+1: bgpb[0].write<1>(byte); break;
    case REG_BG3PB+0: bgpb[1].write<0>(byte); break;
    case REG_BG3PB+1: bgpb[1].write<1>(byte); break;
    case REG_BG2PC+0: bgpc[0].write<0>(byte); break;
    case REG_BG2PC+1: bgpc[0].write<1>(byte); break;
    case REG_BG3PC+0: bgpc[1].write<0>(byte); break;
    case REG_BG3PC+1: bgpc[1].write<1>(byte); break;
    case REG_BG2PD+0: bgpd[0].write<0>(byte); break;
    case REG_BG2PD+1: bgpd[0].write<1>(byte); break;
    case REG_BG3PD+0: bgpd[1].write<0>(byte); break;
    case REG_BG3PD+1: bgpd[1].write<1>(byte); break;

    case REG_BG2X+0: bgx[0].write<0>(byte); break;
    case REG_BG2X+1: bgx[0].write<1>(byte); break;
    case REG_BG2X+2: bgx[0].write<2>(byte); break;
    case REG_BG2X+3: bgx[0].write<3>(byte); break;
    case REG_BG3X+0: bgx[1].write<0>(byte); break;
    case REG_BG3X+1: bgx[1].write<1>(byte); break;
    case REG_BG3X+2: bgx[1].write<2>(byte); break;
    case REG_BG3X+3: bgx[1].write<3>(byte); break;
    case REG_BG2Y+0: bgy[0].write<0>(byte); break;
    case REG_BG2Y+1: bgy[0].write<1>(byte); break;
    case REG_BG2Y+2: bgy[0].write<2>(byte); break;
    case REG_BG2Y+3: bgy[0].write<3>(byte); break;
    case REG_BG3Y+0: bgy[1].write<0>(byte); break;
    case REG_BG3Y+1: bgy[1].write<1>(byte); break;
    case REG_BG3Y+2: bgy[1].write<2>(byte); break;
    case REG_BG3Y+3: bgy[1].write<3>(byte); break;

    case REG_WIN0H+0: winh[0].write<0>(byte); break;
    case REG_WIN0H+1: winh[0].write<1>(byte); break;
    case REG_WIN1H+0: winh[1].write<0>(byte); break;
    case REG_WIN1H+1: winh[1].write<1>(byte); break;
    case REG_WIN0V+0: winv[0].write<0>(byte); break;
    case REG_WIN0V+1: winv[0].write<1>(byte); break;
    case REG_WIN1V+0: winv[1].write<0>(byte); break;
    case REG_WIN1V+1: winv[1].write<1>(byte); break;

    case REG_WININ+0: winin.win0.write(byte); break;
    case REG_WININ+1: winin.win1.write(byte); break;

    case REG_WINOUT+0: winout.winout.write(byte); break;
    case REG_WINOUT+1: winout.winobj.write(byte); break;

    case REG_MOSAIC+0: mosaic.write<0>(byte); break;
    case REG_MOSAIC+1: mosaic.write<1>(byte); break;

    case REG_BLDCNT+0: bldcnt.write<0>(byte); break;
    case REG_BLDCNT+1: bldcnt.write<1>(byte); break;

    case REG_BLDALPHA+0: bldalpha.write<0>(byte); break;
    case REG_BLDALPHA+1: bldalpha.write<1>(byte); break;

    case REG_BLDY: 
        bldy.write(byte); 
        break;

    case REG_DMA0SAD+0: dma.sad[0].write<0>(byte); break;
    case REG_DMA0SAD+1: dma.sad[0].write<1>(byte); break;
    case REG_DMA0SAD+2: dma.sad[0].write<2>(byte); break;
    case REG_DMA0SAD+3: dma.sad[0].write<3>(byte); break;
    case REG_DMA1SAD+0: dma.sad[1].write<0>(byte); break;
    case REG_DMA1SAD+1: dma.sad[1].write<1>(byte); break;
    case REG_DMA1SAD+2: dma.sad[1].write<2>(byte); break;
    case REG_DMA1SAD+3: dma.sad[1].write<3>(byte); break;
    case REG_DMA2SAD+0: dma.sad[2].write<0>(byte); break;
    case REG_DMA2SAD+1: dma.sad[2].write<1>(byte); break;
    case REG_DMA2SAD+2: dma.sad[2].write<2>(byte); break;
    case REG_DMA2SAD+3: dma.sad[2].write<3>(byte); break;
    case REG_DMA3SAD+0: dma.sad[3].write<0>(byte); break;
    case REG_DMA3SAD+1: dma.sad[3].write<1>(byte); break;
    case REG_DMA3SAD+2: dma.sad[3].write<2>(byte); break;
    case REG_DMA3SAD+3: dma.sad[3].write<3>(byte); break;

    case REG_DMA0DAD+0: dma.dad[0].write<0>(byte); break;
    case REG_DMA0DAD+1: dma.dad[0].write<1>(byte); break;
    case REG_DMA0DAD+2: dma.dad[0].write<2>(byte); break;
    case REG_DMA0DAD+3: dma.dad[0].write<3>(byte); break;
    case REG_DMA1DAD+0: dma.dad[1].write<0>(byte); break;
    case REG_DMA1DAD+1: dma.dad[1].write<1>(byte); break;
    case REG_DMA1DAD+2: dma.dad[1].write<2>(byte); break;
    case REG_DMA1DAD+3: dma.dad[1].write<3>(byte); break;
    case REG_DMA2DAD+0: dma.dad[2].write<0>(byte); break;
    case REG_DMA2DAD+1: dma.dad[2].write<1>(byte); break;
    case REG_DMA2DAD+2: dma.dad[2].write<2>(byte); break;
    case REG_DMA2DAD+3: dma.dad[2].write<3>(byte); break;
    case REG_DMA3DAD+0: dma.dad[3].write<0>(byte); break;
    case REG_DMA3DAD+1: dma.dad[3].write<1>(byte); break;
    case REG_DMA3DAD+2: dma.dad[3].write<2>(byte); break;
    case REG_DMA3DAD+3: dma.dad[3].write<3>(byte); break;

    case REG_DMA0CNT_L+0: dma.control[0].write<0>(byte); break;
    case REG_DMA0CNT_L+1: dma.control[0].write<1>(byte); break;
    case REG_DMA0CNT_H+0: dma.control[0].write<2>(byte); break;
    case REG_DMA0CNT_H+1: dma.control[0].write<3>(byte); break;
    case REG_DMA1CNT_L+0: dma.control[1].write<0>(byte); break;
    case REG_DMA1CNT_L+1: dma.control[1].write<1>(byte); break;
    case REG_DMA1CNT_H+0: dma.control[1].write<2>(byte); break;
    case REG_DMA1CNT_H+1: dma.control[1].write<3>(byte); break;
    case REG_DMA2CNT_L+0: dma.control[2].write<0>(byte); break;
    case REG_DMA2CNT_L+1: dma.control[2].write<1>(byte); break;
    case REG_DMA2CNT_H+0: dma.control[2].write<2>(byte); break;
    case REG_DMA2CNT_H+1: dma.control[2].write<3>(byte); break;
    case REG_DMA3CNT_L+0: dma.control[3].write<0>(byte); break;
    case REG_DMA3CNT_L+1: dma.control[3].write<1>(byte); break;
    case REG_DMA3CNT_H+0: dma.control[3].write<2>(byte); break;
    case REG_DMA3CNT_H+1: dma.control[3].write<3>(byte); break;

    case REG_TM0CNT_L+0: tmcnt[0].write<0>(byte); break;
    case REG_TM0CNT_L+1: tmcnt[0].write<1>(byte); break;
    case REG_TM0CNT_H+0: tmcnt[0].write<2>(byte); break;
    case REG_TM1CNT_L+0: tmcnt[1].write<0>(byte); break;
    case REG_TM1CNT_L+1: tmcnt[1].write<1>(byte); break;
    case REG_TM1CNT_H+0: tmcnt[1].write<2>(byte); break;
    case REG_TM2CNT_L+0: tmcnt[2].write<0>(byte); break;
    case REG_TM2CNT_L+1: tmcnt[2].write<1>(byte); break;
    case REG_TM2CNT_H+0: tmcnt[2].write<2>(byte); break;
    case REG_TM3CNT_L+0: tmcnt[3].write<0>(byte); break;
    case REG_TM3CNT_L+1: tmcnt[3].write<1>(byte); break;
    case REG_TM3CNT_H+0: tmcnt[3].write<2>(byte); break;

    case REG_KEYCNT+0: keycnt.write<0>(byte); break;
    case REG_KEYCNT+1: keycnt.write<1>(byte); break;

    case REG_WAITCNT+0: waitcnt.write<0>(byte); break;
    case REG_WAITCNT+1: waitcnt.write<1>(byte); break;

    case REG_IME:
        intr_master = bits<0, 1>(byte);
        break;

    case REG_IE:
        bytes(&intr_enabled)[0] = byte;
        break;

    case REG_IE+1:
        bytes(&intr_enabled)[1] = byte;
        break;

    case REG_IF:
        bytes(&intr_request)[0] &= ~byte;
        return;

    case REG_IF+1: 
        bytes(&intr_request)[1] &= ~byte;
        return;

    case REG_HALTCNT:
        halt = true;
        break;
    }
    data[addr] = byte;
}

void MMIO::writeHalf(u32 addr, u16 half)
{
    writeByte(addr + 0, (half >> 0) & 0xFF);
    writeByte(addr + 1, (half >> 8) & 0xFF);
}

void MMIO::writeWord(u32 addr, u32 word)
{
    writeWord(addr + 0, (word >>  0) & 0xFFFF);
    writeWord(addr + 2, (word >> 16) & 0xFFFF);
}
