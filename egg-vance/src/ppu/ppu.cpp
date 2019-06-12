#include "ppu.h"

#include "mmu/map.h"

PPU::PPU(MMU& mmu)
    : mmu(mmu)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "egg-vance",
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
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
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
    mmu.dispstat.hblank = false;
    mmu.dispstat.vblank = false;

    for (BgBuffer& buffer : buffer_bg)
        buffer.flip();

    if (mmu.dispcnt.sprites)
    {
        for (auto& buffer : buffer_sprites)
            buffer.fill(COLOR_TRANSPARENT);
    }

    switch (mmu.dispcnt.bg_mode)
    {
    case 0: renderMode0(); break;
    case 1: renderMode1(); break;
    case 2: renderMode2(); break;
    case 3: renderMode3(); break;
    case 4: renderMode4(); break;
    case 5: renderMode5(); break;
    }
    renderSprites();

    applyEffects();

    generateScanline();

    mmu.bg2x.internal += mmu.bg2pb.value();
    mmu.bg3x.internal += mmu.bg3pb.value();
    mmu.bg2y.internal += mmu.bg2pd.value();
    mmu.bg3y.internal += mmu.bg3pd.value();
}

void PPU::hblank()
{
    mmu.dispstat.hblank = true;
    mmu.dispstat.vblank = false;

    if (mmu.dispstat.hblank_irq)
    {
        // Todo: request interrupt
    }
}

void PPU::vblank()
{
    mmu.dispstat.hblank = false;
    mmu.dispstat.vblank = true;

    mmu.bg2x.internal = mmu.bg2x.value();
    mmu.bg3x.internal = mmu.bg3x.value();
    mmu.bg2y.internal = mmu.bg2y.value();
    mmu.bg3y.internal = mmu.bg3y.value();

    if (mmu.dispstat.vblank_irq)
    {
        // Todo: request interrupt
    }
}

void PPU::next()
{
    bool vcount_match = mmu.vcount == mmu.dispstat.vcount_compare;

    mmu.vcount = (mmu.vcount + 1) % 228;
    mmu.dispstat.vcount_match = vcount_match;

    if (vcount_match && mmu.dispstat.vcount_irq)
    {
        // Todo: request interrupt
    }
}

void PPU::render()
{
    SDL_UpdateTexture(
        texture,
        0,
        buffer_screen.data(),
        2 * WIDTH
    );
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);
}

void PPU::generateScanline()
{
    int backdrop = mmu.readHalfFast(MAP_PALETTE);
    u16* scanline = &buffer_screen[mmu.vcount * WIDTH];
    for (int x = 0; x < WIDTH; ++x)
    {
        int pixel = backdrop;
        for (int priority = 3; priority > -1; --priority)
        {
            for (int bg = 3; bg > -1; --bg)
            {
                if (mmu.dispcnt.bg(bg) && mmu.bgcnt[bg].priority == priority)
                {
                    int value = buffer_bg[bg][x];
                    if (value != COLOR_TRANSPARENT)
                        pixel = value;
                }
            }
            if (mmu.dispcnt.sprites)
            {
                int value = buffer_sprites[priority][x];
                if (value != COLOR_TRANSPARENT)
                    pixel = value;
            }
        }
        scanline[x] = pixel;
    }
}

int PPU::readBgColor(int index, int palette)
{
    if (index == 0)
        return COLOR_TRANSPARENT;

    return mmu.readHalfFast(MAP_PALETTE + 0x20 * palette + 2 * index);
}

int PPU::readFgColor(int index, int palette)
{
    if (index == 0)
        return COLOR_TRANSPARENT;

    return mmu.readHalfFast(MAP_PALETTE + 0x200 + 0x20 * palette + 2 * index);
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

int PPU::readTilePixel(u32 addr, int x, int y, bool flip_x, bool flip_y, PixelFormat format)
{
    if (flip_x) x = 7 - x;
    if (flip_y) y = 7 - y;

    return readPixel(addr, x, y, format);
}

void PPU::applyEffects()
{
    applyMosaic();
}

void PPU::applyMosaic()
{
    for (int bg = 0; bg < 4; ++bg)
    {
        if (mmu.dispcnt.bg(bg) && mmu.bgcnt[bg].mosaic)
            applyMosaicBg(buffer_bg[bg]);
    }
}

void PPU::applyMosaicBg(BgBuffer& buffer)
{
    int mosaic_x = mmu.mosaic.bg_x + 1;
    int mosaic_y = mmu.mosaic.bg_y + 1;

    if (mmu.vcount.line % mosaic_y == 0)
    {
        if (mosaic_x == 1)
            return;

        int color;
        for (int x = 0; x < WIDTH; ++x)
        {
            if (x % mosaic_x == 0)
                color = buffer[x];

            buffer[x] = color;
        }
    }
    else
    {
        buffer.copyPage();
    }
}
