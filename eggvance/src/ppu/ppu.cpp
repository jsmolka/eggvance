#include "ppu.h"

#include <algorithm>

#include "common/bits.h"
#include "common/constants.h"
#include "common/macros.h"
#include "mmu/mmu.h"
#include "platform/videodevice.h"
#include "system/dmacontroller.h"
#include "system/irqhandler.h"

PPU ppu;

void PPU::reset()
{
    io.reset();

    for (auto& background : backgrounds)
    {
        background.fill(TRANSPARENT);
        background.flip();
        background.fill(TRANSPARENT);
    }
    objects.fill({});
    objects_exist = false;
    objects_alpha = false;
}

void PPU::scanline()
{
    io.dispstat.vblank = false;
    io.dispstat.hblank = false;

    if (io.dispcnt.force_blank)
    {
        u32* scanline = &video_device->buffer[SCREEN_W * io.vcount];
        std::fill_n(scanline, SCREEN_W, 0xFFFFFFFF);
        return;
    }

    backgrounds[0].flip();
    backgrounds[1].flip();
    backgrounds[2].flip();
    backgrounds[3].flip();

    if (objects_exist)
    {
        objects.fill({});
        objects_exist = false;
        objects_alpha = false;
    }
    if (io.dispcnt.layers & LF_OBJ)
    {
        renderObjects();
    }

    switch (io.dispcnt.mode)
    {
    case 0:
        renderBg(&PPU::renderBgMode0, 0);
        renderBg(&PPU::renderBgMode0, 1);
        renderBg(&PPU::renderBgMode0, 2);
        renderBg(&PPU::renderBgMode0, 3);
        collapse(0, 4);
        break;

    case 1:
        renderBg(&PPU::renderBgMode0, 0);
        renderBg(&PPU::renderBgMode0, 1);
        renderBg(&PPU::renderBgMode2, 2);
        collapse(0, 3);
        break;

    case 2:
        renderBg(&PPU::renderBgMode2, 2);
        renderBg(&PPU::renderBgMode2, 3);
        collapse(2, 4);
        break;

    case 3:
        renderBg(&PPU::renderBgMode3, 2);
        collapse(2, 3);
        break;

    case 4:
        renderBg(&PPU::renderBgMode4, 2);
        collapse(2, 3);
        break;

    case 5:
        renderBg(&PPU::renderBgMode5, 2);
        collapse(2, 3);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void PPU::hblank()
{
    io.dispstat.hblank = true;
    io.dispstat.vblank = false;

    io.bgx[0].current += io.bgpb[0].value;
    io.bgx[1].current += io.bgpb[1].value;
    io.bgy[0].current += io.bgpd[0].value;
    io.bgy[1].current += io.bgpd[1].value;

    if (io.dispstat.hblank_irq)
    {
        irqh.request(IRQ::HBlank);
    }
    dmac.broadcast(DMA::Timing::HBlank);
}

void PPU::vblank()
{
    io.dispstat.vblank = true;
    io.dispstat.hblank = false;

    io.bgx[0].current = io.bgx[0].value;
    io.bgx[1].current = io.bgx[1].value;
    io.bgy[0].current = io.bgy[0].value;
    io.bgy[1].current = io.bgy[1].value;

    if (io.dispstat.vblank_irq)
    {
        irqh.request(IRQ::VBlank);
    }
    dmac.broadcast(DMA::Timing::VBlank);
}

void PPU::next()
{
    io.dispstat.vmatch = io.vcount == io.dispstat.vcompare;
    if (io.dispstat.vmatch && io.dispstat.vmatch_irq)
    {
        irqh.request(IRQ::VMatch);
    }
    io.vcount.next();
}

void PPU::present()
{
    if (io.dispcnt.isActive())
    {
        video_device->present();
    }
}

void PPU::mosaic(int bg)
{
    int mosaic_x = io.mosaic.bgs.x;
    if (mosaic_x == 1)
        return;

    u16 color;
    for (int x = 0; x < SCREEN_W; ++x)
    {
        if (x % mosaic_x == 0)
        {
            color = backgrounds[bg][x];
        }
        backgrounds[bg][x] = color;
    }
}

bool PPU::mosaicAffected(int bg) const
{
    return io.bgcnt[bg].mosaic && (io.mosaic.bgs.x > 1 || io.mosaic.bgs.y > 1);
}

bool PPU::mosaicDominant() const
{
    return io.vcount % io.mosaic.bgs.y == 0;
}

u16 PPU::blendAlpha(u16 a, u16 b) const
{
    int ar = bits< 0, 5>(a);
    int ag = bits< 5, 5>(a);
    int ab = bits<10, 5>(a);

    int br = bits< 0, 5>(b);
    int bg = bits< 5, 5>(b);
    int bb = bits<10, 5>(b);

    int tr = std::min(31, (ar * io.bldalpha.eva + br * io.bldalpha.evb) >> 4);
    int tg = std::min(31, (ag * io.bldalpha.eva + bg * io.bldalpha.evb) >> 4);
    int tb = std::min(31, (ab * io.bldalpha.eva + bb * io.bldalpha.evb) >> 4);

    return (tr << 0) | (tg << 5) | (tb << 10);
}

u16 PPU::blendWhite(u16 a) const
{
    int ar = bits< 0, 5>(a);
    int ag = bits< 5, 5>(a);
    int ab = bits<10, 5>(a);

    int tr = std::min(31, ar + (((31 - ar) * io.bldy.evy) >> 4));
    int tg = std::min(31, ag + (((31 - ag) * io.bldy.evy) >> 4));
    int tb = std::min(31, ab + (((31 - ab) * io.bldy.evy) >> 4));

    return (tr << 0) | (tg << 5) | (tb << 10);
}

u16 PPU::blendBlack(u16 a) const
{
    int ar = bits< 0, 5>(a);
    int ag = bits< 5, 5>(a);
    int ab = bits<10, 5>(a);

    int tr = std::min(31, ar - ((ar * io.bldy.evy) >> 4));
    int tg = std::min(31, ag - ((ag * io.bldy.evy) >> 4));
    int tb = std::min(31, ab - ((ab * io.bldy.evy) >> 4));

    return (tr << 0) | (tg << 5) | (tb << 10);
}

u32 PPU::argb(u16 color)
{
    return 0xFF000000
        | (color & 0x001F) << 19
        | (color & 0x03E0) <<  6
        | (color & 0x7C00) >>  7;
}

