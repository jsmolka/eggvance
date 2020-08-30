#include "io.h"

#include "arm/arm.h"
#include "dma/dmac.h"
#include "irq/irqh.h"
#include "keypad/keypad.h"
#include "mmu/mmu.h"
#include "ppu/ppu.h"
#include "timer/timer.h"

u8 Io::readByte(u32 addr) const
{
    switch (addr & 0x3FF'FFFF)
    {
    INDEXED_CASE2(kRegDispayControl , return ppu.io.dispcnt.read<kIndex>());
    INDEXED_CASE2(kRegGreenSwap     , return ppu.io.greenswap.read<kIndex>());
    INDEXED_CASE2(kRegDisplayStatus , return ppu.io.dispstat.read<kIndex>());
    INDEXED_CASE2(kRegVerticalCount , return ppu.io.vcount.read<kIndex>());
    INDEXED_CASE2(kRegBg0Control    , return ppu.io.bgcnt[0].read<kIndex>());
    INDEXED_CASE2(kRegBg1Control    , return ppu.io.bgcnt[1].read<kIndex>());
    INDEXED_CASE2(kRegBg2Control    , return ppu.io.bgcnt[2].read<kIndex>());
    INDEXED_CASE2(kRegBg3Control    , return ppu.io.bgcnt[3].read<kIndex>());
    INDEXED_CASE2(kRegWindowInside  , return ppu.io.winin.read<kIndex>());
    INDEXED_CASE2(kRegWindowOutside , return ppu.io.winout.read<kIndex>());
    INDEXED_CASE2(kRegBlendControl  , return ppu.io.bldcnt.read<kIndex>());
    INDEXED_CASE2(kRegBlendAlpha    , return ppu.io.bldalpha.read<kIndex>());
    INDEXED_CASE2(kRegSound1ControlL, return unused.soundcnt1_l.read<kIndex>());
    INDEXED_CASE2(kRegSound1ControlH, return unused.soundcnt1_h.read<kIndex>());
    INDEXED_CASE2(kRegSound1ControlX, return unused.soundcnt1_x.read<kIndex>());
    INDEXED_CASE2(kRegSound2ControlL, return unused.soundcnt2_l.read<kIndex>());
    INDEXED_CASE2(kRegSound2ControlH, return unused.soundcnt2_h.read<kIndex>());
    INDEXED_CASE2(kRegSound3ControlL, return unused.soundcnt3_l.read<kIndex>());
    INDEXED_CASE2(kRegSound3ControlH, return unused.soundcnt3_h.read<kIndex>());
    INDEXED_CASE2(kRegSound3ControlX, return unused.soundcnt3_x.read<kIndex>());
    INDEXED_CASE2(kRegSound4ControlL, return unused.soundcnt4_l.read<kIndex>());
    INDEXED_CASE2(kRegSound4ControlH, return unused.soundcnt4_h.read<kIndex>());
    INDEXED_CASE2(kRegSoundControlL , return unused.soundcnt_l.read<kIndex>());
    INDEXED_CASE2(kRegSoundControlH , return unused.soundcnt_h.read<kIndex>());
    INDEXED_CASE2(kRegSoundControlX , return unused.soundcnt_x.read<kIndex>());
    INDEXED_CASE2(kRegSoundBias     , return unused.soundbias.read<kIndex>());
    INDEXED_CASE2(kRegWaveRam0      , return unused.waveram[0].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam1      , return unused.waveram[1].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam2      , return unused.waveram[2].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam3      , return unused.waveram[3].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam4      , return unused.waveram[4].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam5      , return unused.waveram[5].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam6      , return unused.waveram[6].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam7      , return unused.waveram[7].read<kIndex>());
    INDEXED_CASE2(kRegSioMulti0     , return unused.siomulti[0].read<kIndex>());
    INDEXED_CASE2(kRegSioMulti1     , return unused.siomulti[1].read<kIndex>());
    INDEXED_CASE2(kRegSioMulti2     , return unused.siomulti[2].read<kIndex>());
    INDEXED_CASE2(kRegSioMulti3     , return unused.siomulti[3].read<kIndex>());
    INDEXED_CASE2(kRegSioControl    , return unused.siocnt.read<kIndex>());
    INDEXED_CASE2(kRegSioSend       , return unused.siosend.read<kIndex>());
    INDEXED_CASE2(kRegRemoteControl , return unused.rcnt.read<kIndex>());
    INDEXED_CASE2(kRegJoyControl    , return unused.joycnt.read<kIndex>());
    INDEXED_CASE4(kRegJoyReceive    , return unused.joyrecv.read<kIndex>());
    INDEXED_CASE4(kRegJoyTransmit   , return unused.joytrans.read<kIndex>());
    INDEXED_CASE2(kRegJoyStatus     , return unused.joystat.read<kIndex>());
    INDEXED_CASE2(kRegWaitControl   , return arm.io.waitcnt.read<kIndex>());
    INDEXED_CASE1(kRegPostFlag      , return unused.postflag.read<kIndex>());
    INDEXED_CASE2(kRegKeyInput      , return keypad.io.input.read<kIndex>());
    INDEXED_CASE2(kRegKeyControl    , return keypad.io.control.read<kIndex>());
    INDEXED_CASE2(kRegDma0Count     , return 0);
    INDEXED_CASE2(kRegDma0Control   , return dmac.read<kLabel>());
    INDEXED_CASE2(kRegDma1Count     , return 0);
    INDEXED_CASE2(kRegDma1Control   , return dmac.read<kLabel>());
    INDEXED_CASE2(kRegDma2Count     , return 0);
    INDEXED_CASE2(kRegDma2Control   , return dmac.read<kLabel>());
    INDEXED_CASE2(kRegDma3Count     , return 0);
    INDEXED_CASE2(kRegDma3Control   , return dmac.read<kLabel>());
    INDEXED_CASE2(kRegTimer0Count   , return timer.channels[0].count.read<kIndex>());
    INDEXED_CASE2(kRegTimer0Control , return timer.channels[0].control.read<kIndex>());
    INDEXED_CASE2(kRegTimer1Count   , return timer.channels[1].count.read<kIndex>());
    INDEXED_CASE2(kRegTimer1Control , return timer.channels[1].control.read<kIndex>());
    INDEXED_CASE2(kRegTimer2Count   , return timer.channels[2].count.read<kIndex>());
    INDEXED_CASE2(kRegTimer2Control , return timer.channels[2].control.read<kIndex>());
    INDEXED_CASE2(kRegTimer3Count   , return timer.channels[3].count.read<kIndex>());
    INDEXED_CASE2(kRegTimer3Control , return timer.channels[3].control.read<kIndex>());
    INDEXED_CASE2(kRegIrqEnable     , return irqh.read<kLabel>());
    INDEXED_CASE2(kRegIrqRequest    , return irqh.read<kLabel>());
    INDEXED_CASE4(kRegIrqMaster     , return irqh.read<kLabel>());

    INDEXED_CASE2(kRegUnused066, return 0);
    INDEXED_CASE2(kRegUnused06E, return 0);
    INDEXED_CASE2(kRegUnused076, return 0);
    INDEXED_CASE2(kRegUnused07A, return 0);
    INDEXED_CASE2(kRegUnused07E, return 0);
    INDEXED_CASE2(kRegUnused086, return 0);
    INDEXED_CASE2(kRegUnused08A, return 0);
    INDEXED_CASE2(kRegUnused136, return 0);
    INDEXED_CASE2(kRegUnused142, return 0);
    INDEXED_CASE2(kRegUnused15A, return 0);
    INDEXED_CASE2(kRegUnused206, return 0);

    default:
        return mmu.readUnused(addr);
    }
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
    INDEXED_CASE2(kRegDispayControl , ppu.io.dispcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegGreenSwap     , ppu.io.greenswap.write<kIndex>(byte));
    INDEXED_CASE2(kRegDisplayStatus , ppu.io.dispstat.write<kIndex>(byte));
    INDEXED_CASE2(kRegBg0Control    , ppu.io.bgcnt[0].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kRegBg1Control    , ppu.io.bgcnt[1].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kRegBg2Control    , ppu.io.bgcnt[2].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kRegBg3Control    , ppu.io.bgcnt[3].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kRegBg0HorOffset  , ppu.io.bghofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg0VerOffset  , ppu.io.bgvofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg1HorOffset  , ppu.io.bghofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg1VerOffset  , ppu.io.bgvofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2HorOffset  , ppu.io.bghofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2VerOffset  , ppu.io.bgvofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3HorOffset  , ppu.io.bghofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3VerOffset  , ppu.io.bgvofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterA , ppu.io.bgpa[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterB , ppu.io.bgpb[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterC , ppu.io.bgpc[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterD , ppu.io.bgpd[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg2ReferenceX , ppu.io.bgx[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg2ReferenceY , ppu.io.bgy[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterA , ppu.io.bgpa[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterB , ppu.io.bgpb[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterC , ppu.io.bgpc[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterD , ppu.io.bgpd[1].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg3ReferenceX , ppu.io.bgx[1].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg3ReferenceY , ppu.io.bgy[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow0Hor    , ppu.io.winh[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow1Hor    , ppu.io.winh[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow0Ver    , ppu.io.winv[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow1Ver    , ppu.io.winv[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindowInside  , ppu.io.winin.write<kIndex>(byte));
    INDEXED_CASE2(kRegWindowOutside , ppu.io.winout.write<kIndex>(byte));
    INDEXED_CASE2(kRegMosaic        , ppu.io.mosaic.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendControl  , ppu.io.bldcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendAlpha    , ppu.io.bldalpha.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendFade     , ppu.io.bldfade.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound1ControlL, unused.soundcnt1_l.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound1ControlH, unused.soundcnt1_h.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound1ControlX, unused.soundcnt1_x.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound2ControlL, unused.soundcnt2_l.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound2ControlH, unused.soundcnt2_h.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound3ControlL, unused.soundcnt3_l.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound3ControlH, unused.soundcnt3_h.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound3ControlX, unused.soundcnt3_x.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound4ControlL, unused.soundcnt4_l.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound4ControlH, unused.soundcnt4_h.write<kIndex>(byte));
    INDEXED_CASE2(kRegSoundControlL , unused.soundcnt_l.write<kIndex>(byte));
    INDEXED_CASE2(kRegSoundControlH , unused.soundcnt_h.write<kIndex>(byte));
    INDEXED_CASE2(kRegSoundControlX , unused.soundcnt_x.write<kIndex>(byte));
    INDEXED_CASE2(kRegSoundBias     , unused.soundbias.write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam0      , unused.waveram[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam1      , unused.waveram[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam2      , unused.waveram[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam3      , unused.waveram[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam4      , unused.waveram[4].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam5      , unused.waveram[5].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam6      , unused.waveram[6].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam7      , unused.waveram[7].write<kIndex>(byte));
    INDEXED_CASE4(kRegFifoA         , unused.fifo[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegFifoB         , unused.fifo[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegSioMulti0     , unused.siomulti[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegSioMulti1     , unused.siomulti[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegSioMulti2     , unused.siomulti[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegSioMulti3     , unused.siomulti[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegSioControl    , unused.siocnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegSioSend       , unused.siosend.write<kIndex>(byte));
    INDEXED_CASE2(kRegRemoteControl , unused.rcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegJoyControl    , unused.joycnt.write<kIndex>(byte));
    INDEXED_CASE4(kRegJoyReceive    , unused.joyrecv.write<kIndex>(byte));
    INDEXED_CASE4(kRegJoyTransmit   , unused.joytrans.write<kIndex>(byte));
    INDEXED_CASE2(kRegJoyStatus     , unused.joystat.write<kIndex>(byte));
    INDEXED_CASE2(kRegWaitControl   , arm.io.waitcnt.write<kIndex>(byte));
    INDEXED_CASE1(kRegPostFlag      , unused.postflag.write<kIndex>(byte));
    INDEXED_CASE1(kRegHaltControl   , arm.io.haltcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegKeyControl    , keypad.io.control.write<kIndex>(byte));
    INDEXED_CASE4(kRegDma0Sad       , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma0Dad       , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma0Count     , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma0Control   , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma1Sad       , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma1Dad       , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma1Count     , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma1Control   , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma2Sad       , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma2Dad       , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma2Count     , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma2Control   , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma3Sad       , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma3Dad       , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma3Count     , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma3Control   , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer0Count   , timer.channels[0].count.write<kIndex>(byte));
    INDEXED_CASE1(kRegTimer0Control , timer.channels[0].control.write<kIndex>(byte));
    INDEXED_CASE2(kRegTimer1Count   , timer.channels[1].count.write<kIndex>(byte));
    INDEXED_CASE1(kRegTimer1Control , timer.channels[1].control.write<kIndex>(byte));
    INDEXED_CASE2(kRegTimer2Count   , timer.channels[2].count.write<kIndex>(byte));
    INDEXED_CASE1(kRegTimer2Control , timer.channels[2].control.write<kIndex>(byte));
    INDEXED_CASE2(kRegTimer3Count   , timer.channels[3].count.write<kIndex>(byte));
    INDEXED_CASE1(kRegTimer3Control , timer.channels[3].control.write<kIndex>(byte));
    INDEXED_CASE2(kRegIrqEnable     , irqh.write<kLabel>(byte));
    INDEXED_CASE2(kRegIrqRequest    , irqh.write<kLabel>(byte));
    INDEXED_CASE4(kRegIrqMaster     , irqh.write<kLabel>(byte));
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

Io::UnusedIo::UnusedIo()
{
    if (config.bios_skip)
    {
        rcnt.value = 0x8000;
        postflag.value = 0x01;
    }
}
