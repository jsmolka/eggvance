#include "io.h"

#include "core/core.h"

Io::Io(Core& core)
    : core(core)
{

}

u8 Io::readByte(u32 addr) const
{
    switch (addr & 0x3FF'FFFF)
    {
    INDEXED_CASE2(kRegDispayControl , return core.ppu.io.dispcnt.read<kIndex>());
    INDEXED_CASE2(kRegGreenSwap     , return core.ppu.io.greenswap.read<kIndex>());
    INDEXED_CASE2(kRegDisplayStatus , return core.ppu.io.dispstat.read<kIndex>());
    INDEXED_CASE2(kRegVerticalCount , return core.ppu.io.vcount.read<kIndex>());
    INDEXED_CASE2(kRegBg0Control    , return core.ppu.io.bgcnt[0].read<kIndex>());
    INDEXED_CASE2(kRegBg1Control    , return core.ppu.io.bgcnt[1].read<kIndex>());
    INDEXED_CASE2(kRegBg2Control    , return core.ppu.io.bgcnt[2].read<kIndex>());
    INDEXED_CASE2(kRegBg3Control    , return core.ppu.io.bgcnt[3].read<kIndex>());
    INDEXED_CASE2(kRegWindowInside  , return core.ppu.io.winin.read<kIndex>());
    INDEXED_CASE2(kRegWindowOutside , return core.ppu.io.winout.read<kIndex>());
    INDEXED_CASE2(kRegBlendControl  , return core.ppu.io.bldcnt.read<kIndex>());
    INDEXED_CASE2(kRegBlendAlpha    , return core.ppu.io.bldalpha.read<kIndex>());
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
    INDEXED_CASE2(kRegWaitControl   , return core.arm.io.waitcnt.read<kIndex>());
    INDEXED_CASE1(kRegPostFlag      , return unused.postflag.read<kIndex>());
    INDEXED_CASE2(kRegKeyInput      , return core.keypad.io.input.read<kIndex>());
    INDEXED_CASE2(kRegKeyControl    , return core.keypad.io.control.read<kIndex>());
    INDEXED_CASE2(kRegDma0Count     , return 0);
    INDEXED_CASE2(kRegDma0Control   , return core.dmac.read<kLabel>());
    INDEXED_CASE2(kRegDma1Count     , return 0);
    INDEXED_CASE2(kRegDma1Control   , return core.dmac.read<kLabel>());
    INDEXED_CASE2(kRegDma2Count     , return 0);
    INDEXED_CASE2(kRegDma2Control   , return core.dmac.read<kLabel>());
    INDEXED_CASE2(kRegDma3Count     , return 0);
    INDEXED_CASE2(kRegDma3Control   , return core.dmac.read<kLabel>());
    INDEXED_CASE2(kRegTimer0Count   , return core.timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer0Control , return core.timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer1Count   , return core.timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer1Control , return core.timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer2Count   , return core.timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer2Control , return core.timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer3Count   , return core.timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer3Control , return core.timerc.read<kLabel>());
    INDEXED_CASE2(kRegIrqEnable     , return core.irqh.read<kLabel>());
    INDEXED_CASE2(kRegIrqRequest    , return core.irqh.read<kLabel>());
    INDEXED_CASE4(kRegIrqMaster     , return core.irqh.read<kLabel>());

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
        return core.mmu.readUnused(addr);
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
    INDEXED_CASE2(kRegDispayControl , core.ppu.io.dispcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegGreenSwap     , core.ppu.io.greenswap.write<kIndex>(byte));
    INDEXED_CASE2(kRegDisplayStatus , core.ppu.io.dispstat.write<kIndex>(byte));
    INDEXED_CASE2(kRegBg0Control    , core.ppu.io.bgcnt[0].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kRegBg1Control    , core.ppu.io.bgcnt[1].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kRegBg2Control    , core.ppu.io.bgcnt[2].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kRegBg3Control    , core.ppu.io.bgcnt[3].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kRegBg0HorOffset  , core.ppu.io.bghofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg0VerOffset  , core.ppu.io.bgvofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg1HorOffset  , core.ppu.io.bghofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg1VerOffset  , core.ppu.io.bgvofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2HorOffset  , core.ppu.io.bghofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2VerOffset  , core.ppu.io.bgvofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3HorOffset  , core.ppu.io.bghofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3VerOffset  , core.ppu.io.bgvofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterA , core.ppu.io.bgpa[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterB , core.ppu.io.bgpb[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterC , core.ppu.io.bgpc[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterD , core.ppu.io.bgpd[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg2ReferenceX , core.ppu.io.bgx[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg2ReferenceY , core.ppu.io.bgy[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterA , core.ppu.io.bgpa[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterB , core.ppu.io.bgpb[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterC , core.ppu.io.bgpc[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterD , core.ppu.io.bgpd[1].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg3ReferenceX , core.ppu.io.bgx[1].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg3ReferenceY , core.ppu.io.bgy[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow0Hor    , core.ppu.io.winh[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow1Hor    , core.ppu.io.winh[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow0Ver    , core.ppu.io.winv[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow1Ver    , core.ppu.io.winv[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindowInside  , core.ppu.io.winin.write<kIndex>(byte));
    INDEXED_CASE2(kRegWindowOutside , core.ppu.io.winout.write<kIndex>(byte));
    INDEXED_CASE2(kRegMosaic        , core.ppu.io.mosaic.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendControl  , core.ppu.io.bldcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendAlpha    , core.ppu.io.bldalpha.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendFade     , core.ppu.io.bldfade.write<kIndex>(byte));
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
    INDEXED_CASE2(kRegWaitControl   , core.arm.io.waitcnt.write<kIndex>(byte));
    INDEXED_CASE1(kRegPostFlag      , unused.postflag.write<kIndex>(byte));
    INDEXED_CASE1(kRegHaltControl   , core.arm.io.haltcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegKeyControl    , core.keypad.io.control.write<kIndex>(byte));
    INDEXED_CASE4(kRegDma0Sad       , core.dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma0Dad       , core.dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma0Count     , core.dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma0Control   , core.dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma1Sad       , core.dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma1Dad       , core.dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma1Count     , core.dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma1Control   , core.dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma2Sad       , core.dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma2Dad       , core.dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma2Count     , core.dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma2Control   , core.dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma3Sad       , core.dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma3Dad       , core.dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma3Count     , core.dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma3Control   , core.dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer0Count   , core.timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer0Control , core.timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer1Count   , core.timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer1Control , core.timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer2Count   , core.timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer2Control , core.timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer3Count   , core.timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer3Control , core.timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegIrqEnable     , core.irqh.write<kLabel>(byte));
    INDEXED_CASE2(kRegIrqRequest    , core.irqh.write<kLabel>(byte));
    INDEXED_CASE4(kRegIrqMaster     , core.irqh.write<kLabel>(byte));
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
