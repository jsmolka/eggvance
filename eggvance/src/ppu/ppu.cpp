#include "ppu.h"

#include "mmu/interrupt.h"
#include "scanlinebuilder.h"

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
    for (DoubleBuffer<u16>& bg : bgs)
    {
        bg.fill(COLOR_T);
        bg.flip();
        bg.fill(COLOR_T);
    }
    obj = {};
    obj_exist = false;
    obj_alpha = false;
}

void PPU::scanline()
{
    mmio.dispstat.vblank = false;
    mmio.dispstat.hblank = false;

    if (mmio.dispcnt.force_blank)
    {
        u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
        std::fill_n(scanline, WIDTH, 0x7FFF);
        return;
    }

    bgs[0].flip();
    bgs[1].flip();
    bgs[2].flip();
    bgs[3].flip();

    if (mmio.dispcnt.obj)
    {
        if (obj_exist)
        {
            obj.fill(ObjectData());
            obj_exist = false;
            obj_alpha = false;
        }
        renderObjects();
    }

    switch (mmio.dispcnt.mode)
    {
    case 0: 
        renderBg(&PPU::renderBgMode0, 0);
        renderBg(&PPU::renderBgMode0, 1);
        renderBg(&PPU::renderBgMode0, 2);
        renderBg(&PPU::renderBgMode0, 3);
        finalize(0, 4);
        break;

    case 1: 
        renderBg(&PPU::renderBgMode0, 0);
        renderBg(&PPU::renderBgMode0, 1);
        renderBg(&PPU::renderBgMode2, 2);
        finalize(0, 3);
        break;

    case 2: 
        renderBg(&PPU::renderBgMode2, 2);
        renderBg(&PPU::renderBgMode2, 3);
        finalize(2, 4);
        break;

    case 3: 
        renderBg(&PPU::renderBgMode3, 2);
        finalize(2, 3);
        break;

    case 4: 
        renderBg(&PPU::renderBgMode4, 2);
        finalize(2, 3);
        break;

    case 5: 
        renderBg(&PPU::renderBgMode5, 2);
        finalize(2, 3);
        break;
    }
}

void PPU::hblank()
{
    mmu.signalDMA(DMA::Timing::HBLANK);
    mmio.dispstat.vblank = false;
    mmio.dispstat.hblank = true;

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
    mmu.signalDMA(DMA::Timing::VBLANK);
    mmio.dispstat.vblank = true;
    mmio.dispstat.hblank = false;

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
    int vmatch = mmio.vcount == mmio.dispstat.vcount_compare;

    mmio.vcount = (mmio.vcount + 1) % 228;
    mmio.dispstat.vmatch = vmatch;

    if (vmatch && mmio.dispstat.vmatch_irq)
    {
        Interrupt::request(IF_VMATCH);
    }
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
            bgs[bg].flip();
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

    int color;
    for (int x = 0; x < WIDTH; ++x)
    {
        if (x % mosaic_x == 0)
        {
            color = bgs[bg][x];
        }
        bgs[bg][x] = color;
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

/*
 * Todo: The whole generation process in its current state is really slow
 * compared to what it could be (~6% CPU usage maximum) but I don't feel like
 * improving it right now because it seems to be pretty accurate at least.
 */
void PPU::finalizeOld()
{
    ScanlineBuilder builder(bgs, obj, mmu);

    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        builder.build(x);

        int color = builder.begin()->color;

        if ((builder.windowSfx() && mmio.bldcnt.mode != BLD_DISABLED) || obj[x].mode == GFX_ALPHA)
        {
            int a = 0;
            int b = 0;

            bool blended = false;

            if (obj[x].mode == GFX_ALPHA)
            {
                if (blended = builder.getBlendLayers(a, b))
                    color = blendAlpha(a, b);
            }

            if (!blended)
            {
                switch (mmio.bldcnt.mode)
                {
                case BLD_ALPHA:
                    if (builder.getBlendLayers(a, b))
                        color = blendAlpha(a, b);
                    break;

                case BLD_WHITE:
                    if (builder.getBlendLayers(a))
                        color = blendWhite(a);
                    break;

                case BLD_BLACK:
                    if (builder.getBlendLayers(a))
                        color = blendBlack(a);
                    break;
                }
            }
        }
        scanline[x] = color;
    }
}

int PPU::blendAlpha(int a, int b) const
{
    int a_r = (a >>  0) & 0x1F;
    int a_g = (a >>  5) & 0x1F;
    int a_b = (a >> 10) & 0x1F;
    int b_r = (b >>  0) & 0x1F;
    int b_g = (b >>  5) & 0x1F;
    int b_b = (b >> 10) & 0x1F;

    int eva = std::min(17, mmio.bldalpha.eva);
    int evb = std::min(17, mmio.bldalpha.evb);

    int t_r = std::min(31, (a_r * eva + b_r * evb) >> 4);
    int t_g = std::min(31, (a_g * eva + b_g * evb) >> 4);
    int t_b = std::min(31, (a_b * eva + b_b * evb) >> 4);

    return (t_r << 0) | (t_g << 5) | (t_b << 10);
}

int PPU::blendWhite(int a) const
{
    int a_r = (a >>  0) & 0x1F;
    int a_g = (a >>  5) & 0x1F;
    int a_b = (a >> 10) & 0x1F;

    int evy = std::min(17, mmio.bldy.evy);

    int t_r = std::min(31, a_r + (((31 - a_r) * evy) >> 4));
    int t_g = std::min(31, a_g + (((31 - a_g) * evy) >> 4));
    int t_b = std::min(31, a_b + (((31 - a_b) * evy) >> 4));

    return (t_r << 0) | (t_g << 5) | (t_b << 10);
}

int PPU::blendBlack(int a) const
{
    int a_r = (a >>  0) & 0x1F;
    int a_g = (a >>  5) & 0x1F;
    int a_b = (a >> 10) & 0x1F;

    int evy = std::min(17, mmio.bldy.evy);

    int t_r = std::min(31, a_r - ((a_r * evy) >> 4));
    int t_g = std::min(31, a_g - ((a_g * evy) >> 4));
    int t_b = std::min(31, a_b - ((a_b * evy) >> 4));

    return (t_r << 0) | (t_g << 5) | (t_b << 10);
}

int PPU::readBgColor(int index, int palette)
{
    if (index == 0)
        return COLOR_T;

    return mmu.palette.readHalf(0x20 * palette + 2 * index);
}

int PPU::readFgColor(int index, int palette)
{
    if (index == 0)
        return COLOR_T;

    return mmu.palette.readHalf(0x200 + 0x20 * palette + 2 * index);
}

int PPU::readPixel(u32 addr, int x, int y, PixelFormat format)
{
    if (format == BPP4)
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
