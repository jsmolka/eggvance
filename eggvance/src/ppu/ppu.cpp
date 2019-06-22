#include "ppu.h"

#include "mmu/map.h"

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

    buffer[0].flip();
    buffer[1].flip();
    buffer[2].flip();
    buffer[3].flip();

    sprites.fill({ COLOR_TRANSPARENT, 0, 4 });

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

    effects();

    generateScanline();
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
        screen.data(),
        2 * WIDTH
    );
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);
}

void PPU::generateScanline()
{
    int backdrop = mmu.readHalfFast(MAP_PALETTE);
    u16* scanline = &screen[WIDTH * mmu.vcount];
    for (int x = 0; x < WIDTH; ++x)
    {
        int pixel = backdrop;

        for (int priority = 0; priority < 4; ++priority)
        {
            for (int bg = 3; bg > -1; --bg)
            {
                if (mmu.dispcnt.bg(bg) && mmu.bgcnt[bg].priority == priority)
                {
                    int color = buffer[bg][x];
                    if (color != COLOR_TRANSPARENT)
                        pixel = color;
                }
            }

            if (mmu.dispcnt.sprites)
            {
                SpritePixel& sprite = sprites[x];
                if (sprite.priority == priority && sprite.color != COLOR_TRANSPARENT)
                    pixel = sprite.color;
            }

            if (pixel != COLOR_TRANSPARENT)
                break;
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
