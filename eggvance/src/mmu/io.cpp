#include "io.h"

#include "memmap.h"
#include "mmu.h"
#include "apu/apu.h"
#include "arm/arm.h"
#include "common/config.h"
#include "ppu/ppu.h"
#include "registers/macros.h"
#include "system/dmacontroller.h"
#include "system/irqhandler.h"
#include "system/keypad.h"
#include "system/serial.h"
#include "system/timercontroller.h"

IO::IO()
{
    std::memset(masks, 0xFF, sizeof(masks));

    masks[REG_DISPCNT     + 0] = 0xF7;
    masks[REG_DISPSTAT    + 0] = 0x38;
    masks[REG_BG0CNT      + 1] = 0xDF;
    masks[REG_BG1CNT      + 1] = 0xDF;
    masks[REG_BG0HOFS     + 1] = 0x01;
    masks[REG_BG0VOFS     + 1] = 0x01;
    masks[REG_BG1HOFS     + 1] = 0x01;
    masks[REG_BG1VOFS     + 1] = 0x01;
    masks[REG_BG2HOFS     + 1] = 0x01;
    masks[REG_BG2VOFS     + 1] = 0x01;
    masks[REG_BG3HOFS     + 1] = 0x01;
    masks[REG_BG3VOFS     + 1] = 0x01;
    masks[REG_BG2X        + 3] = 0x0F;
    masks[REG_BG2Y        + 3] = 0x0F;
    masks[REG_BG3X        + 3] = 0x0F;
    masks[REG_BG3Y        + 3] = 0x0F;
    masks[REG_WININ       + 0] = 0x3F;
    masks[REG_WININ       + 1] = 0x3F;
    masks[REG_WINOUT      + 0] = 0x3F;
    masks[REG_WINOUT      + 1] = 0x3F;
    masks[REG_BLDCNT      + 1] = 0x3F;
    masks[REG_BLDALPHA    + 0] = 0x1F;
    masks[REG_BLDALPHA    + 1] = 0x1F;
    masks[REG_BLDY        + 0] = 0x1F;
    masks[REG_BLDY        + 1] = 0x00;
    masks[REG_DMA0SAD     + 3] = 0x07;
    masks[REG_DMA1SAD     + 3] = 0x0F;
    masks[REG_DMA2SAD     + 3] = 0x0F;
    masks[REG_DMA3SAD     + 3] = 0x0F;
    masks[REG_DMA0DAD     + 3] = 0x07;
    masks[REG_DMA1DAD     + 3] = 0x07;
    masks[REG_DMA2DAD     + 3] = 0x07;
    masks[REG_DMA3DAD     + 3] = 0x0F;
    masks[REG_DMA0CNT_L   + 1] = 0x3F;
    masks[REG_DMA0CNT_H   + 0] = 0xE0;
    masks[REG_DMA0CNT_H   + 1] = 0xF7;
    masks[REG_DMA1CNT_L   + 1] = 0x3F;
    masks[REG_DMA1CNT_H   + 0] = 0xE0;
    masks[REG_DMA1CNT_H   + 1] = 0xF7;
    masks[REG_DMA2CNT_L   + 1] = 0x3F;
    masks[REG_DMA2CNT_H   + 0] = 0xE0;
    masks[REG_DMA2CNT_H   + 1] = 0xF7;
    masks[REG_DMA3CNT_H   + 0] = 0xE0;
    masks[REG_TM0CNT_H    + 0] = 0xC7;
    masks[REG_TM0CNT_H    + 1] = 0x00;
    masks[REG_TM1CNT_H    + 0] = 0xC7;
    masks[REG_TM1CNT_H    + 1] = 0x00;
    masks[REG_TM2CNT_H    + 0] = 0xC7;
    masks[REG_TM2CNT_H    + 1] = 0x00;
    masks[REG_TM3CNT_H    + 0] = 0xC7;
    masks[REG_TM3CNT_H    + 1] = 0x00;
    masks[REG_SOUND1CNT_L + 0] = 0x7F;
    masks[REG_SOUND1CNT_L + 1] = 0x00;
    masks[REG_SOUND1CNT_H + 0] = 0xC0;
    masks[REG_SOUND1CNT_X + 0] = 0x00;
    masks[REG_SOUND1CNT_X + 1] = 0x40;
    masks[REG_SOUND2CNT_L + 0] = 0xC0;
    masks[REG_SOUND2CNT_H + 0] = 0x00;
    masks[REG_SOUND2CNT_H + 1] = 0x40;
    masks[REG_SOUND3CNT_L + 0] = 0xE0;
    masks[REG_SOUND3CNT_L + 1] = 0x00;
    masks[REG_SOUND3CNT_H + 0] = 0x00;
    masks[REG_SOUND3CNT_H + 1] = 0xE0;
    masks[REG_SOUND3CNT_X + 0] = 0x00;
    masks[REG_SOUND3CNT_X + 1] = 0x40;
    masks[REG_SOUND4CNT_L + 0] = 0x00;
    masks[REG_SOUND4CNT_H + 1] = 0x40;
    masks[REG_SOUNDCNT_L  + 0] = 0x77;
    masks[REG_SOUNDCNT_H  + 0] = 0x0F;
    masks[REG_SOUNDCNT_H  + 1] = 0x77;
    masks[REG_SOUNDCNT_X  + 0] = 0x80;
    masks[REG_SOUNDCNT_X  + 1] = 0x00;
    masks[REG_KEYCNT      + 1] = 0xC3;
    masks[REG_IE          + 1] = 0x3F;
    masks[REG_IF          + 1] = 0x3F;
    masks[REG_WAITCNT     + 1] = 0x5F;
    masks[REG_IME         + 0] = 0x01;
    masks[REG_IME         + 1] = 0x00;
}

void IO::reset()
{
    *this = IO();
}

u8 IO::readByte(u32 addr)
{
    u32 unused = addr;

    if (addr >= 0x400'0400)
    {
        if ((addr & 0xFFFC) == 0x800)
        {
            switch (addr & 0x3)
            {
            case 0: return io.memory_control.read<0>();
            case 1: return io.memory_control.read<1>();
            case 2: return io.memory_control.read<2>();
            case 3: return io.memory_control.read<3>();

            default:
                EGG_UNREACHABLE;
                return 0;
            }
        }
        else
        {
            return mmu.readUnused(unused);
        }
    }
    addr &= 0x3FF;

    switch (addr)
    {
    READ_HALF_REG(REG_GREENSWAP  , io.greenswap      );
    READ_HALF_REG(REG_DISPCNT    , ppu.io.dispcnt    );
    READ_HALF_REG(REG_DISPSTAT   , ppu.io.dispstat   );
    READ_HALF_REG(REG_VCOUNT     , ppu.io.vcount     );
    READ_HALF_REG(REG_BG0CNT     , ppu.io.bgcnt[0]   );
    READ_HALF_REG(REG_BG1CNT     , ppu.io.bgcnt[1]   );
    READ_HALF_REG(REG_BG2CNT     , ppu.io.bgcnt[2]   );
    READ_HALF_REG(REG_BG3CNT     , ppu.io.bgcnt[3]   );
    READ_HALF_REG(REG_WININ      , ppu.io.winin      );
    READ_HALF_REG(REG_WINOUT     , ppu.io.winout     );
    READ_HALF_REG(REG_BLDCNT     , ppu.io.bldcnt     );
    READ_HALF_REG(REG_BLDALPHA   , ppu.io.bldalpha   );
    READ_HALF_REG(REG_SOUND1CNT_L, apu.io.soundcnt1_l);
    READ_HALF_REG(REG_SOUND1CNT_H, apu.io.soundcnt1_h);
    READ_HALF_REG(REG_SOUND1CNT_X, apu.io.soundcnt1_x);
    READ_HALF_REG(REG_SOUND2CNT_L, apu.io.soundcnt2_l);
    READ_HALF_REG(REG_SOUND2CNT_H, apu.io.soundcnt2_h);
    READ_HALF_REG(REG_SOUND3CNT_L, apu.io.soundcnt3_l);
    READ_HALF_REG(REG_SOUND3CNT_H, apu.io.soundcnt3_h);
    READ_HALF_REG(REG_SOUND3CNT_X, apu.io.soundcnt3_x);
    READ_HALF_REG(REG_SOUND4CNT_L, apu.io.soundcnt4_l);
    READ_HALF_REG(REG_SOUND4CNT_H, apu.io.soundcnt4_h);
    READ_HALF_REG(REG_SOUNDCNT_L , apu.io.soundcnt_l );
    READ_HALF_REG(REG_SOUNDCNT_H , apu.io.soundcnt_h );
    READ_HALF_REG(REG_SOUNDCNT_X , apu.io.soundcnt_x );
    READ_HALF_REG(REG_SOUNDBIAS  , apu.io.soundbias  );
    READ_HALF_REG(REG_WAVE_RAM_0 , apu.io.wave_ram[0]);
    READ_HALF_REG(REG_WAVE_RAM_1 , apu.io.wave_ram[1]);
    READ_HALF_REG(REG_WAVE_RAM_2 , apu.io.wave_ram[2]);
    READ_HALF_REG(REG_WAVE_RAM_3 , apu.io.wave_ram[3]);
    READ_HALF_REG(REG_WAVE_RAM_4 , apu.io.wave_ram[4]);
    READ_HALF_REG(REG_WAVE_RAM_5 , apu.io.wave_ram[5]);
    READ_HALF_REG(REG_WAVE_RAM_6 , apu.io.wave_ram[6]);
    READ_HALF_REG(REG_WAVE_RAM_7 , apu.io.wave_ram[7]);
    READ_HALF_REG(REG_WAITCNT    , arm.io.waitcnt    );
    READ_HALF_REG(REG_KEYINPUT   , keypad.io.keyinput);
    READ_HALF_REG(REG_KEYCNT     , keypad.io.keycnt  );
    READ_HALF_REG(REG_SIOMULTI0  , sio.io.siomulti[0]);
    READ_HALF_REG(REG_SIOMULTI1  , sio.io.siomulti[1]);
    READ_HALF_REG(REG_SIOMULTI2  , sio.io.siomulti[2]);
    READ_HALF_REG(REG_SIOMULTI3  , sio.io.siomulti[3]);
    READ_HALF_REG(REG_SIOCNT     , sio.io.siocnt     );
    READ_HALF_REG(REG_SIOSEND    , sio.io.siosend    );
    READ_HALF_REG(REG_RCNT       , sio.io.rcnt       );
    READ_HALF_REG(REG_JOYCNT     , sio.io.joycnt     );
    READ_WORD_REG(REG_JOY_RECV   , sio.io.joyrecv    );
    READ_WORD_REG(REG_JOY_TRANS  , sio.io.joytrans   );
    READ_HALF_REG(REG_JOYSTAT    , sio.io.joystat    );
    READ_BYTE_REG(REG_POSTFLG    , io.postflag        );

    CASE_HALF_REG(REG_IE ):
    CASE_HALF_REG(REG_IF ):
    CASE_HALF_REG(REG_IME):
        return irqh.read(addr);

    CASE_HALF_REG(REG_DMA0CNT_L):
    CASE_HALF_REG(REG_DMA1CNT_L):
    CASE_HALF_REG(REG_DMA2CNT_L):
    CASE_HALF_REG(REG_DMA3CNT_L):
        return 0;

    CASE_HALF_REG(REG_DMA0CNT_H):
    CASE_HALF_REG(REG_DMA1CNT_H):
    CASE_HALF_REG(REG_DMA2CNT_H):
    CASE_HALF_REG(REG_DMA3CNT_H):
        return dmac.read(addr);

    CASE_HALF_REG(REG_TM0CNT_L):
    CASE_HALF_REG(REG_TM0CNT_H):
    CASE_HALF_REG(REG_TM1CNT_L):
    CASE_HALF_REG(REG_TM1CNT_H):
    CASE_HALF_REG(REG_TM2CNT_L):
    CASE_HALF_REG(REG_TM2CNT_H):
    CASE_HALF_REG(REG_TM3CNT_L):
    CASE_HALF_REG(REG_TM3CNT_H):
        return timerc.read(addr);

    CASE_HALF_REG(0x066):
    CASE_HALF_REG(0x06E):
    CASE_HALF_REG(0x076):
    CASE_HALF_REG(0x07A):
    CASE_HALF_REG(0x07E):
    CASE_HALF_REG(0x086):
    CASE_HALF_REG(0x08A):
    CASE_HALF_REG(0x136):
    CASE_HALF_REG(0x142):
    CASE_HALF_REG(0x15A):
    CASE_HALF_REG(0x206):
        return 0;

    default:
        return mmu.readUnused(unused);
    }
}

u16 IO::readHalf(u32 addr)
{
    addr &= ~0x1;
    return readByte(addr) | readByte(addr + 1) << 8;
}

u32 IO::readWord(u32 addr)
{
    addr &= ~0x3;
    return readHalf(addr) | readHalf(addr + 2) << 16;
}

void IO::writeByte(u32 addr, u8 byte)
{
    if (addr >= 0x400'0400)
    {
        if ((addr & 0xFFFC) == 0x800)
        {
            switch (addr & 0x3)
            {
            case 0: io.memory_control.write<0>(byte); break;
            case 1: io.memory_control.write<1>(byte); break;
            case 2: io.memory_control.write<2>(byte); break;
            case 3: io.memory_control.write<3>(byte); break;

            default:
                EGG_UNREACHABLE;
                break;
            }
        }
        return;
    }

    addr &= 0x3FF;
    byte &= masks[addr];

    switch (addr)
    {
    WRITE_HALF_REGX(REG_GREENSWAP, io.greenswap   , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_DISPCNT , ppu.io.dispcnt  , 0x0000'FFF7);
    WRITE_HALF_REGX(REG_DISPSTAT, ppu.io.dispstat , 0x0000'FF38);
    WRITE_HALF_REGX(REG_BG0CNT  , ppu.io.bgcnt[0] , 0x0000'DFFF);
    WRITE_HALF_REGX(REG_BG1CNT  , ppu.io.bgcnt[1] , 0x0000'DFFF);
    WRITE_HALF_REGX(REG_BG2CNT  , ppu.io.bgcnt[2] , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BG3CNT  , ppu.io.bgcnt[3] , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BG0HOFS , ppu.io.bghofs[0], 0x0000'01FF);
    WRITE_HALF_REGX(REG_BG0VOFS , ppu.io.bgvofs[0], 0x0000'01FF);
    WRITE_HALF_REGX(REG_BG1HOFS , ppu.io.bghofs[1], 0x0000'01FF);
    WRITE_HALF_REGX(REG_BG1VOFS , ppu.io.bgvofs[1], 0x0000'01FF);
    WRITE_HALF_REGX(REG_BG2HOFS , ppu.io.bghofs[2], 0x0000'01FF);
    WRITE_HALF_REGX(REG_BG2VOFS , ppu.io.bgvofs[2], 0x0000'01FF);
    WRITE_HALF_REGX(REG_BG3HOFS , ppu.io.bghofs[3], 0x0000'01FF);
    WRITE_HALF_REGX(REG_BG3VOFS , ppu.io.bgvofs[3], 0x0000'01FF);
    WRITE_HALF_REGX(REG_BG2PA   , ppu.io.bgpa[0]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BG2PB   , ppu.io.bgpb[0]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BG2PC   , ppu.io.bgpc[0]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BG2PD   , ppu.io.bgpd[0]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BG3PA   , ppu.io.bgpa[1]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BG3PB   , ppu.io.bgpb[1]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BG3PC   , ppu.io.bgpc[1]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BG3PD   , ppu.io.bgpd[1]  , 0x0000'FFFF);
    WRITE_WORD_REGX(REG_BG2X    , ppu.io.bgx[0]   , 0x0FFF'FFFF);
    WRITE_WORD_REGX(REG_BG2Y    , ppu.io.bgy[0]   , 0x0FFF'FFFF);
    WRITE_WORD_REGX(REG_BG3X    , ppu.io.bgx[1]   , 0x0FFF'FFFF);
    WRITE_WORD_REGX(REG_BG3Y    , ppu.io.bgy[1]   , 0x0FFF'FFFF);
    WRITE_HALF_REGX(REG_WIN0H   , ppu.io.winh[0]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WIN0V   , ppu.io.winv[0]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WIN1H   , ppu.io.winh[1]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WIN1V   , ppu.io.winv[1]  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WININ   , ppu.io.winin    , 0x0000'3F3F);
    WRITE_HALF_REGX(REG_WINOUT  , ppu.io.winout   , 0x0000'3F3F);
    WRITE_HALF_REGX(REG_MOSAIC  , ppu.io.mosaic   , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_BLDCNT  , ppu.io.bldcnt   , 0x0000'3FFF);
    WRITE_HALF_REGX(REG_BLDALPHA, ppu.io.bldalpha , 0x0000'1F1F);
    WRITE_HALF_REGX(REG_BLDY    , ppu.io.bldy     , 0x0000'001F);
    WRITE_BYTE_REGX(REG_HALTCNT , arm.io.haltcnt  , 0x0000'00FF);
    WRITE_HALF_REGX(REG_WAITCNT , arm.io.waitcnt  , 0x0000'FFFF);
    WRITE_HALF_REGX(REG_KEYCNT  , keypad.io.keycnt, 0x0000'FFFF);
    WRITE_HALF_REGX(REG_SOUND1CNT_L, apu.io.soundcnt1_l, 0x0000'007F);
    WRITE_HALF_REGX(REG_SOUND1CNT_H, apu.io.soundcnt1_h, 0x0000'FFC0);
    WRITE_HALF_REGX(REG_SOUND1CNT_X, apu.io.soundcnt1_x, 0x0000'4000);
    WRITE_HALF_REGX(REG_SOUND2CNT_L, apu.io.soundcnt2_l, 0x0000'FFC0);
    WRITE_HALF_REGX(REG_SOUND2CNT_H, apu.io.soundcnt2_h, 0x0000'4000);
    WRITE_HALF_REGX(REG_SOUND3CNT_L, apu.io.soundcnt3_l, 0x0000'00E0);
    WRITE_HALF_REGX(REG_SOUND3CNT_H, apu.io.soundcnt3_h, 0x0000'E000);
    WRITE_HALF_REGX(REG_SOUND3CNT_X, apu.io.soundcnt3_x, 0x0000'4000);
    WRITE_HALF_REGX(REG_SOUND4CNT_L, apu.io.soundcnt4_l, 0x0000'FF00);
    WRITE_HALF_REGX(REG_SOUND4CNT_H, apu.io.soundcnt4_h, 0x0000'40FF);
    WRITE_HALF_REGX(REG_SOUNDCNT_L, apu.io.soundcnt_l, 0x0000'FF77);
    WRITE_HALF_REGX(REG_SOUNDCNT_H, apu.io.soundcnt_h, 0x0000'770F);
    WRITE_HALF_REGX(REG_SOUNDCNT_X, apu.io.soundcnt_x, 0x0000'0080);
    WRITE_HALF_REGX(REG_SOUNDBIAS, apu.io.soundbias, 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WAVE_RAM_0, apu.io.wave_ram[0], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WAVE_RAM_1, apu.io.wave_ram[1], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WAVE_RAM_2, apu.io.wave_ram[2], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WAVE_RAM_3, apu.io.wave_ram[3], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WAVE_RAM_4, apu.io.wave_ram[4], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WAVE_RAM_5, apu.io.wave_ram[5], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WAVE_RAM_6, apu.io.wave_ram[6], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_WAVE_RAM_7, apu.io.wave_ram[7], 0x0000'FFFF);
    WRITE_WORD_REGX(REG_FIFO_A, apu.io.fifo_a, 0xFFFF'FFFF);
    WRITE_WORD_REGX(REG_FIFO_B, apu.io.fifo_b, 0xFFFF'FFFF);
    WRITE_HALF_REGX(REG_SIOMULTI0, sio.io.siomulti[0], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_SIOMULTI1, sio.io.siomulti[1], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_SIOMULTI2, sio.io.siomulti[2], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_SIOMULTI3, sio.io.siomulti[3], 0x0000'FFFF);
    WRITE_HALF_REGX(REG_SIOCNT, sio.io.siocnt, 0x0000'FFFF);
    WRITE_HALF_REGX(REG_SIOSEND, sio.io.siosend, 0x0000'FFFF);
    WRITE_HALF_REGX(REG_RCNT, sio.io.rcnt, 0x0000'FFFF);
    WRITE_HALF_REGX(REG_JOYCNT, sio.io.joycnt, 0x0000'FFFF);
    WRITE_WORD_REGX(REG_JOY_RECV, sio.io.joyrecv, 0xFFFF'FFFF);
    WRITE_WORD_REGX(REG_JOY_TRANS, sio.io.joytrans, 0xFFFF'FFFF);
    WRITE_HALF_REGX(REG_JOYSTAT, sio.io.joystat, 0x0000'FFFF);
    WRITE_BYTE_REGX(REG_POSTFLG, io.postflag, 0x0000'00FF);

    CASE2(REG_IE):
    CASE2(REG_IF):
    CASE2(REG_IME):
        irqh.write(addr, byte);
        break;

    CASE_WORD_REG(REG_DMA0SAD):
    CASE_WORD_REG(REG_DMA0DAD):
    CASE_WORD_REG(REG_DMA1SAD):
    CASE_WORD_REG(REG_DMA1DAD):
    CASE_WORD_REG(REG_DMA2SAD):
    CASE_WORD_REG(REG_DMA2DAD):
    CASE_WORD_REG(REG_DMA3SAD):
    CASE_WORD_REG(REG_DMA3DAD):
    CASE_HALF_REG(REG_DMA0CNT_L):
    CASE_HALF_REG(REG_DMA0CNT_H):
    CASE_HALF_REG(REG_DMA1CNT_L):
    CASE_HALF_REG(REG_DMA1CNT_H):
    CASE_HALF_REG(REG_DMA2CNT_L):
    CASE_HALF_REG(REG_DMA2CNT_H):
    CASE_HALF_REG(REG_DMA3CNT_L):
    CASE_HALF_REG(REG_DMA3CNT_H):
        dmac.write(addr, byte);
        break;

    CASE2(REG_TM0CNT_L):
    CASE1(REG_TM0CNT_H):
    CASE2(REG_TM1CNT_L):
    CASE1(REG_TM1CNT_H):
    CASE2(REG_TM2CNT_L):
    CASE1(REG_TM2CNT_H):
    CASE2(REG_TM3CNT_L):
    CASE1(REG_TM3CNT_H):
        timerc.write(addr, byte);
        break;
    }
}

void IO::writeHalf(u32 addr, u16 half)
{
    addr &= ~0x1;
    writeByte(addr + 0, (half >> 0) & 0xFF);
    writeByte(addr + 1, (half >> 8) & 0xFF);
}

void IO::writeWord(u32 addr, u32 word)
{
    addr &= ~0x3;
    writeHalf(addr + 0, (word >>  0) & 0xFFFF);
    writeHalf(addr + 2, (word >> 16) & 0xFFFF);
}
