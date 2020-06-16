#include "ppu.h"

#include <algorithm>

#include "base/bits.h"
#include "base/constants.h"
#include "base/macros.h"
#include "dma/dmac.h"
#include "interrupt/irqhandler.h"
#include "mmu/mmu.h"
#include "platform/videodevice.h"

PPU ppu;

void PPU::reset()
{
    io = PPUIO();

    for (auto& background : backgrounds)
    {
        background.fill(kTransparent);
        background.flip();
        background.fill(kTransparent);
    }
    objects.fill({});
    objects_exist = false;
    objects_alpha = false;
}

void PPU::scanline()
{
    io.dispstat.vblank = false;
    io.dispstat.hblank = false;

    if (io.dispcnt.blank)
    {
        u32* scanline = &video_device->buffer[kScreenW * io.vcount];
        std::fill_n(scanline, kScreenW, 0xFFFFFFFF);
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
    }
}

void PPU::hblank()
{
    io.dispstat.hblank = true;
    io.dispstat.vblank = false;

    io.bgx[0].hblank(io.bgpb[0]);
    io.bgx[1].hblank(io.bgpb[1]);
    io.bgy[0].hblank(io.bgpd[0]);
    io.bgy[1].hblank(io.bgpd[1]);

    if (io.dispstat.hblank_irq)
    {
        irqh.request(kIrqHBlank);
    }
    dmac.broadcast(DMA::Timing::HBlank);
}

void PPU::vblank()
{
    io.dispstat.vblank = true;
    io.dispstat.hblank = false;

    io.bgx[0].vblank();
    io.bgx[1].vblank();
    io.bgy[0].vblank();
    io.bgy[1].vblank();

    if (io.dispstat.vblank_irq)
    {
        irqh.request(kIrqVBlank);
    }
    dmac.broadcast(DMA::Timing::VBlank);
}

void PPU::next()
{
    io.dispstat.vmatch = io.vcount == io.dispstat.vcompare;
    if (io.dispstat.vmatch && io.dispstat.vmatch_irq)
    {
        irqh.request(kIrqVMatch);
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
    for (int x = 0; x < kScreenW; ++x)
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

u32 PPU::argb(u16 color)
{
    return 0xFF000000
        | (color & 0x001F) << 19
        | (color & 0x03E0) <<  6
        | (color & 0x7C00) >>  7;
}

