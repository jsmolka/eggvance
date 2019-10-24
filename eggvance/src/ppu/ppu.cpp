#include "ppu.h"

#include <algorithm>

#include "common/constants.h"
#include "arm/arm.h"
#include "mmu/mmu.h"

PPU ppu;

void PPU::reset()
{
    io.reset();

    for (auto& background : backgrounds)
    {
        background.fill(Palette::transparent);
        background.flip();
        background.fill(Palette::transparent);
    }
    objects.fill(ObjectLayer());
    objects_exist = false;
    objects_alpha = false;
}

void PPU::scanline()
{
    io.dispstat.vblank = false;
    io.dispstat.hblank = false;

    if (io.dispcnt.force_blank)
    {
        u32* scanline = &backend.buffer[240 * io.vcount];
        std::fill_n(scanline, 240, 0xFFFFFFFF);
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
    if (io.dispcnt.obj)
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

    io.bgx[0].reg += io.bgpb[0].param;
    io.bgx[1].reg += io.bgpb[1].param;
    io.bgy[0].reg += io.bgpd[0].param;
    io.bgy[1].reg += io.bgpd[1].param;

    if (io.dispstat.hblank_irq)
    {
        arm.request(Interrupt::HBlank);
    }
    arm.dma.signal(DMA::Timing::HBlank);
}

void PPU::vblank()
{
    io.dispstat.vblank = true;
    io.dispstat.hblank = false;

    io.bgx[0].reg = io.bgx[0].ref;
    io.bgx[1].reg = io.bgx[1].ref;
    io.bgy[0].reg = io.bgy[0].ref;
    io.bgy[1].reg = io.bgy[1].ref;

    if (io.dispstat.vblank_irq)
    {
        arm.request(Interrupt::VBlank);
    }
    arm.dma.signal(DMA::Timing::VBlank);
}

void PPU::next()
{
    io.dispstat.vmatch = io.vcount == io.dispstat.vcompare;
    if (io.dispstat.vmatch && io.dispstat.vmatch_irq)
    {
        arm.request(Interrupt::VMatch);
    }
    io.vcount = (io.vcount + 1) % 228;
}

void PPU::present()
{
    if (io.dispcnt.has_content)
        backend.present();
}

void PPU::renderBg(RenderFunc func, int bg)
{
    if (!io.dispcnt.bg[bg])
        return;

    if (mosaicAffected(bg))
    {
        if (mosaicDominant())
        {
            (this->*func)(bg);
            mosaic(bg);
        }
        else
        {
            backgrounds[bg].flip();
        }
    }
    else
    {
        (this->*func)(bg);
    }
}

void PPU::mosaic(int bg)
{
    int mosaic_x = io.mosaic.bgs.x;
    if (mosaic_x == 1)
        return;

    u16 color;
    for (int x = 0; x < 240; ++x)
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

void PPU::collapse(int begin, int end)
{
    std::vector<BackgroundLayer> layers;
    layers.reserve(end - begin);

    for (int bg = begin; bg < end; ++bg) 
    {
        if (io.dispcnt.bg[bg])
        {
            layers.emplace_back(
                bg, 
                backgrounds[bg].data(),
                io.bgcnt[bg].priority
            );
        }
    }

    std::sort(layers.begin(), layers.end(),
        [](const BackgroundLayer& lhs, const BackgroundLayer& rhs) {
            return lhs.priority != rhs.priority
                ? lhs.priority < rhs.priority
                : lhs.id < rhs.id;
        }
    );

    if (objects_exist)
        collapse<1>(layers);
    else
        collapse<0>(layers);
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

