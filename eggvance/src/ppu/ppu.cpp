#include "ppu.h"

#include <algorithm>
#include <numeric>

#include "constants.h"
#include "arm/arm.h"
#include "arm/constants.h"
#include "base/bit.h"
#include "base/config.h"
#include "core/videocontext.h"
#include "dma/dma.h"

Ppu::Ppu()
{
    bgpa[0].write(1, 0x01);
    bgpa[1].write(1, 0x01);
    bgpd[0].write(1, 0x01);
    bgpd[1].write(1, 0x01);

    backgrounds[0].fill(kTransparent);
    backgrounds[1].fill(kTransparent);
    backgrounds[2].fill(kTransparent);
    backgrounds[3].fill(kTransparent);

    for (u32 color = 0; color < argb.size(); ++color)
    {
        uint r = bit::seq< 0, 5>(color) << 3 | 0x7;
        uint g = bit::seq< 5, 5>(color) << 3 | 0x7;
        uint b = bit::seq<10, 5>(color) << 3 | 0x7;

        if (config.lcd_color)
        {
            constexpr double kDarken   = 0.5;
            constexpr double kGammaOut = 2.2;
            constexpr double kGammaLcd = 2.5;

            double rs = std::pow(r / 255.0, kGammaOut + kDarken);
            double gs = std::pow(g / 255.0, kGammaOut + kDarken);
            double bs = std::pow(b / 255.0, kGammaOut + kDarken);

            double rd = std::pow(0.84 * rs + 0.18 * gs + 0.00 * bs, 1 / kGammaLcd + kDarken * 0.125);
            double gd = std::pow(0.09 * rs + 0.67 * gs + 0.26 * bs, 1 / kGammaLcd + kDarken * 0.125);
            double bd = std::pow(0.15 * rs + 0.10 * gs + 0.75 * bs, 1 / kGammaLcd + kDarken * 0.125);

            r = static_cast<uint>(std::min(1.0, rd) * 255);
            g = static_cast<uint>(std::min(1.0, gd) * 255);
            b = static_cast<uint>(std::min(1.0, bd) * 255);
        }

        argb[color] = 0xFF00'0000 | (r << 16) | (g << 8) | b;
    }
}

void Ppu::init()
{
    events.hblank = [this](u64 late)
    {
        if (vcount < 160)
            scanline();

        ppu.hblank();

        scheduler.queueIn(events.hblank_end, 226 - late);
    };

    events.hblank_end = [this](u64 late)
    {
        next();

        if (vcount == 160)
            vblank();

        scheduler.queueIn(events.hblank, 1006 - late);
    };

    scheduler.queueIn(events.hblank, 1006);
}

void Ppu::scanline()
{
    if (dispcnt.blank)
    {
        u32* scanline = video_ctx.scanline(vcount);
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
        renderBg(&Ppu::renderBgMode0, 0);
        renderBg(&Ppu::renderBgMode0, 1);
        renderBg(&Ppu::renderBgMode0, 2);
        renderBg(&Ppu::renderBgMode0, 3);
        collapse(kLayerBg0 | kLayerBg1 | kLayerBg2 | kLayerBg3);
        break;

    case 1:
        renderBg(&Ppu::renderBgMode0, 0);
        renderBg(&Ppu::renderBgMode0, 1);
        renderBg(&Ppu::renderBgMode2, 2);
        collapse(kLayerBg0 | kLayerBg1 | kLayerBg2);
        break;

    case 2:
        renderBg(&Ppu::renderBgMode2, 2);
        renderBg(&Ppu::renderBgMode2, 3);
        collapse(kLayerBg2 | kLayerBg3);
        break;

    case 3:
        renderBg(&Ppu::renderBgMode3, 2);
        collapse(kLayerBg2);
        break;

    case 4:
        renderBg(&Ppu::renderBgMode4, 2);
        collapse(kLayerBg2);
        break;

    case 5:
        renderBg(&Ppu::renderBgMode5, 2);
        collapse(kLayerBg2);
        break;
    }
}

void Ppu::hblank()
{
    dispstat.hblank = true;

    if (dispstat.hblank_irq)
    {
        arm.raise(kIrqHBlank);
    }

    if (vcount < 160)
    {
        bgx[0].hblank(bgpb[0]);
        bgx[1].hblank(bgpb[1]);
        bgy[0].hblank(bgpd[0]);
        bgy[1].hblank(bgpd[1]);

        dma.broadcast(Dma::Timing::HBlank);
    }

    if (vcount > 1 && vcount < 162)
    {
        dma.broadcast(Dma::Timing::Video);
    }
}

void Ppu::vblank()
{
    dispstat.vblank = true;
    
    bgx[0].vblank();
    bgx[1].vblank();
    bgy[0].vblank();
    bgy[1].vblank();

    if (dispstat.vblank_irq)
    {
        arm.raise(kIrqVBlank);
    }
    dma.broadcast(Dma::Timing::VBlank);
}

void Ppu::next()
{
    dispstat.hblank = false;

    ++vcount;

    dispstat.vmatch = vcount == dispstat.vcompare;
    if (dispstat.vmatch && dispstat.vmatch_irq)
    {
        arm.raise(kIrqVMatch);
    }
}

void Ppu::present()
{
    dispstat.vblank = false;

    if (dispcnt.isActive())
    {
        video_ctx.renderCopyTexture();
        video_ctx.renderPresent();
    }
}
