#include "gpu.h"

#include <algorithm>

#include "constants.h"
#include "arm/arm.h"
#include "arm/constants.h"
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

u32 Gpu::argb(u16 color)
{
    return 0xFF00'0000
        | (color & (0x1F <<  0)) << 19
        | (color & (0x1F <<  5)) <<  6
        | (color & (0x1F << 10)) >>  7;
}

void Gpu::scanline()
{
    dispstat.vblank = false;
    dispstat.hblank = false;

    if (dispcnt.blank)
    {
        u32* scanline = video_ctx.scanline(vcount.value);
        std::fill_n(scanline, kScreen.x, 0xFFFF'FFFF);
        return;
    }

    backgrounds[0].flip();
    backgrounds[1].flip();
    backgrounds[2].flip();
    backgrounds[3].flip();

    if (objects_exist)
    {
        objects.fill(ObjectLayer());
        objects_exist = false;
        objects_alpha = false;
    }

    if (dispcnt.layers & kLayerObj)
    {
        renderObjects();
    }

    switch (dispcnt.mode)
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
    dispstat.hblank = true;
    dispstat.vblank = false;

    bgx[0].hblank(bgpb[0].value);
    bgx[1].hblank(bgpb[1].value);
    bgy[0].hblank(bgpd[0].value);
    bgy[1].hblank(bgpd[1].value);

    if (dispstat.hblank_irq)
    {
        arm.raise(kIrqHBlank);
    }
    dma.broadcast(DmaControl::kTimingHBlank);
}

void Gpu::vblank()
{
    dispstat.vblank = true;
    dispstat.hblank = false;

    bgx[0].vblank();
    bgx[1].vblank();
    bgy[0].vblank();
    bgy[1].vblank();

    if (dispstat.vblank_irq)
    {
        arm.raise(kIrqVBlank);
    }
    dma.broadcast(DmaControl::kTimingVBlank);
}

void Gpu::next()
{
    dispstat.vmatch = vcount.value == dispstat.vcompare;
    if (dispstat.vmatch && dispstat.vmatch_irq)
    {
        arm.raise(kIrqVMatch);
    }
    vcount.next();
}

void Gpu::present()
{
    dispstat.hblank = false;
    dispstat.vblank = false;

    if (dispcnt.isActive())
    {
        video_ctx.renderCopyTexture();
        video_ctx.renderPresent();
    }
}
