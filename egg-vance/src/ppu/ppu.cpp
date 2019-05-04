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
    buffer.fill(0);
}

void PPU::scanline()
{
    mmu.dispstat.hblank = false;
    mmu.dispstat.vblank = false;

    switch (mmu.dispcnt.bg_mode)
    {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        // Bitmap modes only use BG2
        if (mmu.dispcnt.bg2)
            renderBitmapMode3();
        break;

    case 4:
        // Bitmap modes only use BG2
        if (mmu.dispcnt.bg2)
            renderBitmapMode4();
        break;

    case 5:
        // Bitmap modes only use BG2
        if (mmu.dispcnt.bg2)
            renderBitmapMode5();
        break;
    }
    renderSprites();
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

void PPU::clear()
{
    buffer.fill(0);
}

void PPU::render()
{
    SDL_UpdateTexture(
        texture,
        0,
        buffer.data(),
        2 * WIDTH
    );
    SDL_RenderCopy(renderer, texture, 0, 0);
    SDL_RenderPresent(renderer);
}

void PPU::draw(int x, int y, int color)
{
    buffer[WIDTH * y + x] = color;
}

int PPU::readColor(int index)
{
    return mmu.readHalfFast(MAP_PALETTE + 2 * index);
}

int PPU::readSpriteColor(int index)
{
    return mmu.readHalfFast(MAP_PALETTE + 0x200 + 2 * index);
}
