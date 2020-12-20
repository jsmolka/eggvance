#include "arm.h"

#include "apu/apu.h"
#include "base/macros.h"
#include "dma/dma.h"
#include "gamepad/gamepad.h"
#include "gamepak/gamepak.h"
#include "ppu/ppu.h"
#include "sio/sio.h"
#include "timer/timer.h"

enum Region
{
    kRegionBios,
    kRegionUnused,
    kRegionExternalWorkRam,
    kRegionInternalWorkRam,
    kRegionIo,
    kRegionPaletteRam,
    kRegionVideoRam,
    kRegionOam,
    kRegionGamePak0L,
    kRegionGamePak0H,
    kRegionGamePak1L,
    kRegionGamePak1H,
    kRegionGamePak2L,
    kRegionGamePak2H,
    kRegionSaveL,
    kRegionSaveH
};

u32 Arm::readUnused() const
{
    if (cpsr.t == 0)
        return pipe[1];

    switch (pc >> 24)
    {
    case kRegionBios:
    case kRegionOam:
        return (pipe[1] << 16) | pipe[0];

    case kRegionInternalWorkRam:
        return (pipe[1] << 16) | pipe[(pc >> 1) ^ 0x1];

    default:
        return (pipe[1] << 16) | pipe[1];
    }
}

u8 Arm::readByte(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    u8  byte;
    int wait;

    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
        {
            byte = bios.readByte(addr);
            wait = 1;
            break;
        }
        [[fallthrough]];

    case kRegionUnused:
        byte = readUnused() >> (8 * (addr & 0x3));
        wait = 1;
        break;

    case kRegionExternalWorkRam:
        byte = ewram.readByte(addr);
        wait = 3;
        break;

    case kRegionInternalWorkRam:
        byte = iwram.readByte(addr);
        wait = 1;
        break;

    case kRegionIo:
        byte = readIo(addr);
        wait = 1;
        break;

    case kRegionPaletteRam:
        byte = ppu.pram.readByte(addr);
        wait = 1;
        break;

    case kRegionVideoRam:
        byte = ppu.vram.readByte(addr);
        wait = 1;
        break;

    case kRegionOam:
        byte = ppu.oam.readByte(addr);
        wait = 1;
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        byte = gamepak.read<u8>(addr);
        wait = waitcnt.cyclesHalf(addr, access);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        byte = gamepak.readSave(addr);
        wait = waitcnt.cyclesHalf(addr, access);
        break;

    default:
        byte = readUnused() >> (8 * (addr & 0x3));
        wait = 1;
        break;
    }

    clock(wait);

    return byte;
}

u16 Arm::readHalf(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    u16 half;
    int wait;
    
    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
        {
            half = bios.readHalf(addr);
            wait = 1;
            break;
        }
        [[fallthrough]];

    case kRegionUnused:
        half = readUnused() >> (8 * (addr & 0x2));
        wait = 1;
        break;

    case kRegionExternalWorkRam:
        half = ewram.readHalf(addr);
        wait = 3;
        break;

    case kRegionInternalWorkRam:
        half = iwram.readHalf(addr);
        wait = 1;
        break;

    case kRegionIo:
        addr &= ~0x1;
        half = 0;
        half |= readIo(addr + 0) << 0;
        half |= readIo(addr + 1) << 8;
        wait = 1;
        break;

    case kRegionPaletteRam:
        half = ppu.pram.readHalf(addr);
        wait = 1;
        break;

    case kRegionVideoRam:
        half = ppu.vram.readHalf(addr);
        wait = 1;
        break;

    case kRegionOam:
        half = ppu.oam.readHalf(addr);
        wait = 1;
        break;

    case kRegionGamePak2H:
        if (gamepak.isEepromAccess(addr))
        {
            half = 1;
            wait = waitcnt.cyclesHalf(addr, access);
            break;
        }
        [[fallthrough]];

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        half = gamepak.read<u16>(addr);
        wait = waitcnt.cyclesHalf(addr, access);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        half = gamepak.readSave(addr) * 0x0101;
        wait = waitcnt.cyclesHalf(addr, access);
        break;

    default:
        half = readUnused() >> (8 * (addr & 0x2));
        wait = 1;
        break;
    }

    clock(wait);

    return half;
}

u32 Arm::readWord(u32 addr, Access access)
{
    pipe.access = Access::NonSequential;

    u32 word;
    int wait;

    switch (addr >> 24)
    {
    case kRegionBios:
        if (addr < Bios::kSize)
        {
            word = bios.readWord(addr);
            wait = 1;
            break;
        }
        [[fallthrough]];

    case kRegionUnused:
        word = readUnused();
        wait = 1;
        break;

    case kRegionExternalWorkRam:
        word = ewram.readWord(addr);
        wait = 6;
        break;

    case kRegionInternalWorkRam:
        word = iwram.readWord(addr);
        wait = 1;
        break;

    case kRegionIo:
        addr &= ~0x3;
        word = 0;
        word |= readIo(addr + 0) <<  0;
        word |= readIo(addr + 1) <<  8;
        word |= readIo(addr + 2) << 16;
        word |= readIo(addr + 3) << 24;
        wait = 1;
        break;

    case kRegionPaletteRam:
        word = ppu.pram.readWord(addr);
        wait = 2;
        break;

    case kRegionVideoRam:
        word = ppu.vram.readWord(addr);
        wait = 2;
        break;

    case kRegionOam:
        word = ppu.oam.readWord(addr);
        wait = 1;
        break;

    case kRegionGamePak2H:
        if (gamepak.isEepromAccess(addr))
        {
            word = 1;
            wait = waitcnt.cyclesWord(addr, access);
            break;
        }
        [[fallthrough]];

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
        word = gamepak.read<u32>(addr);
        wait = waitcnt.cyclesWord(addr, access);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        word = gamepak.readSave(addr) * 0x0101'0101;
        wait = waitcnt.cyclesWord(addr, access);
        break;

    default:
        word = readUnused();
        wait = 1;
        break;
    }

    clock(wait);

    return word;
}

void Arm::writeByte(u32 addr, u8 byte, Access access)
{
    pipe.access = Access::NonSequential;

    int wait;
    
    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        wait = 1;
        break;

    case kRegionExternalWorkRam:
        ewram.writeByte(addr, byte);
        wait = 3;
        break;

    case kRegionInternalWorkRam:
        iwram.writeByte(addr, byte);
        wait = 1;
        break;

    case kRegionIo:
        writeIo(addr, byte);
        wait = 1;
        break;

    case kRegionPaletteRam:
        ppu.pram.writeByte(addr, byte);
        wait = 1;
        break;

    case kRegionVideoRam:
        ppu.vram.writeByte(addr, byte);
        wait = 1;
        break;

    case kRegionOam:
        wait = 1;
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        gamepak.write<u8>(addr, byte);
        wait = waitcnt.cyclesHalf(addr, access);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        gamepak.writeSave(addr, byte);
        wait = waitcnt.cyclesHalf(addr, access);
        break;

    default:
        wait = 1;
        break;
    }

    clock(wait);
}

void Arm::writeHalf(u32 addr, u16 half, Access access)
{
    pipe.access = Access::NonSequential;
    
    int wait;

    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        wait = 1;
        break;

    case kRegionExternalWorkRam:
        ewram.writeHalf(addr, half);
        wait = 3;
        break;

    case kRegionInternalWorkRam:
        iwram.writeHalf(addr, half);
        wait = 1;
        break;

    case kRegionIo:
        addr &= ~0x1;
        writeIo(addr + 0, bit::seq<0, 8>(half));
        writeIo(addr + 1, bit::seq<8, 8>(half));
        wait = 1;
        break;

    case kRegionPaletteRam:
        ppu.pram.writeHalf(addr, half);
        wait = 1;
        break;

    case kRegionVideoRam:
        ppu.vram.writeHalf(addr, half);
        wait = 1;
        break;

    case kRegionOam:
        ppu.oam.writeHalf(addr, half);
        wait = 1;
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        gamepak.write<u16>(addr, half);
        wait = waitcnt.cyclesHalf(addr, access);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        gamepak.writeSave(addr, half >> (8 * (addr & 0x1)));
        wait = waitcnt.cyclesHalf(addr, access);
        break;

    default:
        wait = 1;
        break;
    }

    clock(wait);
}

void Arm::writeWord(u32 addr, u32 word, Access access)
{
    pipe.access = Access::NonSequential;
    
    int wait;

    switch (addr >> 24)
    {
    case kRegionBios:
    case kRegionUnused:
        wait = 1;
        break;

    case kRegionExternalWorkRam:
        ewram.writeWord(addr, word);
        wait = 6;
        break;

    case kRegionInternalWorkRam:
        iwram.writeWord(addr, word);
        wait = 1;
        break;

    case kRegionIo:
        addr &= ~0x3;
        writeIo(addr + 0, bit::seq< 0, 8>(word));
        writeIo(addr + 1, bit::seq< 8, 8>(word));
        writeIo(addr + 2, bit::seq<16, 8>(word));
        writeIo(addr + 3, bit::seq<24, 8>(word));
        wait = 1;
        break;

    case kRegionPaletteRam:
        ppu.pram.writeWord(addr, word);
        wait = 2;
        break;

    case kRegionVideoRam:
        ppu.vram.writeWord(addr, word);
        wait = 2;
        break;

    case kRegionOam:
        ppu.oam.writeWord(addr, word);
        wait = 1;
        break;

    case kRegionGamePak0L:
    case kRegionGamePak0H:
    case kRegionGamePak1L:
    case kRegionGamePak1H:
    case kRegionGamePak2L:
    case kRegionGamePak2H:
        gamepak.write<u32>(addr, word);
        wait = waitcnt.cyclesWord(addr, access);
        break;

    case kRegionSaveL:
    case kRegionSaveH:
        gamepak.writeSave(addr, word >> (8 * (addr & 0x3)));
        wait = waitcnt.cyclesWord(addr, access);
        break;

    default:
        wait = 1;
        break;
    }

    clock(wait);
}

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
    kRegSound1ControlH = 0x062,
    kRegSound1ControlX = 0x064,
    kRegUnused066      = 0x066,
    kRegSound2ControlL = 0x068,
    kRegSound2ControlH = 0x06C,
    kRegUnused06E      = 0x06E,
    kRegSound3ControlL = 0x070,
    kRegSound3ControlH = 0x072,
    kRegSound3ControlX = 0x074,
    kRegUnused076      = 0x076,
    kRegSound4ControlL = 0x078,
    kRegUnused07A      = 0x07A,
    kRegSound4ControlH = 0x07C,
    kRegUnused07E      = 0x07E,
    kRegSoundControlL  = 0x080,
    kRegSoundControlH  = 0x082,
    kRegSoundControlX  = 0x084,
    kRegUnused086      = 0x086,
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
    INDEXED_CASE2(kRegDisplayControl, return ppu.dispcnt.read<kIndex>());
    INDEXED_CASE2(kRegGreenSwap,      return ppu.greenswap.read<kIndex>());
    INDEXED_CASE2(kRegDisplayStatus,  return ppu.dispstat.read<kIndex>());
    INDEXED_CASE2(kRegVerticalCount,  return ppu.vcount.read<kIndex>());
    INDEXED_CASE2(kRegBg0Control,     return ppu.bgcnt[0].read<kIndex>());
    INDEXED_CASE2(kRegBg1Control,     return ppu.bgcnt[1].read<kIndex>());
    INDEXED_CASE2(kRegBg2Control,     return ppu.bgcnt[2].read<kIndex>());
    INDEXED_CASE2(kRegBg3Control,     return ppu.bgcnt[3].read<kIndex>());
    INDEXED_CASE2(kRegWindowInside,   return ppu.winin.read<kIndex>());
    INDEXED_CASE2(kRegWindowOutside,  return ppu.winout.read<kIndex>());
    INDEXED_CASE2(kRegBlendControl,   return ppu.bldcnt.read<kIndex>());
    INDEXED_CASE2(kRegBlendAlpha,     return ppu.bldalpha.read<kIndex>());
    INDEXED_CASE2(kRegSound1ControlL, return apu.soundcnt1l.read<kIndex>());
    INDEXED_CASE2(kRegSound1ControlH, return apu.soundcnt1h.read<kIndex>());
    INDEXED_CASE2(kRegSound1ControlX, return apu.soundcnt1x.read<kIndex>());
    INDEXED_CASE2(kRegUnused066,      return 0);
    INDEXED_CASE2(kRegSound2ControlL, return apu.soundcnt2l.read<kIndex>());
    INDEXED_CASE2(kRegSound2ControlH, return apu.soundcnt2h.read<kIndex>());
    INDEXED_CASE2(kRegUnused06E,      return 0);
    INDEXED_CASE2(kRegSound3ControlL, return apu.soundcnt3l.read<kIndex>());
    INDEXED_CASE2(kRegSound3ControlH, return apu.soundcnt3h.read<kIndex>());
    INDEXED_CASE2(kRegSound3ControlX, return apu.soundcnt3x.read<kIndex>());
    INDEXED_CASE2(kRegUnused076,      return 0);
    INDEXED_CASE2(kRegSound4ControlL, return apu.soundcnt4l.read<kIndex>());
    INDEXED_CASE2(kRegUnused07A,      return 0);
    INDEXED_CASE2(kRegSound4ControlH, return apu.soundcnt4h.read<kIndex>());
    INDEXED_CASE2(kRegUnused07E,      return 0);
    INDEXED_CASE2(kRegSoundControlL,  return apu.soundcntl.read<kIndex>());
    INDEXED_CASE2(kRegSoundControlH,  return apu.soundcnth.read<kIndex>());
    INDEXED_CASE2(kRegSoundControlX,  return apu.soundcntx.read<kIndex>());
    INDEXED_CASE2(kRegUnused086,      return 0);
    INDEXED_CASE2(kRegSoundBias,      return apu.soundbias.read<kIndex>());
    INDEXED_CASE2(kRegUnused08A,      return 0);
    INDEXED_CASE2(kRegWaveRam0,       return apu.waveram[0].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam1,       return apu.waveram[1].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam2,       return apu.waveram[2].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam3,       return apu.waveram[3].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam4,       return apu.waveram[4].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam5,       return apu.waveram[5].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam6,       return apu.waveram[6].read<kIndex>());
    INDEXED_CASE2(kRegWaveRam7,       return apu.waveram[7].read<kIndex>());
    INDEXED_CASE2(kRegDma0Count,      return 0);
    INDEXED_CASE2(kRegDma0Control,    return dma.channels[0].control.read<kIndex>());
    INDEXED_CASE2(kRegDma1Count,      return 0);
    INDEXED_CASE2(kRegDma1Control,    return dma.channels[1].control.read<kIndex>());
    INDEXED_CASE2(kRegDma2Count,      return 0);
    INDEXED_CASE2(kRegDma2Control,    return dma.channels[2].control.read<kIndex>());
    INDEXED_CASE2(kRegDma3Count,      return 0);
    INDEXED_CASE2(kRegDma3Control,    return dma.channels[3].control.read<kIndex>());
    INDEXED_CASE2(kRegTimer0Count,    return timer.channels[0].count.read<kIndex>());
    INDEXED_CASE2(kRegTimer0Control,  return timer.channels[0].control.read<kIndex>());
    INDEXED_CASE2(kRegTimer1Count,    return timer.channels[1].count.read<kIndex>());
    INDEXED_CASE2(kRegTimer1Control,  return timer.channels[1].control.read<kIndex>());
    INDEXED_CASE2(kRegTimer2Count,    return timer.channels[2].count.read<kIndex>());
    INDEXED_CASE2(kRegTimer2Control,  return timer.channels[2].control.read<kIndex>());
    INDEXED_CASE2(kRegTimer3Count,    return timer.channels[3].count.read<kIndex>());
    INDEXED_CASE2(kRegTimer3Control,  return timer.channels[3].control.read<kIndex>());
    INDEXED_CASE2(kRegSioMulti0,      return sio.siomulti[0].read<kIndex>());
    INDEXED_CASE2(kRegSioMulti1,      return sio.siomulti[1].read<kIndex>());
    INDEXED_CASE2(kRegSioMulti2,      return sio.siomulti[2].read<kIndex>());
    INDEXED_CASE2(kRegSioMulti3,      return sio.siomulti[3].read<kIndex>());
    INDEXED_CASE2(kRegSioControl,     return sio.siocnt.read<kIndex>());
    INDEXED_CASE2(kRegSioSend,        return sio.siosend.read<kIndex>());
    INDEXED_CASE2(kRegKeyInput,       return gamepad.input.read<kIndex>());
    INDEXED_CASE2(kRegKeyControl,     return gamepad.control.read<kIndex>());
    INDEXED_CASE2(kRegRemoteControl,  return sio.rcnt.read<kIndex>());
    INDEXED_CASE2(kRegUnused136,      return 0);
    INDEXED_CASE2(kRegJoyControl,     return sio.joycnt.read<kIndex>());
    INDEXED_CASE2(kRegUnused142,      return 0);
    INDEXED_CASE4(kRegJoyReceive,     return sio.joyrecv.read<kIndex>());
    INDEXED_CASE4(kRegJoyTransmit,    return sio.joytrans.read<kIndex>());
    INDEXED_CASE2(kRegJoyStatus,      return sio.joystat.read<kIndex>());
    INDEXED_CASE2(kRegUnused15A,      return 0);
    INDEXED_CASE2(kRegIrqEnable,      return irq.enable.read<kIndex>());
    INDEXED_CASE2(kRegIrqRequest,     return irq.request.read<kIndex>());
    INDEXED_CASE2(kRegWaitControl,    return waitcnt.read<kIndex>());
    INDEXED_CASE2(kRegUnused206,      return 0);
    INDEXED_CASE4(kRegIrqMaster,      return irq.master.read<kIndex>());
    INDEXED_CASE1(kRegPostFlag,       return postflag.read<kIndex>());

    default:
        return readUnused() >> (8 * (addr & 0x3));
    }
}

void Arm::writeIo(u32 addr, u8 byte)
{
    switch (addr & 0x3FF'FFFF)
    {
    INDEXED_CASE2(kRegDisplayControl, ppu.dispcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegGreenSwap,      ppu.greenswap.write<kIndex>(byte));
    INDEXED_CASE2(kRegDisplayStatus,  ppu.dispstat.write<kIndex>(byte));
    INDEXED_CASE2(kRegBg0Control,     ppu.bgcnt[0].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kRegBg1Control,     ppu.bgcnt[1].write<kIndex, 0xDFFF>(byte));
    INDEXED_CASE2(kRegBg2Control,     ppu.bgcnt[2].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kRegBg3Control,     ppu.bgcnt[3].write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kRegBg0HorOffset,   ppu.bghofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg0VerOffset,   ppu.bgvofs[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg1HorOffset,   ppu.bghofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg1VerOffset,   ppu.bgvofs[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2HorOffset,   ppu.bghofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2VerOffset,   ppu.bgvofs[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3HorOffset,   ppu.bghofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3VerOffset,   ppu.bgvofs[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterA,  ppu.bgpa[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterB,  ppu.bgpb[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterC,  ppu.bgpc[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg2ParameterD,  ppu.bgpd[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg2ReferenceX,  ppu.bgx[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg2ReferenceY,  ppu.bgy[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterA,  ppu.bgpa[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterB,  ppu.bgpb[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterC,  ppu.bgpc[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegBg3ParameterD,  ppu.bgpd[1].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg3ReferenceX,  ppu.bgx[1].write<kIndex>(byte));
    INDEXED_CASE4(kRegBg3ReferenceY,  ppu.bgy[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow0Hor,     ppu.winh[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow1Hor,     ppu.winh[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow0Ver,     ppu.winv[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindow1Ver,     ppu.winv[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWindowInside,   ppu.winin.write<kIndex>(byte));
    INDEXED_CASE2(kRegWindowOutside,  ppu.winout.write<kIndex>(byte));
    INDEXED_CASE2(kRegMosaic,         ppu.mosaic.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendControl,   ppu.bldcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendAlpha,     ppu.bldalpha.write<kIndex>(byte));
    INDEXED_CASE2(kRegBlendFade,      ppu.bldfade.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound1ControlL, apu.soundcnt1l.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound1ControlH, apu.soundcnt1h.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound1ControlX, apu.soundcnt1x.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound2ControlL, apu.soundcnt2l.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound2ControlH, apu.soundcnt2h.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound3ControlL, apu.soundcnt3l.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound3ControlH, apu.soundcnt3h.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound3ControlX, apu.soundcnt3x.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound4ControlL, apu.soundcnt4l.write<kIndex>(byte));
    INDEXED_CASE2(kRegSound4ControlH, apu.soundcnt4h.write<kIndex>(byte));
    INDEXED_CASE2(kRegSoundControlL,  apu.soundcntl.write<kIndex>(byte));
    INDEXED_CASE2(kRegSoundControlH,  apu.soundcnth.write<kIndex>(byte));
    INDEXED_CASE2(kRegSoundControlX,  apu.soundcntx.write<kIndex>(byte));
    INDEXED_CASE2(kRegSoundBias,      apu.soundbias.write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam0,       apu.waveram[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam1,       apu.waveram[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam2,       apu.waveram[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam3,       apu.waveram[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam4,       apu.waveram[4].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam5,       apu.waveram[5].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam6,       apu.waveram[6].write<kIndex>(byte));
    INDEXED_CASE2(kRegWaveRam7,       apu.waveram[7].write<kIndex>(byte));
    INDEXED_CASE4(kRegFifoA,          apu.fifo[0].write<kIndex>(byte));
    INDEXED_CASE4(kRegFifoB,          apu.fifo[1].write<kIndex>(byte));
    INDEXED_CASE4(kRegDma0Sad,        dma.channels[0].sad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_CASE4(kRegDma0Dad,        dma.channels[0].dad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_CASE2(kRegDma0Count,      dma.channels[0].count.write<kIndex, 0x3FFF>(byte));
    INDEXED_CASE2(kRegDma0Control,    dma.channels[0].control.write<kIndex, 0xF7E0>(byte));
    INDEXED_CASE4(kRegDma1Sad,        dma.channels[1].sad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_CASE4(kRegDma1Dad,        dma.channels[1].dad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_CASE2(kRegDma1Count,      dma.channels[1].count.write<kIndex, 0x3FFF>(byte));
    INDEXED_CASE2(kRegDma1Control,    dma.channels[1].control.write<kIndex, 0xF7E0>(byte));
    INDEXED_CASE4(kRegDma2Sad,        dma.channels[2].sad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_CASE4(kRegDma2Dad,        dma.channels[2].dad.write<kIndex, 0x07FF'FFFF>(byte));
    INDEXED_CASE2(kRegDma2Count,      dma.channels[2].count.write<kIndex, 0x3FFF>(byte));
    INDEXED_CASE2(kRegDma2Control,    dma.channels[2].control.write<kIndex, 0xF7E0>(byte));
    INDEXED_CASE4(kRegDma3Sad,        dma.channels[3].sad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_CASE4(kRegDma3Dad,        dma.channels[3].dad.write<kIndex, 0x0FFF'FFFF>(byte));
    INDEXED_CASE2(kRegDma3Count,      dma.channels[3].count.write<kIndex, 0xFFFF>(byte));
    INDEXED_CASE2(kRegDma3Control,    dma.channels[3].control.write<kIndex, 0xFFE0>(byte));
    INDEXED_CASE2(kRegTimer0Count,    timer.channels[0].count.write<kIndex>(byte));
    INDEXED_CASE1(kRegTimer0Control,  timer.channels[0].control.write<kIndex>(byte));
    INDEXED_CASE2(kRegTimer1Count,    timer.channels[1].count.write<kIndex>(byte));
    INDEXED_CASE1(kRegTimer1Control,  timer.channels[1].control.write<kIndex>(byte));
    INDEXED_CASE2(kRegTimer2Count,    timer.channels[2].count.write<kIndex>(byte));
    INDEXED_CASE1(kRegTimer2Control,  timer.channels[2].control.write<kIndex>(byte));
    INDEXED_CASE2(kRegTimer3Count,    timer.channels[3].count.write<kIndex>(byte));
    INDEXED_CASE1(kRegTimer3Control,  timer.channels[3].control.write<kIndex>(byte));
    INDEXED_CASE2(kRegSioMulti0,      sio.siomulti[0].write<kIndex>(byte));
    INDEXED_CASE2(kRegSioMulti1,      sio.siomulti[1].write<kIndex>(byte));
    INDEXED_CASE2(kRegSioMulti2,      sio.siomulti[2].write<kIndex>(byte));
    INDEXED_CASE2(kRegSioMulti3,      sio.siomulti[3].write<kIndex>(byte));
    INDEXED_CASE2(kRegSioControl,     sio.siocnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegSioSend,        sio.siosend.write<kIndex>(byte));
    INDEXED_CASE2(kRegKeyControl,     gamepad.control.write<kIndex>(byte));
    INDEXED_CASE2(kRegRemoteControl,  sio.rcnt.write<kIndex>(byte));
    INDEXED_CASE2(kRegJoyControl,     sio.joycnt.write<kIndex>(byte));
    INDEXED_CASE4(kRegJoyReceive,     sio.joyrecv.write<kIndex>(byte));
    INDEXED_CASE4(kRegJoyTransmit,    sio.joytrans.write<kIndex>(byte));
    INDEXED_CASE2(kRegJoyStatus,      sio.joystat.write<kIndex>(byte));
    INDEXED_CASE2(kRegIrqEnable,      irq.enable.write<kIndex>(byte));
    INDEXED_CASE2(kRegIrqRequest,     irq.request.write<kIndex>(byte));
    INDEXED_CASE2(kRegWaitControl,    waitcnt.write<kIndex>(byte));
    INDEXED_CASE4(kRegIrqMaster,      irq.master.write<kIndex>(byte));
    INDEXED_CASE1(kRegPostFlag,       postflag.write<kIndex>(byte));
    INDEXED_CASE1(kRegHaltControl,    haltcnt.write<kIndex>(byte));
    }
}

u32 Arm::readHalfRotate(u32 addr, Access access)
{
    u32 value = readHalf(addr, access);
    return bit::ror(value, 8 * (addr & 0x1));
}

u32 Arm::readWordRotate(u32 addr, Access access)
{
    u32 value = readWord(addr, access);
    return bit::ror(value, 8 * (addr & 0x3));
}

u32 Arm::readByteSignEx(u32 addr, Access access)
{
    u32 value = readByte(addr, access);
    return bit::signEx<8>(value);
}

u32 Arm::readHalfSignEx(u32 addr, Access access)
{
    if (addr & 0x1)
    {
        u32 value = readByte(addr, access);
        return bit::signEx<8>(value);
    }
    else
    {
        u32 value = readHalf(addr, access);
        return bit::signEx<16>(value);
    }
}
