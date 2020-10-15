#include "io.h"

#include "arm/arm.h"
#include "base/macros.h"
#include "dma/dma.h"
#include "gamepad/gamepad.h"
#include "mmu/constants.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "timer/timer.h"

Io::Io()
{
    if (config.bios_skip)
    {
        rcnt.value = 0x8000;
        postflag.value = 0x01;
        soundcnt_h.value = 0x880E;
        soundbias.value = 0x0200;
    }
}

u8 Io::readByte(u32 addr) const
{
    switch (addr & 0x3FF'FFFF)
    {
    INDEXED_CASE2(kMmioDispayControl , return ppu.io.dispcnt.read<kIndex>());
    INDEXED_CASE2(kMmioGreenSwap     , return ppu.io.greenswap.read<kIndex>());
    INDEXED_CASE2(kMmioDisplayStatus , return ppu.io.dispstat.read<kIndex>());
    INDEXED_CASE2(kMmioVerticalCount , return ppu.io.vcount.read<kIndex>());
    INDEXED_CASE2(kMmioBg0Control    , return ppu.io.bgcnt[0].read<kIndex>());
    INDEXED_CASE2(kMmioBg1Control    , return ppu.io.bgcnt[1].read<kIndex>());
    INDEXED_CASE2(kMmioBg2Control    , return ppu.io.bgcnt[2].read<kIndex>());
    INDEXED_CASE2(kMmioBg3Control    , return ppu.io.bgcnt[3].read<kIndex>());
    INDEXED_CASE2(kMmioWindowInside  , return ppu.io.winin.read<kIndex>());
    INDEXED_CASE2(kMmioWindowOutside , return ppu.io.winout.read<kIndex>());
    INDEXED_CASE2(kMmioBlendControl  , return ppu.io.bldcnt.read<kIndex>());
    INDEXED_CASE2(kMmioBlendAlpha    , return ppu.io.bldalpha.read<kIndex>());
    INDEXED_CASE2(kMmioSound1ControlL, return soundcnt1_l.read<kIndex>());
    INDEXED_CASE2(kMmioSound1ControlH, return soundcnt1_h.read<kIndex>());
    INDEXED_CASE2(kMmioSound1ControlX, return soundcnt1_x.read<kIndex>());
    INDEXED_CASE2(kMmioUnused066     , return 0);
    INDEXED_CASE2(kMmioSound2ControlL, return soundcnt2_l.read<kIndex>());
    INDEXED_CASE2(kMmioSound2ControlH, return soundcnt2_h.read<kIndex>());
    INDEXED_CASE2(kMmioUnused06E     , return 0);
    INDEXED_CASE2(kMmioSound3ControlL, return soundcnt3_l.read<kIndex>());
    INDEXED_CASE2(kMmioSound3ControlH, return soundcnt3_h.read<kIndex>());
    INDEXED_CASE2(kMmioSound3ControlX, return soundcnt3_x.read<kIndex>());
    INDEXED_CASE2(kMmioUnused076     , return 0);
    INDEXED_CASE2(kMmioSound4ControlL, return soundcnt4_l.read<kIndex>());
    INDEXED_CASE2(kMmioUnused07A     , return 0);
    INDEXED_CASE2(kMmioSound4ControlH, return soundcnt4_h.read<kIndex>());
    INDEXED_CASE2(kMmioUnused07E     , return 0);
    INDEXED_CASE2(kMmioSoundControlL , return soundcnt_l.read<kIndex>());
    INDEXED_CASE2(kMmioSoundControlH , return soundcnt_h.read<kIndex>());
    INDEXED_CASE2(kMmioSoundControlX , return soundcnt_x.read<kIndex>());
    INDEXED_CASE2(kMmioUnused086     , return 0);
    INDEXED_CASE2(kMmioSoundBias     , return soundbias.read<kIndex>());
    INDEXED_CASE2(kMmioUnused08A     , return 0);
    INDEXED_CASE2(kMmioWaveRam0      , return waveram[0].read<kIndex>());
    INDEXED_CASE2(kMmioWaveRam1      , return waveram[1].read<kIndex>());
    INDEXED_CASE2(kMmioWaveRam2      , return waveram[2].read<kIndex>());
    INDEXED_CASE2(kMmioWaveRam3      , return waveram[3].read<kIndex>());
    INDEXED_CASE2(kMmioWaveRam4      , return waveram[4].read<kIndex>());
    INDEXED_CASE2(kMmioWaveRam5      , return waveram[5].read<kIndex>());
    INDEXED_CASE2(kMmioWaveRam6      , return waveram[6].read<kIndex>());
    INDEXED_CASE2(kMmioWaveRam7      , return waveram[7].read<kIndex>());
    INDEXED_CASE2(kMmioDma0Count     , return 0);
    INDEXED_CASE2(kMmioDma0Control   , return dma.channels[0].control.read<kIndex>());
    INDEXED_CASE2(kMmioDma1Count     , return 0);
    INDEXED_CASE2(kMmioDma1Control   , return dma.channels[1].control.read<kIndex>());
    INDEXED_CASE2(kMmioDma2Count     , return 0);
    INDEXED_CASE2(kMmioDma2Control   , return dma.channels[2].control.read<kIndex>());
    INDEXED_CASE2(kMmioDma3Count     , return 0);
    INDEXED_CASE2(kMmioDma3Control   , return dma.channels[3].control.read<kIndex>());
    INDEXED_CASE2(kMmioTimer0Count   , return timer.channels[0].count.read<kIndex>());
    INDEXED_CASE2(kMmioTimer0Control , return timer.channels[0].control.read<kIndex>());
    INDEXED_CASE2(kMmioTimer1Count   , return timer.channels[1].count.read<kIndex>());
    INDEXED_CASE2(kMmioTimer1Control , return timer.channels[1].control.read<kIndex>());
    INDEXED_CASE2(kMmioTimer2Count   , return timer.channels[2].count.read<kIndex>());
    INDEXED_CASE2(kMmioTimer2Control , return timer.channels[2].control.read<kIndex>());
    INDEXED_CASE2(kMmioTimer3Count   , return timer.channels[3].count.read<kIndex>());
    INDEXED_CASE2(kMmioTimer3Control , return timer.channels[3].control.read<kIndex>());
    INDEXED_CASE2(kMmioSioMulti0     , return siomulti[0].read<kIndex>());
    INDEXED_CASE2(kMmioSioMulti1     , return siomulti[1].read<kIndex>());
    INDEXED_CASE2(kMmioSioMulti2     , return siomulti[2].read<kIndex>());
    INDEXED_CASE2(kMmioSioMulti3     , return siomulti[3].read<kIndex>());
    INDEXED_CASE2(kMmioSioControl    , return siocnt.read<kIndex>());
    INDEXED_CASE2(kMmioSioSend       , return siosend.read<kIndex>());
    INDEXED_CASE2(kMmioKeyInput      , return gamepad.input.read<kIndex>());
    INDEXED_CASE2(kMmioKeyControl    , return gamepad.control.read<kIndex>());
    INDEXED_CASE2(kMmioRemoteControl , return rcnt.read<kIndex>());
    INDEXED_CASE2(kMmioUnused136     , return 0);
    INDEXED_CASE2(kMmioJoyControl    , return joycnt.read<kIndex>());
    INDEXED_CASE2(kMmioUnused142     , return 0);
    INDEXED_CASE4(kMmioJoyReceive    , return joyrecv.read<kIndex>());
    INDEXED_CASE4(kMmioJoyTransmit   , return joytrans.read<kIndex>());
    INDEXED_CASE2(kMmioJoyStatus     , return joystat.read<kIndex>());
    INDEXED_CASE2(kMmioUnused15A     , return 0);
    INDEXED_CASE2(kMmioIrqEnable     , return arm.irq.enable.read<kIndex>());
    INDEXED_CASE2(kMmioIrqRequest    , return arm.irq.request.read<kIndex>());
    INDEXED_CASE2(kMmioWaitControl   , return arm.waitcnt.read<kIndex>());
    INDEXED_CASE2(kMmioUnused206     , return 0);
    INDEXED_CASE4(kMmioIrqMaster     , return arm.irq.master.read<kIndex>());
    INDEXED_CASE1(kMmioPostFlag      , return postflag.read<kIndex>());
    }
    return mmu.readUnused(addr);
}

u16 Io::readHalf(u32 addr) const
{
    addr &= ~0x1;

    u16 value = 0;
    value |= readByte(addr + 0) << 0;
    value |= readByte(addr + 1) << 8;

    return value;
}

u32 Io::readWord(u32 addr) const
{
    addr &= ~0x3;

    u32 value = 0;
    value |= readByte(addr + 0) <<  0;
    value |= readByte(addr + 1) <<  8;
    value |= readByte(addr + 2) << 16;
    value |= readByte(addr + 3) << 24;

    return value;
}

void Io::writeByte(u32 addr, u8 byte)
{
    switch (addr & 0x3FF'FFFF)
    {
    INDEXED_CASE2(kMmioDispayControl , ppu.io.dispcnt.write<kIndex>(byte));
    INDEXED_CASE2(kMmioGreenSwap     , ppu.io.greenswap.write<kIndex>(byte));
    INDEXED_CASE2(kMmioDisplayStatus , ppu.io.dispstat.write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg0Control    , ppu.io.bgcnt[0].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kMmioBg1Control    , ppu.io.bgcnt[1].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kMmioBg2Control    , ppu.io.bgcnt[2].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kMmioBg3Control    , ppu.io.bgcnt[3].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kMmioBg0HorOffset  , ppu.io.bghofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg0VerOffset  , ppu.io.bgvofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg1HorOffset  , ppu.io.bghofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg1VerOffset  , ppu.io.bgvofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg2HorOffset  , ppu.io.bghofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg2VerOffset  , ppu.io.bgvofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg3HorOffset  , ppu.io.bghofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg3VerOffset  , ppu.io.bgvofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg2ParameterA , ppu.io.bgpa[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg2ParameterB , ppu.io.bgpb[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg2ParameterC , ppu.io.bgpc[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg2ParameterD , ppu.io.bgpd[0].write<kIndex>(byte));
    INDEXED_CASE4(kMmioBg2ReferenceX , ppu.io.bgx[0].write<kIndex>(byte));
    INDEXED_CASE4(kMmioBg2ReferenceY , ppu.io.bgy[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg3ParameterA , ppu.io.bgpa[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg3ParameterB , ppu.io.bgpb[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg3ParameterC , ppu.io.bgpc[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioBg3ParameterD , ppu.io.bgpd[1].write<kIndex>(byte));
    INDEXED_CASE4(kMmioBg3ReferenceX , ppu.io.bgx[1].write<kIndex>(byte));
    INDEXED_CASE4(kMmioBg3ReferenceY , ppu.io.bgy[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWindow0Hor    , ppu.io.winh[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWindow1Hor    , ppu.io.winh[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWindow0Ver    , ppu.io.winv[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWindow1Ver    , ppu.io.winv[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWindowInside  , ppu.io.winin.write<kIndex>(byte));
    INDEXED_CASE2(kMmioWindowOutside , ppu.io.winout.write<kIndex>(byte));
    INDEXED_CASE2(kMmioMosaic        , ppu.io.mosaic.write<kIndex>(byte));
    INDEXED_CASE2(kMmioBlendControl  , ppu.io.bldcnt.write<kIndex>(byte));
    INDEXED_CASE2(kMmioBlendAlpha    , ppu.io.bldalpha.write<kIndex>(byte));
    INDEXED_CASE2(kMmioBlendFade     , ppu.io.bldfade.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound1ControlL, soundcnt1_l.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound1ControlH, soundcnt1_h.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound1ControlX, soundcnt1_x.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound2ControlL, soundcnt2_l.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound2ControlH, soundcnt2_h.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound3ControlL, soundcnt3_l.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound3ControlH, soundcnt3_h.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound3ControlX, soundcnt3_x.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound4ControlL, soundcnt4_l.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSound4ControlH, soundcnt4_h.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSoundControlL , soundcnt_l.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSoundControlH , soundcnt_h.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSoundControlX , soundcnt_x.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSoundBias     , soundbias.write<kIndex>(byte));
    INDEXED_CASE2(kMmioWaveRam0      , waveram[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWaveRam1      , waveram[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWaveRam2      , waveram[2].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWaveRam3      , waveram[3].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWaveRam4      , waveram[4].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWaveRam5      , waveram[5].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWaveRam6      , waveram[6].write<kIndex>(byte));
    INDEXED_CASE2(kMmioWaveRam7      , waveram[7].write<kIndex>(byte));
    INDEXED_CASE4(kMmioFifoA         , fifo[0].write<kIndex>(byte));
    INDEXED_CASE4(kMmioFifoB         , fifo[1].write<kIndex>(byte));
    INDEXED_CASE4(kMmioDma0Sad       , dma.channels[0].sad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_CASE4(kMmioDma0Dad       , dma.channels[0].dad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_CASE2(kMmioDma0Count     , dma.channels[0].count.write<kIndex, 0x3FFF>(byte));
    INDEXED_CASE2(kMmioDma0Control   , dma.channels[0].control.write<kIndex, 0xF7E0>(byte));
    INDEXED_CASE4(kMmioDma1Sad       , dma.channels[1].sad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_CASE4(kMmioDma1Dad       , dma.channels[1].dad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_CASE2(kMmioDma1Count     , dma.channels[1].count.write<kIndex, 0x3FFF>(byte));
    INDEXED_CASE2(kMmioDma1Control   , dma.channels[1].control.write<kIndex, 0xF7E0>(byte));
    INDEXED_CASE4(kMmioDma2Sad       , dma.channels[2].sad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_CASE4(kMmioDma2Dad       , dma.channels[2].dad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_CASE2(kMmioDma2Count     , dma.channels[2].count.write<kIndex, 0x3FFF>(byte));
    INDEXED_CASE2(kMmioDma2Control   , dma.channels[2].control.write<kIndex, 0xF7E0>(byte));
    INDEXED_CASE4(kMmioDma3Sad       , dma.channels[3].sad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_CASE4(kMmioDma3Dad       , dma.channels[3].dad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_CASE2(kMmioDma3Count     , dma.channels[3].count.write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kMmioDma3Control   , dma.channels[3].control.write<kIndex, 0xFFE0>(byte));
    INDEXED_CASE2(kMmioTimer0Count   , timer.channels[0].count.write<kIndex>(byte));
    INDEXED_CASE1(kMmioTimer0Control , timer.channels[0].control.write<kIndex>(byte));
    INDEXED_CASE2(kMmioTimer1Count   , timer.channels[1].count.write<kIndex>(byte));
    INDEXED_CASE1(kMmioTimer1Control , timer.channels[1].control.write<kIndex>(byte));
    INDEXED_CASE2(kMmioTimer2Count   , timer.channels[2].count.write<kIndex>(byte));
    INDEXED_CASE1(kMmioTimer2Control , timer.channels[2].control.write<kIndex>(byte));
    INDEXED_CASE2(kMmioTimer3Count   , timer.channels[3].count.write<kIndex>(byte));
    INDEXED_CASE1(kMmioTimer3Control , timer.channels[3].control.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSioMulti0     , siomulti[0].write<kIndex>(byte));
    INDEXED_CASE2(kMmioSioMulti1     , siomulti[1].write<kIndex>(byte));
    INDEXED_CASE2(kMmioSioMulti2     , siomulti[2].write<kIndex>(byte));
    INDEXED_CASE2(kMmioSioMulti3     , siomulti[3].write<kIndex>(byte));
    INDEXED_CASE2(kMmioSioControl    , siocnt.write<kIndex>(byte));
    INDEXED_CASE2(kMmioSioSend       , siosend.write<kIndex>(byte));
    INDEXED_CASE2(kMmioKeyControl    , gamepad.control.write<kIndex>(byte));
    INDEXED_CASE2(kMmioRemoteControl , rcnt.write<kIndex>(byte));
    INDEXED_CASE2(kMmioJoyControl    , joycnt.write<kIndex>(byte));
    INDEXED_CASE4(kMmioJoyReceive    , joyrecv.write<kIndex>(byte));
    INDEXED_CASE4(kMmioJoyTransmit   , joytrans.write<kIndex>(byte));
    INDEXED_CASE2(kMmioJoyStatus     , joystat.write<kIndex>(byte));
    INDEXED_CASE2(kMmioIrqEnable     , arm.irq.enable.write<kIndex>(byte));
    INDEXED_CASE2(kMmioIrqRequest    , arm.irq.request.write<kIndex>(byte));
    INDEXED_CASE2(kMmioWaitControl   , arm.waitcnt.write<kIndex>(byte));
    INDEXED_CASE4(kMmioIrqMaster     , arm.irq.master.write<kIndex>(byte));
    INDEXED_CASE1(kMmioPostFlag      , postflag.write<kIndex>(byte));
    INDEXED_CASE1(kMmioHaltControl   , arm.haltcnt.write<kIndex>(byte));
    }
}

void Io::writeHalf(u32 addr, u16 half)
{
    addr &= ~0x1;

    writeByte(addr + 0, bit::seq<0, 8>(half));
    writeByte(addr + 1, bit::seq<8, 8>(half));
}

void Io::writeWord(u32 addr, u32 word)
{
    addr &= ~0x3;

    writeByte(addr + 0, bit::seq< 0, 8>(word));
    writeByte(addr + 1, bit::seq< 8, 8>(word));
    writeByte(addr + 2, bit::seq<16, 8>(word));
    writeByte(addr + 3, bit::seq<24, 8>(word));
}
