#include "ppu.h"

#include <algorithm>
#include <numeric>

#include "constants.h"
#include "arm/arm.h"
#include "base/bit.h"
#include "base/config.h"
#include "dma/dma.h"
#include "frontend/videocontext.h"

void Ppu::init()
{
    events.hblank = [this](u64 late)
    {
        hblank(late);
    };

    events.hblank_end = [this](u64 late)
    {
        hblankEnd(late);
    };

    scheduler.insert(events.hblank, 1006);
}

void Ppu::scanline()
{
    if (dispcnt.blank)
    {
        auto& scanline = video_ctx.scanline(vcount);
        std::fill(scanline.begin(), scanline.end(), 0xFFFF'FFFF);
        return;
    }

    for (auto& background : backgrounds)
        background.buffer.flip();

    if (objects_exist)
    {
        objects.fill(ObjectLayer());
        objects_exist = false;
        objects_alpha = false;
    }

    if (dispcnt.layers & Layer::Flag::Obj)
    {
        renderObjects();
    }

    switch (dispcnt.mode)
    {
    case 0:
        renderBg(&Ppu::renderBgMode0, backgrounds[0]);
        renderBg(&Ppu::renderBgMode0, backgrounds[1]);
        renderBg(&Ppu::renderBgMode0, backgrounds[2]);
        renderBg(&Ppu::renderBgMode0, backgrounds[3]);
        collapse(uint(Layer::Flag::Bg0 | Layer::Flag::Bg1 | Layer::Flag::Bg2 | Layer::Flag::Bg3));
        break;

    case 1:
        renderBg(&Ppu::renderBgMode0, backgrounds[0]);
        renderBg(&Ppu::renderBgMode0, backgrounds[1]);
        renderBg(&Ppu::renderBgMode2, backgrounds[2]);
        collapse(uint(Layer::Flag::Bg0 | Layer::Flag::Bg1 | Layer::Flag::Bg2));
        break;

    case 2:
        renderBg(&Ppu::renderBgMode2, backgrounds[2]);
        renderBg(&Ppu::renderBgMode2, backgrounds[3]);
        collapse(uint(Layer::Flag::Bg2 | Layer::Flag::Bg3));
        break;

    case 3:
        renderBg(&Ppu::renderBgMode3, backgrounds[2]);
        collapse(uint(Layer::Flag::Bg2));
        break;

    case 4:
        renderBg(&Ppu::renderBgMode4, backgrounds[2]);
        collapse(uint(Layer::Flag::Bg2));
        break;

    case 5:
        renderBg(&Ppu::renderBgMode5, backgrounds[2]);
        collapse(uint(Layer::Flag::Bg2));
        break;
    }
}

void Ppu::hblank(u64 late)
{
    dispstat.hblank = true;

    if (dispstat.hblank_irq)
    {
        arm.raise(Irq::HBlank, late);
    }

    if (vcount < 160)
    {
        scanline();

        backgrounds[2].matrix.hblank();
        backgrounds[3].matrix.hblank();

        dma.broadcast(Dma::Event::HBlank);
    }

    if (vcount > 1 && vcount < 162)
    {
        dma.broadcast(Dma::Event::Hdma);
    }

    scheduler.insert(events.hblank_end, 226 - late);
}

void Ppu::hblankEnd(u64 late)
{
    ++vcount;

    dispstat.hblank = false;
    dispstat.vblank = vcount >= 160 && vcount < 227;

    dispstat.vmatch = vcount == dispstat.vcompare;
    if (dispstat.vmatch && dispstat.vmatch_irq)
    {
        arm.raise(Irq::VMatch, late);
    }

    if (vcount == 160)
    {
        backgrounds[2].matrix.vblank();
        backgrounds[3].matrix.vblank();

        if (dispstat.vblank_irq)
        {
            arm.raise(Irq::VBlank, late);
        }
        dma.broadcast(Dma::Event::VBlank);
    }

    scheduler.insert(events.hblank, 1006 - late);
}

void Ppu::present()
{
    if (dispcnt.isActive())
    {
        video_ctx.renderCopyTexture();
        video_ctx.renderPresent();
    }
}
