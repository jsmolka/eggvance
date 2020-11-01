#include "gpu.h"

#include <algorithm>
#include <numeric>

#include "constants.h"
#include "arm/arm.h"
#include "arm/constants.h"
#include "base/bit.h"
#include "base/config.h"
#include "base/macros.h"
#include "core/videocontext.h"
#include "dma/dma.h"

Gpu::Gpu()
{
    backgrounds[0].fill(kTransparent);
    backgrounds[1].fill(kTransparent);
    backgrounds[2].fill(kTransparent);
    backgrounds[3].fill(kTransparent);

    bgpa[0].value = 1 << kDecimalBits;
    bgpa[1].value = 1 << kDecimalBits;
    bgpd[0].value = 1 << kDecimalBits;
    bgpd[1].value = 1 << kDecimalBits;

    for (u32 color = 0; color < argb.size(); ++color)
    {
        uint r = bit::seq< 0, 5>(color) << 3;
        uint g = bit::seq< 5, 5>(color) << 3;
        uint b = bit::seq<10, 5>(color) << 3;

        if (config.lcd_color)
        {
            constexpr double kDarken   = 0.5;
            constexpr double kGammaOut = 2.2;
            constexpr double kGammaLcd = 2.5;

            double rs = pow(r / 255.0, kGammaOut + kDarken);
            double gs = pow(g / 255.0, kGammaOut + kDarken);
            double bs = pow(b / 255.0, kGammaOut + kDarken);

            double rd = std::pow(0.84 * rs + 0.18 * gs + 0.00 * bs, 1 / kGammaLcd + kDarken * 0.125);
            double gd = std::pow(0.09 * rs + 0.67 * gs + 0.26 * bs, 1 / kGammaLcd + kDarken * 0.125);
            double bd = std::pow(0.15 * rs + 0.10 * gs + 0.73 * bs, 1 / kGammaLcd + kDarken * 0.125);

            r = static_cast<uint>(std::min(1.0, rd) * 255);
            g = static_cast<uint>(std::min(1.0, gd) * 255);
            b = static_cast<uint>(std::min(1.0, bd) * 255);
        }

        argb[color] = 0xFF00'0000 | (r << 16) | (g << 8) | b;
    }
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
        collapse(kLayerBg0 | kLayerBg1 | kLayerBg2 | kLayerBg3);
        break;

    case 1:
        renderBg(&Gpu::renderBgMode0, 0);
        renderBg(&Gpu::renderBgMode0, 1);
        renderBg(&Gpu::renderBgMode2, 2);
        collapse(kLayerBg0 | kLayerBg1 | kLayerBg2);
        break;

    case 2:
        renderBg(&Gpu::renderBgMode2, 2);
        renderBg(&Gpu::renderBgMode2, 3);
        collapse(kLayerBg2 | kLayerBg3);
        break;

    case 3:
        renderBg(&Gpu::renderBgMode3, 2);
        collapse(kLayerBg2);
        break;

    case 4:
        renderBg(&Gpu::renderBgMode4, 2);
        collapse(kLayerBg2);
        break;

    case 5:
        renderBg(&Gpu::renderBgMode5, 2);
        collapse(kLayerBg2);
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
