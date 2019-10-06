#include "ppu.h"

#include "mmu/interrupt.h"

PPU::PPU(MMU& mmu)
    : mmu(mmu)
    , mmio(mmu.mmio)
{
    reset();

    for (int x = 0; x < 32; ++x)
    {
        pas[x] = mmu.oam.ptr<s16>(0x20 * x + 0x06);
        pbs[x] = mmu.oam.ptr<s16>(0x20 * x + 0x0E);
        pcs[x] = mmu.oam.ptr<s16>(0x20 * x + 0x16);
        pds[x] = mmu.oam.ptr<s16>(0x20 * x + 0x1E);
    }
}

void PPU::reset()
{
    for (auto& background : backgrounds)
    {
        background.fill(TRANSPARENT);
        background.flip();
        background.fill(TRANSPARENT);
    }
    objects.fill(ObjectLayer());
    objects_exist = false;
    objects_alpha = false;
}

void PPU::scanline()
{
    mmio.dispstat.vblank = false;
    mmio.dispstat.hblank = false;

    if (mmio.dispcnt.force_blank)
    {
        u32* scanline = &backend.buffer[WIDTH * mmio.vcount];
        std::fill_n(scanline, WIDTH, 0xFFFFFFFF);
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
    if (mmio.dispcnt.obj)
    {
        renderObjects();
    }

    switch (mmio.dispcnt.mode)
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
    mmio.dispstat.hblank = true;
    mmio.dispstat.vblank = false;
    mmu.signalDMA(DMA::Timing::HBLANK);

    mmio.bgx[0].internal += mmio.bgpb[0].parameter;
    mmio.bgx[1].internal += mmio.bgpb[1].parameter;
    mmio.bgy[0].internal += mmio.bgpd[0].parameter;
    mmio.bgy[1].internal += mmio.bgpd[1].parameter;

    if (mmio.dispstat.hblank_irq)
    {
        Interrupt::request(IF_HBLANK);
    }
}

void PPU::vblank()
{
    mmio.dispstat.vblank = true;
    mmio.dispstat.hblank = false;
    mmu.signalDMA(DMA::Timing::VBLANK);

    mmio.bgx[0].internal = mmio.bgx[0].reference;
    mmio.bgx[1].internal = mmio.bgx[1].reference;
    mmio.bgy[0].internal = mmio.bgy[0].reference;
    mmio.bgy[1].internal = mmio.bgy[1].reference;

    if (mmio.dispstat.vblank_irq)
    {
        Interrupt::request(IF_VBLANK);
    }
}

void PPU::next()
{
    mmio.dispstat.vmatch = mmio.vcount == mmio.dispstat.vcount_compare;
    if (mmio.dispstat.vmatch && mmio.dispstat.vmatch_irq)
    {
        Interrupt::request(IF_VMATCH);
    }
    mmio.vcount = (mmio.vcount + 1) % 228;
}

void PPU::present()
{
    if (mmio.dispcnt.enabled())
        backend.present();
}

void PPU::renderBg(RenderFunc func, int bg)
{
    if (!mmio.dispcnt.bg[bg])
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
    int mosaic_x = mmio.mosaic.bg.x + 1;
    if (mosaic_x == 1)
        return;

    u16 color;
    for (int x = 0; x < WIDTH; ++x)
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
    return mmio.bgcnt[bg].mosaic && (mmio.mosaic.bg.x > 0 || mmio.mosaic.bg.y > 0);
}

bool PPU::mosaicDominant() const
{
    return mmio.vcount % (mmio.mosaic.bg.y + 1) == 0;
}

void PPU::collapse(int begin, int end)
{
    std::vector<BackgroundLayer> layers;
    layers.reserve(end - begin);

    for (int bg = begin; bg < end; ++bg) 
    {
        if (mmio.dispcnt.bg[bg])
        {
            layers.emplace_back(
                bg, 
                backgrounds[bg].data(),
                mmio.bgcnt[bg].priority
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

    int tr = std::min(31, (ar * mmio.bldalpha.eva + br * mmio.bldalpha.evb) >> 4);
    int tg = std::min(31, (ag * mmio.bldalpha.eva + bg * mmio.bldalpha.evb) >> 4);
    int tb = std::min(31, (ab * mmio.bldalpha.eva + bb * mmio.bldalpha.evb) >> 4);

    return (tr << 0) | (tg << 5) | (tb << 10);
}

u16 PPU::blendWhite(u16 a) const
{
    int ar = bits< 0, 5>(a);
    int ag = bits< 5, 5>(a);
    int ab = bits<10, 5>(a);

    int tr = std::min(31, ar + (((31 - ar) * mmio.bldy.evy) >> 4));
    int tg = std::min(31, ag + (((31 - ag) * mmio.bldy.evy) >> 4));
    int tb = std::min(31, ab + (((31 - ab) * mmio.bldy.evy) >> 4));

    return (tr << 0) | (tg << 5) | (tb << 10);
}

u16 PPU::blendBlack(u16 a) const
{
    int ar = bits< 0, 5>(a);
    int ag = bits< 5, 5>(a);
    int ab = bits<10, 5>(a);

    int tr = std::min(31, ar - ((ar * mmio.bldy.evy) >> 4));
    int tg = std::min(31, ag - ((ag * mmio.bldy.evy) >> 4));
    int tb = std::min(31, ab - ((ab * mmio.bldy.evy) >> 4));

    return (tr << 0) | (tg << 5) | (tb << 10);
}

u32 PPU::argb(u16 color)
{
    return 0xFF000000
        | (color & 0x001F) << 19
        | (color & 0x03E0) <<  6
        | (color & 0x7C00) >>  7;
}

u16 PPU::readBgColor(int index, int bank)
{
    if (index == 0)
        return TRANSPARENT;

    return mmu.palette.readHalf(0x20 * bank + 2 * index);
}

u16 PPU::readFgColor(int index, int bank)
{
    if (index == 0)
        return TRANSPARENT;

    return mmu.palette.readHalf(0x200 + 0x20 * bank + 2 * index);
}

u16 PPU::readPixel(u32 addr, int x, int y, Palette palette)
{
    if (palette == P_16_16)
    {
        return (x & 0x1) 
            ? mmu.vram[addr + 4 * y + x / 2] >> 4
            : mmu.vram[addr + 4 * y + x / 2] & 0xF;
    }
    else
    {
        return mmu.vram[addr + 8 * y + x];
    }
}
