#include "gpu.h"

#include <algorithm>

#include "constants.h"
#include "arm/arm.h"
#include "base/bit.h"
#include "base/macros.h"
#include "core/videocontext.h"
#include "dma/dma.h"

Gpu::Gpu()
{
    backgrounds[0].fill(kTransparent);
    backgrounds[1].fill(kTransparent);
    backgrounds[2].fill(kTransparent);
    backgrounds[3].fill(kTransparent);
}

void Gpu::scanline()
{
    io.dispstat.vblank = false;
    io.dispstat.hblank = false;

    if (io.dispcnt.blank)
    {
        u32* scanline = video_ctx.scanline(io.vcount.value);
        std::fill_n(scanline, kScreen.x, 0xFFFF'FFFF);
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
    if (io.dispcnt.layers & kLayerObj)
    {
        renderObjects();
    }

    switch (io.dispcnt.mode)
    {
    case 0:
        renderBg(&Gpu::renderBgMode0, 0);
        renderBg(&Gpu::renderBgMode0, 1);
        renderBg(&Gpu::renderBgMode0, 2);
        renderBg(&Gpu::renderBgMode0, 3);
        collapse(0, 4);
        break;

    case 1:
        renderBg(&Gpu::renderBgMode0, 0);
        renderBg(&Gpu::renderBgMode0, 1);
        renderBg(&Gpu::renderBgMode2, 2);
        collapse(0, 3);
        break;

    case 2:
        renderBg(&Gpu::renderBgMode2, 2);
        renderBg(&Gpu::renderBgMode2, 3);
        collapse(2, 4);
        break;

    case 3:
        renderBg(&Gpu::renderBgMode3, 2);
        collapse(2, 3);
        break;

    case 4:
        renderBg(&Gpu::renderBgMode4, 2);
        collapse(2, 3);
        break;

    case 5:
        renderBg(&Gpu::renderBgMode5, 2);
        collapse(2, 3);
        break;
    }
}

void Gpu::hblank()
{
    io.dispstat.hblank = true;
    io.dispstat.vblank = false;

    io.bgx[0].hblank(io.bgpb[0].value);
    io.bgx[1].hblank(io.bgpb[1].value);
    io.bgy[0].hblank(io.bgpd[0].value);
    io.bgy[1].hblank(io.bgpd[1].value);

    if (io.dispstat.hblank_irq)
    {
        arm.raise(kIrqHBlank);
    }
    dma.broadcast(DmaControl::kTimingHBlank);
}

void Gpu::vblank()
{
    io.dispstat.vblank = true;
    io.dispstat.hblank = false;

    io.bgx[0].vblank();
    io.bgx[1].vblank();
    io.bgy[0].vblank();
    io.bgy[1].vblank();

    if (io.dispstat.vblank_irq)
    {
        arm.raise(kIrqVBlank);
    }
    dma.broadcast(DmaControl::kTimingVBlank);
}

void Gpu::next()
{
    io.dispstat.vmatch = io.vcount.value == io.dispstat.vcompare;
    if (io.dispstat.vmatch && io.dispstat.vmatch_irq)
    {
        arm.raise(kIrqVMatch);
    }
    io.vcount.next();
}

void Gpu::present()
{
    io.dispstat.hblank = false;
    io.dispstat.vblank = false;

    if (io.dispcnt.isActive())
    {
        video_ctx.renderCopyTexture();
        video_ctx.renderPresent();
    }
}

void Gpu::mosaic(int bg)
{
    int mosaic_x = io.mosaic.bgs.x;
    if (mosaic_x == 1)
        return;

    u16 color;
    for (int x = 0; x < kScreen.x; ++x)
    {
        if (x % mosaic_x == 0)
        {
            color = backgrounds[bg][x];
        }
        backgrounds[bg][x] = color;
    }
}

bool Gpu::mosaicAffected(int bg) const
{
    return io.bgcnt[bg].mosaic && (io.mosaic.bgs.x > 1 || io.mosaic.bgs.y > 1);
}

bool Gpu::mosaicDominant() const
{
    return io.vcount.value % io.mosaic.bgs.y == 0;
}

u32 Gpu::argb(u16 color)
{
    return 0xFF00'0000
        | (color & (0x1F <<  0)) << 19
        | (color & (0x1F <<  5)) <<  6
        | (color & (0x1F << 10)) >>  7;
}

