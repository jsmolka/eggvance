#include "ppu.h"

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

void Ppu::hblank(u64 late)
{
    dispstat.hblank = true;

    if (dispstat.hblank_irq)
    {
        arm.raise(Irq::HBlank, late);
    }

    if (vcount < 160)
    {
        render();

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
        video_ctx.renderFrame();

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
