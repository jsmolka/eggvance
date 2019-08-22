#include "mmio.h"

#include "memory.h"

void MMIO::reset()
{
    data.fill(0);

    // Todo: default values
    // Todo: make sure all registers are reset
}

u8 MMIO::readByte(u32 addr)
{
    switch (addr)
    {
    case REG_DISPSTAT: 
        return dispstat.read(0);

    case REG_DISPSTAT+1: 
        return dispstat.read(1);

    case REG_VCOUNT: 
        return vcount;

    case REG_VCOUNT+1: 
        return 0;
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
        return;

    case REG_DISPCNT+0: dispcnt.write(0, byte); break;
    case REG_DISPCNT+1: dispcnt.write(1, byte); break;

    case REG_DISPSTAT+0: dispstat.write(0, byte); break;
    case REG_DISPSTAT+1: dispstat.write(1, byte); break;

    case REG_BG0CNT+0: bgcnt[0].write(0, byte); break;
    case REG_BG0CNT+1: bgcnt[0].write(1, byte); break;
    case REG_BG1CNT+0: bgcnt[1].write(0, byte); break;
    case REG_BG1CNT+1: bgcnt[1].write(1, byte); break;
    case REG_BG2CNT+0: bgcnt[2].write(0, byte); break;
    case REG_BG2CNT+1: bgcnt[2].write(1, byte); break;
    case REG_BG3CNT+0: bgcnt[3].write(0, byte); break;
    case REG_BG3CNT+1: bgcnt[3].write(1, byte); break;

    case REG_BG0HOFS+0: bghofs[0].write(0, byte); break;
    case REG_BG0HOFS+1: bghofs[0].write(1, byte); break;
    case REG_BG0VOFS+0: bgvofs[0].write(0, byte); break;
    case REG_BG0VOFS+1: bgvofs[0].write(1, byte); break;
    case REG_BG1HOFS+0: bghofs[1].write(0, byte); break;
    case REG_BG1HOFS+1: bghofs[1].write(1, byte); break;
    case REG_BG1VOFS+0: bgvofs[1].write(0, byte); break;
    case REG_BG1VOFS+1: bgvofs[1].write(1, byte); break;
    case REG_BG2HOFS+0: bghofs[2].write(0, byte); break;
    case REG_BG2HOFS+1: bghofs[2].write(1, byte); break;
    case REG_BG2VOFS+0: bgvofs[2].write(0, byte); break;
    case REG_BG2VOFS+1: bgvofs[2].write(1, byte); break;
    case REG_BG3HOFS+0: bghofs[3].write(0, byte); break;
    case REG_BG3HOFS+1: bghofs[3].write(1, byte); break;
    case REG_BG3VOFS+0: bgvofs[3].write(0, byte); break;
    case REG_BG3VOFS+1: bgvofs[3].write(1, byte); break;

    case REG_BG2X+0: bgx[0].write(0, byte); break;
    case REG_BG2X+1: bgx[0].write(1, byte); break;
    case REG_BG2X+2: bgx[0].write(2, byte); break;
    case REG_BG2X+3: bgx[0].write(3, byte); break;
    case REG_BG3X+0: bgx[1].write(0, byte); break;
    case REG_BG3X+1: bgx[1].write(1, byte); break;
    case REG_BG3X+2: bgx[1].write(2, byte); break;
    case REG_BG3X+3: bgx[1].write(3, byte); break;

    case REG_BG2Y+0: bgy[0].write(0, byte); break;
    case REG_BG2Y+1: bgy[0].write(1, byte); break;
    case REG_BG2Y+2: bgy[0].write(2, byte); break;
    case REG_BG2Y+3: bgy[0].write(3, byte); break;
    case REG_BG3Y+0: bgy[1].write(0, byte); break;
    case REG_BG3Y+1: bgy[1].write(1, byte); break;
    case REG_BG3Y+2: bgy[1].write(2, byte); break;
    case REG_BG3Y+3: bgy[1].write(3, byte); break;

    case REG_BG2PA+0: bgpa[0].write(0, byte); break;
    case REG_BG2PA+1: bgpa[0].write(1, byte); break;
    case REG_BG2PB+0: bgpb[0].write(0, byte); break;
    case REG_BG2PB+1: bgpb[0].write(1, byte); break;
    case REG_BG2PC+0: bgpc[0].write(0, byte); break;
    case REG_BG2PC+1: bgpc[0].write(1, byte); break;
    case REG_BG2PD+0: bgpd[0].write(0, byte); break;
    case REG_BG2PD+1: bgpd[0].write(1, byte); break;

    case REG_BG3PA+0: bgpa[1].write(0, byte); break;
    case REG_BG3PA+1: bgpa[1].write(1, byte); break;
    case REG_BG3PB+0: bgpb[1].write(0, byte); break;
    case REG_BG3PB+1: bgpb[1].write(1, byte); break;
    case REG_BG3PC+0: bgpc[1].write(0, byte); break;
    case REG_BG3PC+1: bgpc[1].write(1, byte); break;
    case REG_BG3PD+0: bgpd[1].write(0, byte); break;
    case REG_BG3PD+1: bgpd[1].write(1, byte); break;

    case REG_WIN0H+0: winh[0].write(0, byte); break;
    case REG_WIN0H+1: winh[0].write(1, byte); break;
    case REG_WIN0V+0: winv[0].write(0, byte); break;
    case REG_WIN0V+1: winv[0].write(1, byte); break;

    case REG_WIN1H+0: winh[1].write(0, byte); break;
    case REG_WIN1H+1: winh[1].write(1, byte); break;
    case REG_WIN1V+0: winv[1].write(0, byte); break;
    case REG_WIN1V+1: winv[1].write(1, byte); break;

    case REG_WININ+0: winin.win0.write(byte); break;
    case REG_WININ+1: winin.win1.write(byte); break;

    case REG_WINOUT+0: winout.winout.write(byte); break;
    case REG_WINOUT+1: winout.winobj.write(byte); break;

    case REG_MOSAIC+0: mosaic.write(0, byte); break;
    case REG_MOSAIC+1: mosaic.write(1, byte); break;

    case REG_BLDALPHA+0: bldalpha.write(0, byte);
    case REG_BLDALPHA+1: bldalpha.write(1, byte);

    case REG_BLDY: bldy.write(byte); break;
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
