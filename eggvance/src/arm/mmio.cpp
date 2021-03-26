#include "arm.h"

#include <shell/macros.h>

#include "apu/apu.h"
#include "dma/dma.h"
#include "keypad/keypad.h"
#include "ppu/ppu.h"
#include "sio/sio.h"
#include "timer/timer.h"

enum Io
{
    kDisplayControl = 0x000,
    kGreenSwap      = 0x002,
    kDisplayStatus  = 0x004,
    kVerticalCount  = 0x006,
    kBg0Control     = 0x008,
    kBg1Control     = 0x00A,
    kBg2Control     = 0x00C,
    kBg3Control     = 0x00E,
    kBg0HorOffset   = 0x010,
    kBg0VerOffset   = 0x012,
    kBg1HorOffset   = 0x014,
    kBg1VerOffset   = 0x016,
    kBg2HorOffset   = 0x018,
    kBg2VerOffset   = 0x01A,
    kBg3HorOffset   = 0x01C,
    kBg3VerOffset   = 0x01E,
    kBg2ParameterA  = 0x020,
    kBg2ParameterB  = 0x022,
    kBg2ParameterC  = 0x024,
    kBg2ParameterD  = 0x026,
    kBg2ReferenceX  = 0x028,
    kBg2ReferenceY  = 0x02C,
    kBg3ParameterA  = 0x030,
    kBg3ParameterB  = 0x032,
    kBg3ParameterC  = 0x034,
    kBg3ParameterD  = 0x036,
    kBg3ReferenceX  = 0x038,
    kBg3ReferenceY  = 0x03C,
    kWindow0Hor     = 0x040,
    kWindow1Hor     = 0x042,
    kWindow0Ver     = 0x044,
    kWindow1Ver     = 0x046,
    kWindowInside   = 0x048,
    kWindowOutside  = 0x04A,
    kMosaic         = 0x04C,
    kBlendControl   = 0x050,
    kBlendAlpha     = 0x052,
    kBlendFade      = 0x054,
    kSoundSquare1   = 0x060,
    kSoundSquare2   = 0x068,
    kSoundWave      = 0x070,
    kSoundNoise     = 0x078,
    kSoundControl   = 0x080,
    kSoundBias      = 0x088,
    kUnused08A      = 0x08A,
    kWaveRam        = 0x090,
    kFifoA          = 0x0A0,
    kFifoB          = 0x0A4,
    kDma0Sad        = 0x0B0,
    kDma0Dad        = 0x0B4,
    kDma0Count      = 0x0B8,
    kDma0Control    = 0x0BA,
    kDma1Sad        = 0x0BC,
    kDma1Dad        = 0x0C0,
    kDma1Count      = 0x0C4,
    kDma1Control    = 0x0C6,
    kDma2Sad        = 0x0C8,
    kDma2Dad        = 0x0CC,
    kDma2Count      = 0x0D0,
    kDma2Control    = 0x0D2,
    kDma3Sad        = 0x0D4,
    kDma3Dad        = 0x0D8,
    kDma3Count      = 0x0DC,
    kDma3Control    = 0x0DE,
    kTimer0Count    = 0x100,
    kTimer0Control  = 0x102,
    kTimer1Count    = 0x104,
    kTimer1Control  = 0x106,
    kTimer2Count    = 0x108,
    kTimer2Control  = 0x10A,
    kTimer3Count    = 0x10C,
    kTimer3Control  = 0x10E,
    kSioData32      = 0x120,
    kSioMulti0      = 0x120,
    kSioMulti1      = 0x122,
    kSioMulti2      = 0x124,
    kSioMulti3      = 0x126,
    kSioControl     = 0x128,
    kSioSend        = 0x12A,
    kSioData8       = 0x12A,
    kKeyInput       = 0x130,
    kKeyControl     = 0x132,
    kRemoteControl  = 0x134,
    kUnused136      = 0x136,
    kJoyControl     = 0x140,
    kUnused142      = 0x142,
    kJoyReceive     = 0x150,
    kJoyTransmit    = 0x154,
    kJoyStatus      = 0x158,
    kUnused15A      = 0x15A,
    kIrqEnable      = 0x200,
    kIrqRequest     = 0x202,
    kWaitControl    = 0x204,
    kUnused206      = 0x206,
    kIrqMaster      = 0x208,
    kPostFlag       = 0x300,
    kHaltControl    = 0x301
};

u8 Arm::readIo(u32 addr)
{
    switch (addr & 0x3FF'FFFF)
    {
    SHELL_CASE02(Io::kDisplayControl, return ppu.dispcnt.read(kIndex));
    SHELL_CASE02(Io::kGreenSwap,      return ppu.greenswap.read(kIndex));
    SHELL_CASE02(Io::kDisplayStatus,  return ppu.dispstat.read(kIndex));
    SHELL_CASE02(Io::kVerticalCount,  return ppu.vcount.read(kIndex));
    SHELL_CASE02(Io::kBg0Control,     return ppu.bgcnt[0].read(kIndex));
    SHELL_CASE02(Io::kBg1Control,     return ppu.bgcnt[1].read(kIndex));
    SHELL_CASE02(Io::kBg2Control,     return ppu.bgcnt[2].read(kIndex));
    SHELL_CASE02(Io::kBg3Control,     return ppu.bgcnt[3].read(kIndex));
    SHELL_CASE02(Io::kWindowInside,   return ppu.winin.read(kIndex));
    SHELL_CASE02(Io::kWindowOutside,  return ppu.winout.read(kIndex));
    SHELL_CASE02(Io::kBlendControl,   return ppu.bldcnt.read(kIndex));
    SHELL_CASE02(Io::kBlendAlpha,     return ppu.bldalpha.read(kIndex));
    SHELL_CASE08(Io::kSoundSquare1, return apu.square1.read(kIndex));
    SHELL_CASE08(Io::kSoundSquare2, return apu.square2.read(kIndex));
    SHELL_CASE08(Io::kSoundWave, return apu.wave.read(kIndex));
    SHELL_CASE08(Io::kSoundNoise, return apu.noise.read(kIndex));
    SHELL_CASE08(Io::kSoundControl,  return apu.control.read(kIndex));
    SHELL_CASE02(Io::kSoundBias,      return apu.bias.read(kIndex));
    SHELL_CASE02(Io::kUnused08A,      return 0);
    SHELL_CASE16(Io::kWaveRam,       return apu.wave.ram.read(kIndex));
    SHELL_CASE02(Io::kDma0Count,      return 0);
    SHELL_CASE02(Io::kDma0Control,    return dma.channels[0].control.read(kIndex));
    SHELL_CASE02(Io::kDma1Count,      return 0);
    SHELL_CASE02(Io::kDma1Control,    return dma.channels[1].control.read(kIndex));
    SHELL_CASE02(Io::kDma2Count,      return 0);
    SHELL_CASE02(Io::kDma2Control,    return dma.channels[2].control.read(kIndex));
    SHELL_CASE02(Io::kDma3Count,      return 0);
    SHELL_CASE02(Io::kDma3Control,    return dma.channels[3].control.read(kIndex));
    SHELL_CASE02(Io::kTimer0Count,    return timer.channels[0].count.read(kIndex));
    SHELL_CASE02(Io::kTimer0Control,  return timer.channels[0].control.read(kIndex));
    SHELL_CASE02(Io::kTimer1Count,    return timer.channels[1].count.read(kIndex));
    SHELL_CASE02(Io::kTimer1Control,  return timer.channels[1].control.read(kIndex));
    SHELL_CASE02(Io::kTimer2Count,    return timer.channels[2].count.read(kIndex));
    SHELL_CASE02(Io::kTimer2Control,  return timer.channels[2].control.read(kIndex));
    SHELL_CASE02(Io::kTimer3Count,    return timer.channels[3].count.read(kIndex));
    SHELL_CASE02(Io::kTimer3Control,  return timer.channels[3].control.read(kIndex));
    SHELL_CASE02(Io::kSioMulti0,      return sio.siomulti[0].read(kIndex));
    SHELL_CASE02(Io::kSioMulti1,      return sio.siomulti[1].read(kIndex));
    SHELL_CASE02(Io::kSioMulti2,      return sio.siomulti[2].read(kIndex));
    SHELL_CASE02(Io::kSioMulti3,      return sio.siomulti[3].read(kIndex));
    SHELL_CASE02(Io::kSioControl,     return sio.siocnt.read(kIndex));
    SHELL_CASE02(Io::kSioSend,        return sio.siosend.read(kIndex));
    SHELL_CASE02(Io::kKeyInput,       return keypad.input.read(kIndex));
    SHELL_CASE02(Io::kKeyControl,     return keypad.control.read(kIndex));
    SHELL_CASE02(Io::kRemoteControl,  return sio.rcnt.read(kIndex));
    SHELL_CASE02(Io::kUnused136,      return 0);
    SHELL_CASE02(Io::kJoyControl,     return sio.joycnt.read(kIndex));
    SHELL_CASE02(Io::kUnused142,      return 0);
    SHELL_CASE04(Io::kJoyReceive,     return sio.joyrecv.read(kIndex));
    SHELL_CASE04(Io::kJoyTransmit,    return sio.joytrans.read(kIndex));
    SHELL_CASE02(Io::kJoyStatus,      return sio.joystat.read(kIndex));
    SHELL_CASE02(Io::kUnused15A,      return 0);
    SHELL_CASE02(Io::kIrqEnable,      return irq.enable.read(kIndex));
    SHELL_CASE02(Io::kIrqRequest,     return irq.request.read(kIndex));
    SHELL_CASE02(Io::kWaitControl,    return waitcnt.read(kIndex));
    SHELL_CASE02(Io::kUnused206,      return 0);
    SHELL_CASE04(Io::kIrqMaster,      return irq.master.read(kIndex));
    SHELL_CASE01(Io::kPostFlag,       return postflg.read(kIndex));

    default:
        return readUnused() >> (8 * (addr & 0x3));
    }
}

void Arm::writeIo(u32 addr, u8 byte)
{
    switch (addr & 0x3FF'FFFF)
    {
    SHELL_CASE02(Io::kDisplayControl, ppu.dispcnt.write(kIndex, byte));
    SHELL_CASE02(Io::kGreenSwap,      ppu.greenswap.write(kIndex, byte));
    SHELL_CASE02(Io::kDisplayStatus,  ppu.dispstat.write(kIndex, byte));
    SHELL_CASE02(Io::kBg0Control,     ppu.bgcnt[0].write(kIndex, byte));
    SHELL_CASE02(Io::kBg1Control,     ppu.bgcnt[1].write(kIndex, byte));
    SHELL_CASE02(Io::kBg2Control,     ppu.bgcnt[2].write(kIndex, byte));
    SHELL_CASE02(Io::kBg3Control,     ppu.bgcnt[3].write(kIndex, byte));
    SHELL_CASE02(Io::kBg0HorOffset,   ppu.bghofs[0].write(kIndex, byte));
    SHELL_CASE02(Io::kBg0VerOffset,   ppu.bgvofs[0].write(kIndex, byte));
    SHELL_CASE02(Io::kBg1HorOffset,   ppu.bghofs[1].write(kIndex, byte));
    SHELL_CASE02(Io::kBg1VerOffset,   ppu.bgvofs[1].write(kIndex, byte));
    SHELL_CASE02(Io::kBg2HorOffset,   ppu.bghofs[2].write(kIndex, byte));
    SHELL_CASE02(Io::kBg2VerOffset,   ppu.bgvofs[2].write(kIndex, byte));
    SHELL_CASE02(Io::kBg3HorOffset,   ppu.bghofs[3].write(kIndex, byte));
    SHELL_CASE02(Io::kBg3VerOffset,   ppu.bgvofs[3].write(kIndex, byte));
    SHELL_CASE02(Io::kBg2ParameterA,  ppu.bgpa[0].write(kIndex, byte));
    SHELL_CASE02(Io::kBg2ParameterB,  ppu.bgpb[0].write(kIndex, byte));
    SHELL_CASE02(Io::kBg2ParameterC,  ppu.bgpc[0].write(kIndex, byte));
    SHELL_CASE02(Io::kBg2ParameterD,  ppu.bgpd[0].write(kIndex, byte));
    SHELL_CASE04(Io::kBg2ReferenceX,  ppu.bgx[0].write(kIndex, byte));
    SHELL_CASE04(Io::kBg2ReferenceY,  ppu.bgy[0].write(kIndex, byte));
    SHELL_CASE02(Io::kBg3ParameterA,  ppu.bgpa[1].write(kIndex, byte));
    SHELL_CASE02(Io::kBg3ParameterB,  ppu.bgpb[1].write(kIndex, byte));
    SHELL_CASE02(Io::kBg3ParameterC,  ppu.bgpc[1].write(kIndex, byte));
    SHELL_CASE02(Io::kBg3ParameterD,  ppu.bgpd[1].write(kIndex, byte));
    SHELL_CASE04(Io::kBg3ReferenceX,  ppu.bgx[1].write(kIndex, byte));
    SHELL_CASE04(Io::kBg3ReferenceY,  ppu.bgy[1].write(kIndex, byte));
    SHELL_CASE02(Io::kWindow0Hor,     ppu.winh[0].write(kIndex, byte));
    SHELL_CASE02(Io::kWindow1Hor,     ppu.winh[1].write(kIndex, byte));
    SHELL_CASE02(Io::kWindow0Ver,     ppu.winv[0].write(kIndex, byte));
    SHELL_CASE02(Io::kWindow1Ver,     ppu.winv[1].write(kIndex, byte));
    SHELL_CASE02(Io::kWindowInside,   ppu.winin.write(kIndex, byte));
    SHELL_CASE02(Io::kWindowOutside,  ppu.winout.write(kIndex, byte));
    SHELL_CASE02(Io::kMosaic,         ppu.mosaic.write(kIndex, byte));
    SHELL_CASE02(Io::kBlendControl,   ppu.bldcnt.write(kIndex, byte));
    SHELL_CASE02(Io::kBlendAlpha,     ppu.bldalpha.write(kIndex, byte));
    SHELL_CASE02(Io::kBlendFade,      ppu.bldfade.write(kIndex, byte));
    SHELL_CASE08(Io::kSoundSquare1, apu.square1.write(kIndex, byte));
    SHELL_CASE08(Io::kSoundSquare2, apu.square2.write(kIndex, byte));
    SHELL_CASE08(Io::kSoundWave, apu.wave.write(kIndex, byte));
    SHELL_CASE08(Io::kSoundNoise, apu.noise.write(kIndex, byte));
    SHELL_CASE08(Io::kSoundControl,  apu.control.write(kIndex, byte));
    SHELL_CASE02(Io::kSoundBias,      apu.bias.write(kIndex, byte));
    SHELL_CASE16(Io::kWaveRam,       apu.wave.ram.write(kIndex, byte));
    SHELL_CASE04(Io::kFifoA,          apu.fifo[0].write(byte));
    SHELL_CASE04(Io::kFifoB,          apu.fifo[1].write(byte));
    SHELL_CASE04(Io::kDma0Sad,        dma.channels[0].sad.write(kIndex, byte));
    SHELL_CASE04(Io::kDma0Dad,        dma.channels[0].dad.write(kIndex, byte));
    SHELL_CASE02(Io::kDma0Count,      dma.channels[0].count.write(kIndex, byte));
    SHELL_CASE02(Io::kDma0Control,    dma.channels[0].control.write(kIndex, byte));
    SHELL_CASE04(Io::kDma1Sad,        dma.channels[1].sad.write(kIndex, byte));
    SHELL_CASE04(Io::kDma1Dad,        dma.channels[1].dad.write(kIndex, byte));
    SHELL_CASE02(Io::kDma1Count,      dma.channels[1].count.write(kIndex, byte));
    SHELL_CASE02(Io::kDma1Control,    dma.channels[1].control.write(kIndex, byte));
    SHELL_CASE04(Io::kDma2Sad,        dma.channels[2].sad.write(kIndex, byte));
    SHELL_CASE04(Io::kDma2Dad,        dma.channels[2].dad.write(kIndex, byte));
    SHELL_CASE02(Io::kDma2Count,      dma.channels[2].count.write(kIndex, byte));
    SHELL_CASE02(Io::kDma2Control,    dma.channels[2].control.write(kIndex, byte));
    SHELL_CASE04(Io::kDma3Sad,        dma.channels[3].sad.write(kIndex, byte));
    SHELL_CASE04(Io::kDma3Dad,        dma.channels[3].dad.write(kIndex, byte));
    SHELL_CASE02(Io::kDma3Count,      dma.channels[3].count.write(kIndex, byte));
    SHELL_CASE02(Io::kDma3Control,    dma.channels[3].control.write(kIndex, byte));
    SHELL_CASE02(Io::kTimer0Count,    timer.channels[0].count.write(kIndex, byte));
    SHELL_CASE01(Io::kTimer0Control,  timer.channels[0].control.write(kIndex, byte));
    SHELL_CASE02(Io::kTimer1Count,    timer.channels[1].count.write(kIndex, byte));
    SHELL_CASE01(Io::kTimer1Control,  timer.channels[1].control.write(kIndex, byte));
    SHELL_CASE02(Io::kTimer2Count,    timer.channels[2].count.write(kIndex, byte));
    SHELL_CASE01(Io::kTimer2Control,  timer.channels[2].control.write(kIndex, byte));
    SHELL_CASE02(Io::kTimer3Count,    timer.channels[3].count.write(kIndex, byte));
    SHELL_CASE01(Io::kTimer3Control,  timer.channels[3].control.write(kIndex, byte));
    SHELL_CASE02(Io::kSioMulti0,      sio.siomulti[0].write(kIndex, byte));
    SHELL_CASE02(Io::kSioMulti1,      sio.siomulti[1].write(kIndex, byte));
    SHELL_CASE02(Io::kSioMulti2,      sio.siomulti[2].write(kIndex, byte));
    SHELL_CASE02(Io::kSioMulti3,      sio.siomulti[3].write(kIndex, byte));
    SHELL_CASE02(Io::kSioControl,     sio.siocnt.write(kIndex, byte));
    SHELL_CASE02(Io::kSioSend,        sio.siosend.write(kIndex, byte));
    SHELL_CASE02(Io::kKeyControl,     keypad.control.write(kIndex, byte));
    SHELL_CASE02(Io::kRemoteControl,  sio.rcnt.write(kIndex, byte));
    SHELL_CASE02(Io::kJoyControl,     sio.joycnt.write(kIndex, byte));
    SHELL_CASE04(Io::kJoyReceive,     sio.joyrecv.write(kIndex, byte));
    SHELL_CASE04(Io::kJoyTransmit,    sio.joytrans.write(kIndex, byte));
    SHELL_CASE02(Io::kJoyStatus,      sio.joystat.write(kIndex, byte));
    SHELL_CASE02(Io::kIrqEnable,      irq.enable.write(kIndex, byte));
    SHELL_CASE02(Io::kIrqRequest,     irq.request.write(kIndex, byte));
    SHELL_CASE02(Io::kWaitControl,    waitcnt.write(kIndex, byte));
    SHELL_CASE04(Io::kIrqMaster,      irq.master.write(kIndex, byte));
    SHELL_CASE01(Io::kPostFlag,       postflg.write(kIndex, byte));
    SHELL_CASE01(Io::kHaltControl,    haltcnt.write(kIndex, byte));
    }
}
