#include "ppu.h"

#include "common/utility.h"
#include "mmu/interrupt.h"
#include "scanlinebuilder.h"
#include "enums.h"

PPU::PPU(MMU& mmu)
    : mmu(mmu)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "eggvance",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        2 * WIDTH, 2 * HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    
    renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED
    );
    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_BGR555,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT
    );
}

PPU::~PPU()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void PPU::reset()
{

}

void PPU::scanline()
{
    mmu.dispstat.vblank = false;
    mmu.dispstat.hblank = false;
    mmu.mmio<u16>(REG_DISPSTAT) &= ~0x3;

    bgs[0].flip();
    bgs[1].flip();
    bgs[2].flip();
    bgs[3].flip();

    obj.fill(ObjData());

    switch (mmu.dispcnt.mode)
    {
    case 0: 
        renderBg(&PPU::renderBgMode0, 0);
        renderBg(&PPU::renderBgMode0, 1);
        renderBg(&PPU::renderBgMode0, 2);
        renderBg(&PPU::renderBgMode0, 3);
        break;

    case 1: 
        renderBg(&PPU::renderBgMode0, 0);
        renderBg(&PPU::renderBgMode0, 1);
        renderBg(&PPU::renderBgMode2, 2);
        break;

    case 2: 
        renderBg(&PPU::renderBgMode2, 2);
        renderBg(&PPU::renderBgMode2, 3);
        break;

    case 3: 
        renderBg(&PPU::renderBgMode3, 2);
        break;

    case 4: 
        renderBg(&PPU::renderBgMode4, 2);
        break;

    case 5: 
        renderBg(&PPU::renderBgMode5, 2);
        break;
    }
    renderObjects();

    generate();
}

void PPU::hblank()
{
    mmu.dispstat.vblank = false;
    mmu.dispstat.hblank = true;
    mmu.mmio<u16>(REG_DISPSTAT) &= ~0x3;
    mmu.mmio<u16>(REG_DISPSTAT) |= 0x2;

    mmu.bgx[0].internal += mmu.bgpb[0].value;
    mmu.bgx[1].internal += mmu.bgpb[1].value;
    mmu.bgy[0].internal += mmu.bgpd[0].value;
    mmu.bgy[1].internal += mmu.bgpd[1].value;

    if (mmu.dispstat.hblank_irq)
    {
        Interrupt::request(IF_HBLANK);
    }
}

void PPU::vblank()
{
    mmu.dispstat.vblank = true;
    mmu.dispstat.hblank = false;
    mmu.mmio<u16>(REG_DISPSTAT) &= ~0x3;
    mmu.mmio<u16>(REG_DISPSTAT) |= 0x1;

    mmu.bgx[0].internal = mmu.bgx[0].value;
    mmu.bgx[1].internal = mmu.bgx[1].value;
    mmu.bgy[0].internal = mmu.bgy[0].value;
    mmu.bgy[1].internal = mmu.bgy[1].value;

    if (mmu.dispstat.vblank_irq)
    {
        Interrupt::request(IF_VBLANK);
    }
}

void PPU::next()
{
    int vcount_match = mmu.vcount == mmu.dispstat.vcount_compare;

    mmu.vcount = (mmu.vcount + 1) % 228;
    mmu.dispstat.vcount_match = vcount_match;

    mmu.mmio<u16>(REG_DISPSTAT) &= ~0x4;
    mmu.mmio<u16>(REG_DISPSTAT) |= vcount_match << 2;

    if (vcount_match && mmu.dispstat.vcount_irq)
    {
        Interrupt::request(IF_VCOUNT_MATCH);
    }
}

void PPU::present()
{
    if (mmu.mmio<u16>(REG_DISPCNT) & 0x1F00)
    {
        SDL_UpdateTexture(
            texture, 0,
            screen.data(),
            2 * WIDTH
        );
        SDL_RenderCopy(renderer, texture, 0, 0);
        SDL_RenderPresent(renderer);
    }
}

void PPU::renderBg(RenderFunc func, int bg)
{
    if (!mmu.dispcnt.bg[bg])
        return;

    if (mmu.bgcnt[bg].mosaic)
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
    int mosaic_x = mmu.mosaic.bg_x + 1;
    if (mosaic_x == 1)
        return;

    int color;
    for (int x = 0; x < WIDTH; ++x)
    {
        if (x % mosaic_x == 0)
            color = bgs[bg][x];

        bgs[bg][x] = color;
    }
}

bool PPU::mosaicDominant() const
{
    return mmu.vcount % (mmu.mosaic.bg_y + 1) == 0;
}

void PPU::generate()
{
    ScanlineBuilder builder(bgs, obj, mmu);

    u16* scanline = &screen[WIDTH * mmu.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        builder.build(x);

        int color = builder.begin()->color;

        if (builder.windowSfx() && (mmu.bldcnt.mode != BLD_DISABLED || obj[x].mode == GFX_ALPHA))
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
                switch (mmu.bldcnt.mode)
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

    int eva = std::min(17, mmu.bldalpha.eva);
    int evb = std::min(17, mmu.bldalpha.evb);

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

    int evy = std::min(17, mmu.bldy.evy);

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

    int evy = std::min(17, mmu.bldy.evy);

    int t_r = std::min(31, a_r - ((a_r * evy) >> 4));
    int t_g = std::min(31, a_g - ((a_g * evy) >> 4));
    int t_b = std::min(31, a_b - ((a_b * evy) >> 4));

    return (t_r << 0) | (t_g << 5) | (t_b << 10);
}

int PPU::readBgColor(int index, int palette)
{
    if (index == 0)
        return TRANSPARENT;

    return *reinterpret_cast<u16*>(&mmu.palette[0x20 * palette + 2 * index]);
}

int PPU::readFgColor(int index, int palette)
{
    if (index == 0)
        return TRANSPARENT;

    return *reinterpret_cast<u16*>(&mmu.palette[0x200 + 0x20 * palette + 2 * index]);
}

int PPU::readPixel(u32 addr, int x, int y, PixelFormat format)
{
    if (format == BPP4)
    {
        int byte = mmu.readByteFast(
            addr + 4 * y + x / 2
        );
        return (x & 0x1) ? (byte >> 4) : (byte & 0xF);
    }
    else
    {
        return mmu.readByteFast(
            addr + 8 * y + x
        );
    }
}
