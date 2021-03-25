#include "arm.h"

#include <shell/macros.h>

#include "apu/apu.h"
#include "dma/dma.h"
#include "keypad/keypad.h"
#include "ppu/ppu.h"
#include "sio/sio.h"
#include "timer/timer.h"

enum IoRegister
{
    kRegDisplayControl = 0x000,
    kRegGreenSwap      = 0x002,
    kRegDisplayStatus  = 0x004,
    kRegVerticalCount  = 0x006,
    kRegBg0Control     = 0x008,
    kRegBg1Control     = 0x00A,
    kRegBg2Control     = 0x00C,
    kRegBg3Control     = 0x00E,
    kRegBg0HorOffset   = 0x010,
    kRegBg0VerOffset   = 0x012,
    kRegBg1HorOffset   = 0x014,
    kRegBg1VerOffset   = 0x016,
    kRegBg2HorOffset   = 0x018,
    kRegBg2VerOffset   = 0x01A,
    kRegBg3HorOffset   = 0x01C,
    kRegBg3VerOffset   = 0x01E,
    kRegBg2ParameterA  = 0x020,
    kRegBg2ParameterB  = 0x022,
    kRegBg2ParameterC  = 0x024,
    kRegBg2ParameterD  = 0x026,
    kRegBg2ReferenceX  = 0x028,
    kRegBg2ReferenceY  = 0x02C,
    kRegBg3ParameterA  = 0x030,
    kRegBg3ParameterB  = 0x032,
    kRegBg3ParameterC  = 0x034,
    kRegBg3ParameterD  = 0x036,
    kRegBg3ReferenceX  = 0x038,
    kRegBg3ReferenceY  = 0x03C,
    kRegWindow0Hor     = 0x040,
    kRegWindow1Hor     = 0x042,
    kRegWindow0Ver     = 0x044,
    kRegWindow1Ver     = 0x046,
    kRegWindowInside   = 0x048,
    kRegWindowOutside  = 0x04A,
    kRegMosaic         = 0x04C,
    kRegBlendControl   = 0x050,
    kRegBlendAlpha     = 0x052,
    kRegBlendFade      = 0x054,
    kRegSound1ControlL = 0x060,
    kRegSound2ControlL = 0x068,
    kRegSound3ControlL = 0x070,
    kRegSound4ControlL = 0x078,
    kRegSoundControlL  = 0x080,
    kRegSoundBias      = 0x088,
    kRegUnused08A      = 0x08A,
    kRegWaveRam0       = 0x090,
    kRegWaveRam1       = 0x092,
    kRegWaveRam2       = 0x094,
    kRegWaveRam3       = 0x096,
    kRegWaveRam4       = 0x098,
    kRegWaveRam5       = 0x09A,
    kRegWaveRam6       = 0x09C,
    kRegWaveRam7       = 0x09E,
    kRegFifoA          = 0x0A0,
    kRegFifoB          = 0x0A4,
    kRegDma0Sad        = 0x0B0,
    kRegDma0Dad        = 0x0B4,
    kRegDma0Count      = 0x0B8,
    kRegDma0Control    = 0x0BA,
    kRegDma1Sad        = 0x0BC,
    kRegDma1Dad        = 0x0C0,
    kRegDma1Count      = 0x0C4,
    kRegDma1Control    = 0x0C6,
    kRegDma2Sad        = 0x0C8,
    kRegDma2Dad        = 0x0CC,
    kRegDma2Count      = 0x0D0,
    kRegDma2Control    = 0x0D2,
    kRegDma3Sad        = 0x0D4,
    kRegDma3Dad        = 0x0D8,
    kRegDma3Count      = 0x0DC,
    kRegDma3Control    = 0x0DE,
    kRegTimer0Count    = 0x100,
    kRegTimer0Control  = 0x102,
    kRegTimer1Count    = 0x104,
    kRegTimer1Control  = 0x106,
    kRegTimer2Count    = 0x108,
    kRegTimer2Control  = 0x10A,
    kRegTimer3Count    = 0x10C,
    kRegTimer3Control  = 0x10E,
    kRegSioData32      = 0x120,
    kRegSioMulti0      = 0x120,
    kRegSioMulti1      = 0x122,
    kRegSioMulti2      = 0x124,
    kRegSioMulti3      = 0x126,
    kRegSioControl     = 0x128,
    kRegSioSend        = 0x12A,
    kRegSioData8       = 0x12A,
    kRegKeyInput       = 0x130,
    kRegKeyControl     = 0x132,
    kRegRemoteControl  = 0x134,
    kRegUnused136      = 0x136,
    kRegJoyControl     = 0x140,
    kRegUnused142      = 0x142,
    kRegJoyReceive     = 0x150,
    kRegJoyTransmit    = 0x154,
    kRegJoyStatus      = 0x158,
    kRegUnused15A      = 0x15A,
    kRegIrqEnable      = 0x200,
    kRegIrqRequest     = 0x202,
    kRegWaitControl    = 0x204,
    kRegUnused206      = 0x206,
    kRegIrqMaster      = 0x208,
    kRegPostFlag       = 0x300,
    kRegHaltControl    = 0x301
};

u8 Arm::readIo(u32 addr)
{
    switch (addr & 0x3FF'FFFF)
    {
    SHELL_CASE02(kRegDisplayControl, return ppu.dispcnt.read(kIndex));
    SHELL_CASE02(kRegGreenSwap,      return ppu.greenswap.read(kIndex));
    SHELL_CASE02(kRegDisplayStatus,  return ppu.dispstat.read(kIndex));
    SHELL_CASE02(kRegVerticalCount,  return ppu.vcount.read(kIndex));
    SHELL_CASE02(kRegBg0Control,     return ppu.bgcnt[0].read(kIndex));
    SHELL_CASE02(kRegBg1Control,     return ppu.bgcnt[1].read(kIndex));
    SHELL_CASE02(kRegBg2Control,     return ppu.bgcnt[2].read(kIndex));
    SHELL_CASE02(kRegBg3Control,     return ppu.bgcnt[3].read(kIndex));
    SHELL_CASE02(kRegWindowInside,   return ppu.winin.read(kIndex));
    SHELL_CASE02(kRegWindowOutside,  return ppu.winout.read(kIndex));
    SHELL_CASE02(kRegBlendControl,   return ppu.bldcnt.read(kIndex));
    SHELL_CASE02(kRegBlendAlpha,     return ppu.bldalpha.read(kIndex));
    SHELL_CASE08(kRegSound1ControlL, return apu.square1.read(kIndex));
    SHELL_CASE08(kRegSound2ControlL, return apu.square2.read(kIndex));
    SHELL_CASE08(kRegSound3ControlL, return apu.wave.read(kIndex));
    SHELL_CASE08(kRegSound4ControlL, return apu.noise.read(kIndex));
    SHELL_CASE08(kRegSoundControlL,  return apu.control.read(kIndex));
    SHELL_CASE02(kRegSoundBias,      return apu.bias.read(kIndex));
    SHELL_CASE02(kRegUnused08A,      return 0);
    SHELL_CASE16(kRegWaveRam0,       return apu.wave.ram.read(kIndex));
    SHELL_CASE02(kRegDma0Count,      return 0);
    SHELL_CASE02(kRegDma0Control,    return dma.channels[0].control.read(kIndex));
    SHELL_CASE02(kRegDma1Count,      return 0);
    SHELL_CASE02(kRegDma1Control,    return dma.channels[1].control.read(kIndex));
    SHELL_CASE02(kRegDma2Count,      return 0);
    SHELL_CASE02(kRegDma2Control,    return dma.channels[2].control.read(kIndex));
    SHELL_CASE02(kRegDma3Count,      return 0);
    SHELL_CASE02(kRegDma3Control,    return dma.channels[3].control.read(kIndex));
    SHELL_CASE02(kRegTimer0Count,    return timer.channels[0].count.read(kIndex));
    SHELL_CASE02(kRegTimer0Control,  return timer.channels[0].control.read(kIndex));
    SHELL_CASE02(kRegTimer1Count,    return timer.channels[1].count.read(kIndex));
    SHELL_CASE02(kRegTimer1Control,  return timer.channels[1].control.read(kIndex));
    SHELL_CASE02(kRegTimer2Count,    return timer.channels[2].count.read(kIndex));
    SHELL_CASE02(kRegTimer2Control,  return timer.channels[2].control.read(kIndex));
    SHELL_CASE02(kRegTimer3Count,    return timer.channels[3].count.read(kIndex));
    SHELL_CASE02(kRegTimer3Control,  return timer.channels[3].control.read(kIndex));
    SHELL_CASE02(kRegSioMulti0,      return sio.siomulti[0].read(kIndex));
    SHELL_CASE02(kRegSioMulti1,      return sio.siomulti[1].read(kIndex));
    SHELL_CASE02(kRegSioMulti2,      return sio.siomulti[2].read(kIndex));
    SHELL_CASE02(kRegSioMulti3,      return sio.siomulti[3].read(kIndex));
    SHELL_CASE02(kRegSioControl,     return sio.siocnt.read(kIndex));
    SHELL_CASE02(kRegSioSend,        return sio.siosend.read(kIndex));
    SHELL_CASE02(kRegKeyInput,       return keypad.input.read(kIndex));
    SHELL_CASE02(kRegKeyControl,     return keypad.control.read(kIndex));
    SHELL_CASE02(kRegRemoteControl,  return sio.rcnt.read(kIndex));
    SHELL_CASE02(kRegUnused136,      return 0);
    SHELL_CASE02(kRegJoyControl,     return sio.joycnt.read(kIndex));
    SHELL_CASE02(kRegUnused142,      return 0);
    SHELL_CASE04(kRegJoyReceive,     return sio.joyrecv.read(kIndex));
    SHELL_CASE04(kRegJoyTransmit,    return sio.joytrans.read(kIndex));
    SHELL_CASE02(kRegJoyStatus,      return sio.joystat.read(kIndex));
    SHELL_CASE02(kRegUnused15A,      return 0);
    SHELL_CASE02(kRegIrqEnable,      return irq.enable.read(kIndex));
    SHELL_CASE02(kRegIrqRequest,     return irq.request.read(kIndex));
    SHELL_CASE02(kRegWaitControl,    return waitcnt.read(kIndex));
    SHELL_CASE02(kRegUnused206,      return 0);
    SHELL_CASE04(kRegIrqMaster,      return irq.master.read(kIndex));
    SHELL_CASE01(kRegPostFlag,       return postflg.read(kIndex));

    default:
        return readUnused() >> (8 * (addr & 0x3));
    }
}

void Arm::writeIo(u32 addr, u8 byte)
{
    switch (addr & 0x3FF'FFFF)
    {
    SHELL_CASE02(kRegDisplayControl, ppu.dispcnt.write(kIndex, byte));
    SHELL_CASE02(kRegGreenSwap,      ppu.greenswap.write(kIndex, byte));
    SHELL_CASE02(kRegDisplayStatus,  ppu.dispstat.write(kIndex, byte));
    SHELL_CASE02(kRegBg0Control,     ppu.bgcnt[0].write(kIndex, byte));
    SHELL_CASE02(kRegBg1Control,     ppu.bgcnt[1].write(kIndex, byte));
    SHELL_CASE02(kRegBg2Control,     ppu.bgcnt[2].write(kIndex, byte));
    SHELL_CASE02(kRegBg3Control,     ppu.bgcnt[3].write(kIndex, byte));
    SHELL_CASE02(kRegBg0HorOffset,   ppu.bghofs[0].write(kIndex, byte));
    SHELL_CASE02(kRegBg0VerOffset,   ppu.bgvofs[0].write(kIndex, byte));
    SHELL_CASE02(kRegBg1HorOffset,   ppu.bghofs[1].write(kIndex, byte));
    SHELL_CASE02(kRegBg1VerOffset,   ppu.bgvofs[1].write(kIndex, byte));
    SHELL_CASE02(kRegBg2HorOffset,   ppu.bghofs[2].write(kIndex, byte));
    SHELL_CASE02(kRegBg2VerOffset,   ppu.bgvofs[2].write(kIndex, byte));
    SHELL_CASE02(kRegBg3HorOffset,   ppu.bghofs[3].write(kIndex, byte));
    SHELL_CASE02(kRegBg3VerOffset,   ppu.bgvofs[3].write(kIndex, byte));
    SHELL_CASE02(kRegBg2ParameterA,  ppu.bgpa[0].write(kIndex, byte));
    SHELL_CASE02(kRegBg2ParameterB,  ppu.bgpb[0].write(kIndex, byte));
    SHELL_CASE02(kRegBg2ParameterC,  ppu.bgpc[0].write(kIndex, byte));
    SHELL_CASE02(kRegBg2ParameterD,  ppu.bgpd[0].write(kIndex, byte));
    SHELL_CASE04(kRegBg2ReferenceX,  ppu.bgx[0].write(kIndex, byte));
    SHELL_CASE04(kRegBg2ReferenceY,  ppu.bgy[0].write(kIndex, byte));
    SHELL_CASE02(kRegBg3ParameterA,  ppu.bgpa[1].write(kIndex, byte));
    SHELL_CASE02(kRegBg3ParameterB,  ppu.bgpb[1].write(kIndex, byte));
    SHELL_CASE02(kRegBg3ParameterC,  ppu.bgpc[1].write(kIndex, byte));
    SHELL_CASE02(kRegBg3ParameterD,  ppu.bgpd[1].write(kIndex, byte));
    SHELL_CASE04(kRegBg3ReferenceX,  ppu.bgx[1].write(kIndex, byte));
    SHELL_CASE04(kRegBg3ReferenceY,  ppu.bgy[1].write(kIndex, byte));
    SHELL_CASE02(kRegWindow0Hor,     ppu.winh[0].write(kIndex, byte));
    SHELL_CASE02(kRegWindow1Hor,     ppu.winh[1].write(kIndex, byte));
    SHELL_CASE02(kRegWindow0Ver,     ppu.winv[0].write(kIndex, byte));
    SHELL_CASE02(kRegWindow1Ver,     ppu.winv[1].write(kIndex, byte));
    SHELL_CASE02(kRegWindowInside,   ppu.winin.write(kIndex, byte));
    SHELL_CASE02(kRegWindowOutside,  ppu.winout.write(kIndex, byte));
    SHELL_CASE02(kRegMosaic,         ppu.mosaic.write(kIndex, byte));
    SHELL_CASE02(kRegBlendControl,   ppu.bldcnt.write(kIndex, byte));
    SHELL_CASE02(kRegBlendAlpha,     ppu.bldalpha.write(kIndex, byte));
    SHELL_CASE02(kRegBlendFade,      ppu.bldfade.write(kIndex, byte));
    SHELL_CASE08(kRegSound1ControlL, apu.square1.write(kIndex, byte));
    SHELL_CASE08(kRegSound2ControlL, apu.square2.write(kIndex, byte));
    SHELL_CASE08(kRegSound3ControlL, apu.wave.write(kIndex, byte));
    SHELL_CASE08(kRegSound4ControlL, apu.noise.write(kIndex, byte));
    SHELL_CASE08(kRegSoundControlL,  apu.control.write(kIndex, byte));
    SHELL_CASE02(kRegSoundBias,      apu.bias.write(kIndex, byte));
    SHELL_CASE16(kRegWaveRam0,       apu.wave.ram.write(kIndex, byte));
    SHELL_CASE04(kRegFifoA,          apu.fifo[0].write(byte));
    SHELL_CASE04(kRegFifoB,          apu.fifo[1].write(byte));
    SHELL_CASE04(kRegDma0Sad,        dma.channels[0].sad.write(kIndex, byte));
    SHELL_CASE04(kRegDma0Dad,        dma.channels[0].dad.write(kIndex, byte));
    SHELL_CASE02(kRegDma0Count,      dma.channels[0].count.write(kIndex, byte));
    SHELL_CASE02(kRegDma0Control,    dma.channels[0].control.write(kIndex, byte));
    SHELL_CASE04(kRegDma1Sad,        dma.channels[1].sad.write(kIndex, byte));
    SHELL_CASE04(kRegDma1Dad,        dma.channels[1].dad.write(kIndex, byte));
    SHELL_CASE02(kRegDma1Count,      dma.channels[1].count.write(kIndex, byte));
    SHELL_CASE02(kRegDma1Control,    dma.channels[1].control.write(kIndex, byte));
    SHELL_CASE04(kRegDma2Sad,        dma.channels[2].sad.write(kIndex, byte));
    SHELL_CASE04(kRegDma2Dad,        dma.channels[2].dad.write(kIndex, byte));
    SHELL_CASE02(kRegDma2Count,      dma.channels[2].count.write(kIndex, byte));
    SHELL_CASE02(kRegDma2Control,    dma.channels[2].control.write(kIndex, byte));
    SHELL_CASE04(kRegDma3Sad,        dma.channels[3].sad.write(kIndex, byte));
    SHELL_CASE04(kRegDma3Dad,        dma.channels[3].dad.write(kIndex, byte));
    SHELL_CASE02(kRegDma3Count,      dma.channels[3].count.write(kIndex, byte));
    SHELL_CASE02(kRegDma3Control,    dma.channels[3].control.write(kIndex, byte));
    SHELL_CASE02(kRegTimer0Count,    timer.channels[0].count.write(kIndex, byte));
    SHELL_CASE01(kRegTimer0Control,  timer.channels[0].control.write(kIndex, byte));
    SHELL_CASE02(kRegTimer1Count,    timer.channels[1].count.write(kIndex, byte));
    SHELL_CASE01(kRegTimer1Control,  timer.channels[1].control.write(kIndex, byte));
    SHELL_CASE02(kRegTimer2Count,    timer.channels[2].count.write(kIndex, byte));
    SHELL_CASE01(kRegTimer2Control,  timer.channels[2].control.write(kIndex, byte));
    SHELL_CASE02(kRegTimer3Count,    timer.channels[3].count.write(kIndex, byte));
    SHELL_CASE01(kRegTimer3Control,  timer.channels[3].control.write(kIndex, byte));
    SHELL_CASE02(kRegSioMulti0,      sio.siomulti[0].write(kIndex, byte));
    SHELL_CASE02(kRegSioMulti1,      sio.siomulti[1].write(kIndex, byte));
    SHELL_CASE02(kRegSioMulti2,      sio.siomulti[2].write(kIndex, byte));
    SHELL_CASE02(kRegSioMulti3,      sio.siomulti[3].write(kIndex, byte));
    SHELL_CASE02(kRegSioControl,     sio.siocnt.write(kIndex, byte));
    SHELL_CASE02(kRegSioSend,        sio.siosend.write(kIndex, byte));
    SHELL_CASE02(kRegKeyControl,     keypad.control.write(kIndex, byte));
    SHELL_CASE02(kRegRemoteControl,  sio.rcnt.write(kIndex, byte));
    SHELL_CASE02(kRegJoyControl,     sio.joycnt.write(kIndex, byte));
    SHELL_CASE04(kRegJoyReceive,     sio.joyrecv.write(kIndex, byte));
    SHELL_CASE04(kRegJoyTransmit,    sio.joytrans.write(kIndex, byte));
    SHELL_CASE02(kRegJoyStatus,      sio.joystat.write(kIndex, byte));
    SHELL_CASE02(kRegIrqEnable,      irq.enable.write(kIndex, byte));
    SHELL_CASE02(kRegIrqRequest,     irq.request.write(kIndex, byte));
    SHELL_CASE02(kRegWaitControl,    waitcnt.write(kIndex, byte));
    SHELL_CASE04(kRegIrqMaster,      irq.master.write(kIndex, byte));
    SHELL_CASE01(kRegPostFlag,       postflg.write(kIndex, byte));
    SHELL_CASE01(kRegHaltControl,    haltcnt.write(kIndex, byte));
    }
}
