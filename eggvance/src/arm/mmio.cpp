#include "arm.h"

#include "apu/apu.h"
#include "dma/dma.h"
#include "keypad/keypad.h"
#include "ppu/ppu.h"
#include "sio/sio.h"
#include "timer/timer.h"

u8 Arm::readIo(u32 addr)
{
    switch (addr)
    {
    SHELL_CASE02(uint(Io::DisplayControl), return ppu.dispcnt.read(kIndex));
    SHELL_CASE02(uint(Io::GreenSwap),      return ppu.greenswap.read(kIndex));
    SHELL_CASE02(uint(Io::DisplayStatus),  return ppu.dispstat.read(kIndex));
    SHELL_CASE02(uint(Io::VerticalCount),  return ppu.vcount.read(kIndex));
    SHELL_CASE02(uint(Io::Bg0Control),     return ppu.backgrounds[0].control.read(kIndex));
    SHELL_CASE02(uint(Io::Bg1Control),     return ppu.backgrounds[1].control.read(kIndex));
    SHELL_CASE02(uint(Io::Bg2Control),     return ppu.backgrounds[2].control.read(kIndex));
    SHELL_CASE02(uint(Io::Bg3Control),     return ppu.backgrounds[3].control.read(kIndex));
    SHELL_CASE02(uint(Io::WindowInside),   return ppu.winin.read(kIndex));
    SHELL_CASE02(uint(Io::WindowOutside),  return ppu.winout.read(kIndex));
    SHELL_CASE02(uint(Io::BlendControl),   return ppu.bldcnt.read(kIndex));
    SHELL_CASE02(uint(Io::BlendAlpha),     return ppu.bldalpha.read(kIndex));
    SHELL_CASE08(uint(Io::SoundSquare1),   return apu.square1.read(kIndex));
    SHELL_CASE08(uint(Io::SoundSquare2),   return apu.square2.read(kIndex));
    SHELL_CASE08(uint(Io::SoundWave),      return apu.wave.read(kIndex));
    SHELL_CASE08(uint(Io::SoundNoise),     return apu.noise.read(kIndex));
    SHELL_CASE08(uint(Io::SoundControl),   return apu.control.read(kIndex));
    SHELL_CASE02(uint(Io::SoundBias),      return apu.bias.read(kIndex));
    SHELL_CASE02(uint(Io::Unused08A),      return 0);
    SHELL_CASE16(uint(Io::WaveRam),        return apu.wave.ram.read(kIndex));
    SHELL_CASE02(uint(Io::Dma0Count),      return 0);
    SHELL_CASE02(uint(Io::Dma0Control),    return dma.channels[0].control.read(kIndex));
    SHELL_CASE02(uint(Io::Dma1Count),      return 0);
    SHELL_CASE02(uint(Io::Dma1Control),    return dma.channels[1].control.read(kIndex));
    SHELL_CASE02(uint(Io::Dma2Count),      return 0);
    SHELL_CASE02(uint(Io::Dma2Control),    return dma.channels[2].control.read(kIndex));
    SHELL_CASE02(uint(Io::Dma3Count),      return 0);
    SHELL_CASE02(uint(Io::Dma3Control),    return dma.channels[3].control.read(kIndex));
    SHELL_CASE02(uint(Io::Timer0Count),    return timer.channels[0].count.read(kIndex));
    SHELL_CASE02(uint(Io::Timer0Control),  return timer.channels[0].control.read(kIndex));
    SHELL_CASE02(uint(Io::Timer1Count),    return timer.channels[1].count.read(kIndex));
    SHELL_CASE02(uint(Io::Timer1Control),  return timer.channels[1].control.read(kIndex));
    SHELL_CASE02(uint(Io::Timer2Count),    return timer.channels[2].count.read(kIndex));
    SHELL_CASE02(uint(Io::Timer2Control),  return timer.channels[2].control.read(kIndex));
    SHELL_CASE02(uint(Io::Timer3Count),    return timer.channels[3].count.read(kIndex));
    SHELL_CASE02(uint(Io::Timer3Control),  return timer.channels[3].control.read(kIndex));
    SHELL_CASE02(uint(Io::SioMulti),       return sio.siomulti.read(kIndex));
    SHELL_CASE02(uint(Io::SioControl),     return sio.siocnt.read(kIndex));
    SHELL_CASE02(uint(Io::SioSend),        return sio.siosend.read(kIndex));
    SHELL_CASE02(uint(Io::KeyInput),       return keypad.input.read(kIndex));
    SHELL_CASE02(uint(Io::KeyControl),     return keypad.control.read(kIndex));
    SHELL_CASE02(uint(Io::RemoteControl),  return sio.rcnt.read(kIndex));
    SHELL_CASE02(uint(Io::Unused136),      return 0);
    SHELL_CASE02(uint(Io::JoyControl),     return sio.joycnt.read(kIndex));
    SHELL_CASE02(uint(Io::Unused142),      return 0);
    SHELL_CASE04(uint(Io::JoyReceive),     return sio.joyrecv.read(kIndex));
    SHELL_CASE04(uint(Io::JoyTransmit),    return sio.joytrans.read(kIndex));
    SHELL_CASE02(uint(Io::JoyStatus),      return sio.joystat.read(kIndex));
    SHELL_CASE02(uint(Io::Unused15A),      return 0);
    SHELL_CASE02(uint(Io::IrqEnable),      return interrupt.enable.read(kIndex));
    SHELL_CASE02(uint(Io::IrqRequest),     return interrupt.request.read(kIndex));
    SHELL_CASE02(uint(Io::WaitControl),    return waitcnt.read(kIndex));
    SHELL_CASE02(uint(Io::Unused206),      return 0);
    SHELL_CASE04(uint(Io::IrqMaster),      return interrupt.master.read(kIndex));
    SHELL_CASE01(uint(Io::PostFlag),       return postflg.read(kIndex));

    default:
        return readUnused() >> (8 * (addr & 0x3));
    }
}

void Arm::writeIo(u32 addr, u8 byte)
{
    switch (addr)
    {
    SHELL_CASE02(uint(Io::DisplayControl), ppu.dispcnt.write(kIndex, byte));
    SHELL_CASE02(uint(Io::GreenSwap),      ppu.greenswap.write(kIndex, byte));
    SHELL_CASE02(uint(Io::DisplayStatus),  ppu.dispstat.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg0Control),     ppu.backgrounds[0].control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg1Control),     ppu.backgrounds[1].control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg2Control),     ppu.backgrounds[2].control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg3Control),     ppu.backgrounds[3].control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg0HorOffset),   ppu.backgrounds[0].offset.writeX(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg0VerOffset),   ppu.backgrounds[0].offset.writeY(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg1HorOffset),   ppu.backgrounds[1].offset.writeX(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg1VerOffset),   ppu.backgrounds[1].offset.writeY(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg2HorOffset),   ppu.backgrounds[2].offset.writeX(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg2VerOffset),   ppu.backgrounds[2].offset.writeY(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg3HorOffset),   ppu.backgrounds[3].offset.writeX(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg3VerOffset),   ppu.backgrounds[3].offset.writeY(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg2ParameterA),  ppu.backgrounds[2].matrix.writeA(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg2ParameterB),  ppu.backgrounds[2].matrix.writeB(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg2ParameterC),  ppu.backgrounds[2].matrix.writeC(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg2ParameterD),  ppu.backgrounds[2].matrix.writeD(kIndex, byte));
    SHELL_CASE04(uint(Io::Bg2ReferenceX),  ppu.backgrounds[2].matrix.writeX(kIndex, byte));
    SHELL_CASE04(uint(Io::Bg2ReferenceY),  ppu.backgrounds[2].matrix.writeY(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg3ParameterA),  ppu.backgrounds[3].matrix.writeA(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg3ParameterB),  ppu.backgrounds[3].matrix.writeB(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg3ParameterC),  ppu.backgrounds[3].matrix.writeC(kIndex, byte));
    SHELL_CASE02(uint(Io::Bg3ParameterD),  ppu.backgrounds[3].matrix.writeD(kIndex, byte));
    SHELL_CASE04(uint(Io::Bg3ReferenceX),  ppu.backgrounds[3].matrix.writeX(kIndex, byte));
    SHELL_CASE04(uint(Io::Bg3ReferenceY),  ppu.backgrounds[3].matrix.writeY(kIndex, byte));
    SHELL_CASE02(uint(Io::Window0Hor),     ppu.winh[0].write(kIndex, byte));
    SHELL_CASE02(uint(Io::Window1Hor),     ppu.winh[1].write(kIndex, byte));
    SHELL_CASE02(uint(Io::Window0Ver),     ppu.winv[0].write(kIndex, byte));
    SHELL_CASE02(uint(Io::Window1Ver),     ppu.winv[1].write(kIndex, byte));
    SHELL_CASE02(uint(Io::WindowInside),   ppu.winin.write(kIndex, byte));
    SHELL_CASE02(uint(Io::WindowOutside),  ppu.winout.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Mosaic),         ppu.mosaic.write(kIndex, byte));
    SHELL_CASE02(uint(Io::BlendControl),   ppu.bldcnt.write(kIndex, byte));
    SHELL_CASE02(uint(Io::BlendAlpha),     ppu.bldalpha.write(kIndex, byte));
    SHELL_CASE02(uint(Io::BlendFade),      ppu.bldfade.write(kIndex, byte));
    SHELL_CASE08(uint(Io::SoundSquare1),   apu.square1.write(kIndex, byte));
    SHELL_CASE08(uint(Io::SoundSquare2),   apu.square2.write(kIndex, byte));
    SHELL_CASE08(uint(Io::SoundWave),      apu.wave.write(kIndex, byte));
    SHELL_CASE08(uint(Io::SoundNoise),     apu.noise.write(kIndex, byte));
    SHELL_CASE08(uint(Io::SoundControl),   apu.control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::SoundBias),      apu.bias.write(kIndex, byte));
    SHELL_CASE16(uint(Io::WaveRam),        apu.wave.ram.write(kIndex, byte));
    SHELL_CASE04(uint(Io::FifoA),          apu.fifos[0].write(byte));
    SHELL_CASE04(uint(Io::FifoB),          apu.fifos[1].write(byte));
    SHELL_CASE04(uint(Io::Dma0Sad),        dma.channels[0].sad.write(kIndex, byte));
    SHELL_CASE04(uint(Io::Dma0Dad),        dma.channels[0].dad.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Dma0Count),      dma.channels[0].count.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Dma0Control),    dma.channels[0].control.write(kIndex, byte));
    SHELL_CASE04(uint(Io::Dma1Sad),        dma.channels[1].sad.write(kIndex, byte));
    SHELL_CASE04(uint(Io::Dma1Dad),        dma.channels[1].dad.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Dma1Count),      dma.channels[1].count.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Dma1Control),    dma.channels[1].control.write(kIndex, byte));
    SHELL_CASE04(uint(Io::Dma2Sad),        dma.channels[2].sad.write(kIndex, byte));
    SHELL_CASE04(uint(Io::Dma2Dad),        dma.channels[2].dad.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Dma2Count),      dma.channels[2].count.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Dma2Control),    dma.channels[2].control.write(kIndex, byte));
    SHELL_CASE04(uint(Io::Dma3Sad),        dma.channels[3].sad.write(kIndex, byte));
    SHELL_CASE04(uint(Io::Dma3Dad),        dma.channels[3].dad.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Dma3Count),      dma.channels[3].count.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Dma3Control),    dma.channels[3].control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Timer0Count),    timer.channels[0].count.write(kIndex, byte));
    SHELL_CASE01(uint(Io::Timer0Control),  timer.channels[0].control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Timer1Count),    timer.channels[1].count.write(kIndex, byte));
    SHELL_CASE01(uint(Io::Timer1Control),  timer.channels[1].control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Timer2Count),    timer.channels[2].count.write(kIndex, byte));
    SHELL_CASE01(uint(Io::Timer2Control),  timer.channels[2].control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::Timer3Count),    timer.channels[3].count.write(kIndex, byte));
    SHELL_CASE01(uint(Io::Timer3Control),  timer.channels[3].control.write(kIndex, byte));
    SHELL_CASE08(uint(Io::SioMulti),       sio.siomulti.write(kIndex, byte));
    SHELL_CASE02(uint(Io::SioControl),     sio.siocnt.write(kIndex, byte));
    SHELL_CASE02(uint(Io::SioSend),        sio.siosend.write(kIndex, byte));
    SHELL_CASE02(uint(Io::KeyControl),     keypad.control.write(kIndex, byte));
    SHELL_CASE02(uint(Io::RemoteControl),  sio.rcnt.write(kIndex, byte));
    SHELL_CASE02(uint(Io::JoyControl),     sio.joycnt.write(kIndex, byte));
    SHELL_CASE04(uint(Io::JoyReceive),     sio.joyrecv.write(kIndex, byte));
    SHELL_CASE04(uint(Io::JoyTransmit),    sio.joytrans.write(kIndex, byte));
    SHELL_CASE02(uint(Io::JoyStatus),      sio.joystat.write(kIndex, byte));
    SHELL_CASE02(uint(Io::IrqEnable),      interrupt.enable.write(kIndex, byte));
    SHELL_CASE02(uint(Io::IrqRequest),     interrupt.request.write(kIndex, byte));
    SHELL_CASE02(uint(Io::WaitControl),    waitcnt.write(kIndex, byte));
    SHELL_CASE04(uint(Io::IrqMaster),      interrupt.master.write(kIndex, byte));
    SHELL_CASE01(uint(Io::PostFlag),       postflg.write(kIndex, byte));
    SHELL_CASE01(uint(Io::HaltControl),    haltcnt.write(kIndex, byte));
    }
}
