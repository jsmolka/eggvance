#include "io.h"

#include "mmu.h"
#include "arm/arm.h"
#include "dma/dmac.h"
#include "irq/irqh.h"
#include "keypad/keypad.h"
#include "ppu/ppu.h"
#include "timer/timerc.h"

u8 Io::readByte(u32 addr) const
{
    u32 unmasked = addr;

    if (addr > 0x0400'0400)
    {
        if ((addr & 0xFFFC) == 0x800)
        {
            switch (addr & 0x3)
            {
            INDEXED_CASE4(0, io.memcontrol.read<kIndex>())

            default:
                UNREACHABLE;
                return 0;
            }
        }
        return mmu.readUnused(addr);
    }

    addr &= 0x3FF;

    switch (addr)
    {
    INDEXED_CASE2(kRegDispayControl, return ppu.io.dispcnt.read<kIndex>());
    INDEXED_CASE2(kRegGreenSwap    , return ppu.io.greenswap.read<kIndex>());
    INDEXED_CASE2(kRegDisplayStatus, return ppu.io.dispstat.read<kIndex>());
    INDEXED_CASE2(kRegVerticalCount, return ppu.io.vcount.read<kIndex>());
    INDEXED_CASE2(kRegBg0Control   , return ppu.io.bgcnt[0].read<kIndex>());
    INDEXED_CASE2(kRegBg1Control   , return ppu.io.bgcnt[1].read<kIndex>());
    INDEXED_CASE2(kRegBg2Control   , return ppu.io.bgcnt[2].read<kIndex>());
    INDEXED_CASE2(kRegBg3Control   , return ppu.io.bgcnt[3].read<kIndex>());
    INDEXED_CASE2(kRegWindowInside , return ppu.io.winin.read<kIndex>());
    INDEXED_CASE2(kRegWindowOutside, return ppu.io.winout.read<kIndex>());
    INDEXED_CASE2(kRegBlendControl , return ppu.io.bldcnt.read<kIndex>());
    INDEXED_CASE2(kRegBlendAlpha   , return ppu.io.bldalpha.read<kIndex>());

    READ_HALF_REG(REG_SOUND1CNT_L, io.soundcnt1_l   );
    READ_HALF_REG(REG_SOUND1CNT_H, io.soundcnt1_h   );
    READ_HALF_REG(REG_SOUND1CNT_X, io.soundcnt1_x   );
    READ_HALF_REG(REG_SOUND2CNT_L, io.soundcnt2_l   );
    READ_HALF_REG(REG_SOUND2CNT_H, io.soundcnt2_h   );
    READ_HALF_REG(REG_SOUND3CNT_L, io.soundcnt3_l   );
    READ_HALF_REG(REG_SOUND3CNT_H, io.soundcnt3_h   );
    READ_HALF_REG(REG_SOUND3CNT_X, io.soundcnt3_x   );
    READ_HALF_REG(REG_SOUND4CNT_L, io.soundcnt4_l   );
    READ_HALF_REG(REG_SOUND4CNT_H, io.soundcnt4_h   );
    READ_HALF_REG(REG_SOUNDCNT_L , io.soundcnt_l    );
    READ_HALF_REG(REG_SOUNDCNT_H , io.soundcnt_h    );
    READ_HALF_REG(REG_SOUNDCNT_X , io.soundcnt_x    );
    READ_HALF_REG(REG_SOUNDBIAS  , io.soundbias     );
    READ_HALF_REG(REG_WAVE_RAM_0 , io.waveram[0]    );
    READ_HALF_REG(REG_WAVE_RAM_1 , io.waveram[1]    );
    READ_HALF_REG(REG_WAVE_RAM_2 , io.waveram[2]    );
    READ_HALF_REG(REG_WAVE_RAM_3 , io.waveram[3]    );
    READ_HALF_REG(REG_WAVE_RAM_4 , io.waveram[4]    );
    READ_HALF_REG(REG_WAVE_RAM_5 , io.waveram[5]    );
    READ_HALF_REG(REG_WAVE_RAM_6 , io.waveram[6]    );
    READ_HALF_REG(REG_WAVE_RAM_7 , io.waveram[7]    );
    READ_HALF_REG(REG_SIOMULTI0  , io.siomulti[0]   );
    READ_HALF_REG(REG_SIOMULTI1  , io.siomulti[1]   );
    READ_HALF_REG(REG_SIOMULTI2  , io.siomulti[2]   );
    READ_HALF_REG(REG_SIOMULTI3  , io.siomulti[3]   );
    READ_HALF_REG(REG_SIOCNT     , io.siocnt        );
    READ_HALF_REG(REG_SIOSEND    , io.siosend       );
    READ_HALF_REG(REG_RCNT       , io.rcnt          );
    READ_HALF_REG(REG_JOYCNT     , io.joycnt        );
    READ_WORD_REG(REG_JOY_RECV   , io.joyrecv       );
    READ_WORD_REG(REG_JOY_TRANS  , io.joytrans      );
    READ_HALF_REG(REG_JOYSTAT    , io.joystat       );
    READ_HALF_REG(REG_WAITCNT    , arm.io.waitcnt   );
    READ_BYTE_REG(REG_POSTFLG    , io.postflag      );

    INDEXED_CASE2(kRegKeyInput     , return keypad.io.input.read<kIndex>());
    INDEXED_CASE2(kRegKeyControl   , return keypad.io.control.read<kIndex>());
    INDEXED_CASE2(kRegDma0Control  , return dmac.read<kLabel>());
    INDEXED_CASE2(kRegDma1Control  , return dmac.read<kLabel>());
    INDEXED_CASE2(kRegDma2Control  , return dmac.read<kLabel>());
    INDEXED_CASE2(kRegDma3Control  , return dmac.read<kLabel>());
    INDEXED_CASE2(kRegTimer0Count  , return timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer0Control, return timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer1Count  , return timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer1Control, return timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer2Count  , return timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer2Control, return timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer3Count  , return timerc.read<kLabel>());
    INDEXED_CASE2(kRegTimer3Control, return timerc.read<kLabel>());
    INDEXED_CASE2(kRegIrqEnable    , return irqh.read<kLabel>());
    INDEXED_CASE2(kRegIrqRequest   , return irqh.read<kLabel>());
    INDEXED_CASE4(kRegIrqMaster    , return irqh.read<kLabel>());

    INDEXED_CASE2(0x066, return 0);
    INDEXED_CASE2(0x06E, return 0);
    INDEXED_CASE2(0x076, return 0);
    INDEXED_CASE2(0x07A, return 0);
    INDEXED_CASE2(0x07E, return 0);
    INDEXED_CASE2(0x086, return 0);
    INDEXED_CASE2(0x08A, return 0);
    INDEXED_CASE2(0x136, return 0);
    INDEXED_CASE2(0x142, return 0);
    INDEXED_CASE2(0x15A, return 0);
    INDEXED_CASE2(0x206, return 0);

    default:
        return mmu.readUnused(unmasked);
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
    if (addr > 0x0400'0400)
    {
        if ((addr & 0xFFFC) == 0x800)
        {
            switch (addr & 0x3)
            {
            case 0: io.memcontrol.write<0>(byte); break;
            case 1: io.memcontrol.write<1>(byte); break;
            case 2: io.memcontrol.write<2>(byte); break;
            case 3: io.memcontrol.write<3>(byte); break;

            default:
                UNREACHABLE;
                break;
            }
        }
        return;
    }

    addr &= 0x3FF;

    switch (addr)
    {
    INDEXED_CASE2(kRegDispayControl, ppu.io.dispcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegGreenSwap    , ppu.io.greenswap.write<kIndex>(byte));
    INDEXED_CASE2(kRegDisplayStatus, ppu.io.dispstat.write<kIndex>(byte));
    INDEXED_CASE2(kRegBg0Control   , ppu.io.bgcnt[0].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kRegBg1Control   , ppu.io.bgcnt[1].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kRegBg2Control   , ppu.io.bgcnt[2].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kRegBg3Control   , ppu.io.bgcnt[3].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kRegBg0HorOffset , ppu.io.bghofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg0VerOffset , ppu.io.bgvofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg1HorOffset , ppu.io.bghofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg1VerOffset , ppu.io.bgvofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2HorOffset , ppu.io.bghofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2VerOffset , ppu.io.bgvofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3HorOffset , ppu.io.bghofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3VerOffset , ppu.io.bgvofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterA, ppu.io.bgpa[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterB, ppu.io.bgpb[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterC, ppu.io.bgpc[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterD, ppu.io.bgpd[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg2ReferenceX, ppu.io.bgx[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg2ReferenceY, ppu.io.bgy[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterA, ppu.io.bgpa[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterB, ppu.io.bgpb[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterC, ppu.io.bgpc[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterD, ppu.io.bgpd[1].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg3ReferenceX, ppu.io.bgx[1].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg3ReferenceY, ppu.io.bgy[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow0Hor   , ppu.io.winh[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow1Hor   , ppu.io.winh[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow0Ver   , ppu.io.winv[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow1Ver   , ppu.io.winv[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindowInside , ppu.io.winin.write<kIndex>(byte));
    INDEXED_CASE2(kRegWindowOutside, ppu.io.winout.write<kIndex>(byte));
    INDEXED_CASE2(kRegMosaic       , ppu.io.mosaic.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendControl , ppu.io.bldcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendAlpha   , ppu.io.bldalpha.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendFade    , ppu.io.bldfade.write<kIndex>(byte));

    WRITE_HALF_REG(REG_SOUND1CNT_L, io.soundcnt1_l   , 0x0000'007F);
    WRITE_HALF_REG(REG_SOUND1CNT_H, io.soundcnt1_h   , 0x0000'FFC0);
    WRITE_HALF_REG(REG_SOUND1CNT_X, io.soundcnt1_x   , 0x0000'4000);
    WRITE_HALF_REG(REG_SOUND2CNT_L, io.soundcnt2_l   , 0x0000'FFC0);
    WRITE_HALF_REG(REG_SOUND2CNT_H, io.soundcnt2_h   , 0x0000'4000);
    WRITE_HALF_REG(REG_SOUND3CNT_L, io.soundcnt3_l   , 0x0000'00E0);
    WRITE_HALF_REG(REG_SOUND3CNT_H, io.soundcnt3_h   , 0x0000'E000);
    WRITE_HALF_REG(REG_SOUND3CNT_X, io.soundcnt3_x   , 0x0000'4000);
    WRITE_HALF_REG(REG_SOUND4CNT_L, io.soundcnt4_l   , 0x0000'FF00);
    WRITE_HALF_REG(REG_SOUND4CNT_H, io.soundcnt4_h   , 0x0000'40FF);
    WRITE_HALF_REG(REG_SOUNDCNT_L , io.soundcnt_l    , 0x0000'FF77);
    WRITE_HALF_REG(REG_SOUNDCNT_H , io.soundcnt_h    , 0x0000'770F);
    WRITE_HALF_REG(REG_SOUNDCNT_X , io.soundcnt_x    , 0x0000'0080);
    WRITE_HALF_REG(REG_SOUNDBIAS  , io.soundbias     , 0x0000'FFFF);
    WRITE_HALF_REG(REG_WAVE_RAM_0 , io.waveram[0]    , 0x0000'FFFF);
    WRITE_HALF_REG(REG_WAVE_RAM_1 , io.waveram[1]    , 0x0000'FFFF);
    WRITE_HALF_REG(REG_WAVE_RAM_2 , io.waveram[2]    , 0x0000'FFFF);
    WRITE_HALF_REG(REG_WAVE_RAM_3 , io.waveram[3]    , 0x0000'FFFF);
    WRITE_HALF_REG(REG_WAVE_RAM_4 , io.waveram[4]    , 0x0000'FFFF);
    WRITE_HALF_REG(REG_WAVE_RAM_5 , io.waveram[5]    , 0x0000'FFFF);
    WRITE_HALF_REG(REG_WAVE_RAM_6 , io.waveram[6]    , 0x0000'FFFF);
    WRITE_HALF_REG(REG_WAVE_RAM_7 , io.waveram[7]    , 0x0000'FFFF);
    WRITE_WORD_REG(REG_FIFO_A     , io.fifo[0]       , 0xFFFF'FFFF);
    WRITE_WORD_REG(REG_FIFO_B     , io.fifo[1]       , 0xFFFF'FFFF);
    WRITE_HALF_REG(REG_SIOMULTI0  , io.siomulti[0]   , 0x0000'FFFF);
    WRITE_HALF_REG(REG_SIOMULTI1  , io.siomulti[1]   , 0x0000'FFFF);
    WRITE_HALF_REG(REG_SIOMULTI2  , io.siomulti[2]   , 0x0000'FFFF);
    WRITE_HALF_REG(REG_SIOMULTI3  , io.siomulti[3]   , 0x0000'FFFF);
    WRITE_HALF_REG(REG_SIOCNT     , io.siocnt        , 0x0000'FFFF);
    WRITE_HALF_REG(REG_SIOSEND    , io.siosend       , 0x0000'FFFF);
    WRITE_HALF_REG(REG_RCNT       , io.rcnt          , 0x0000'FFFF);
    WRITE_HALF_REG(REG_JOYCNT     , io.joycnt        , 0x0000'FFFF);
    WRITE_WORD_REG(REG_JOY_RECV   , io.joyrecv       , 0xFFFF'FFFF);
    WRITE_WORD_REG(REG_JOY_TRANS  , io.joytrans      , 0xFFFF'FFFF);
    WRITE_HALF_REG(REG_JOYSTAT    , io.joystat       , 0x0000'FFFF);
    WRITE_HALF_REG(REG_WAITCNT    , arm.io.waitcnt   , 0x0000'FFFF);
    WRITE_BYTE_REG(REG_POSTFLG    , io.postflag      , 0x0000'00FF);
    WRITE_BYTE_REG(REG_HALTCNT    , arm.io.haltcnt   , 0x0000'00FF);

    INDEXED_CASE2(kRegKeyControl   , keypad.io.control.write<kIndex>(byte));
    INDEXED_CASE4(kRegDma0Sad      , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma0Dad      , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma0Count    , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma0Control  , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma1Sad      , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma1Dad      , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma1Count    , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma1Control  , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma2Sad      , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma2Dad      , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma2Count    , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma2Control  , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma3Sad      , dmac.write<kLabel>(byte));
    INDEXED_CASE4(kRegDma3Dad      , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma3Count    , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegDma3Control  , dmac.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer0Count  , timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer0Control, timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer1Count  , timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer1Control, timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer2Count  , timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer2Control, timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer3Count  , timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegTimer3Control, timerc.write<kLabel>(byte));
    INDEXED_CASE2(kRegIrqEnable    , irqh.write<kLabel>(byte));
    INDEXED_CASE2(kRegIrqRequest   , irqh.write<kLabel>(byte));
    INDEXED_CASE4(kRegIrqMaster    , irqh.write<kLabel>(byte));
    }
}

void Io::writeHalf(u32 addr, u16 half)
{
    addr &= ~0x1;

    writeByte(addr + 0, bits::seq<0, 8>(half));
    writeByte(addr + 1, bits::seq<8, 8>(half));
}

void Io::writeWord(u32 addr, u32 word)
{
    addr &= ~0x3;

    writeByte(addr + 0, bits::seq< 0, 8>(word));
    writeByte(addr + 1, bits::seq< 8, 8>(word));
    writeByte(addr + 2, bits::seq<16, 8>(word));
    writeByte(addr + 3, bits::seq<24, 8>(word));
}
