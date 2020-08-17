#include "ppu.h"

#include <algorithm>

#include "base/bits.h"
#include "base/constants.h"
#include "base/macros.h"
#include "core/core.h"

PPU::PPU(Core& core)
    : core(core)
{

}

void PPU::reset()
{
    io = PpuIo();

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
        u32* scanline = &core.context.video.buffer[kScreenW * io.vcount.value];
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

    io.bgx[0].hblank(io.bgpb[0].value);
    io.bgx[1].hblank(io.bgpb[1].value);
    io.bgy[0].hblank(io.bgpd[0].value);
    io.bgy[1].hblank(io.bgpd[1].value);

    if (io.dispstat.hblank_irq)
    {
        core.irqh.request(kIrqHBlank);
    }
    core.dmac.broadcast(Dma::kTimingHBlank);
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
        core.irqh.request(kIrqVBlank);
    }
    core.dmac.broadcast(Dma::kTimingVBlank);
}

void PPU::next()
{
    io.dispstat.vmatch = io.vcount.value == io.dispstat.vcompare;
    if (io.dispstat.vmatch && io.dispstat.vmatch_irq)
    {
        core.irqh.request(kIrqVMatch);
    }
    io.vcount.next();
}

void PPU::present()
{
    if (io.dispcnt.isActive())
    {
        core.context.video.renderCopyBuffer();
        core.context.video.renderPresent();
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
    return io.vcount.value % io.mosaic.bgs.y == 0;
}

u32 PPU::argb(u16 color)
{
    return 0xFF000000
        | (color & 0x001F) << 19
        | (color & 0x03E0) <<  6
        | (color & 0x7C00) >>  7;
}

