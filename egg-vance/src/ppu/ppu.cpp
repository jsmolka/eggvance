#include "ppu.h"

/**
 * Todo:
 * - possibly use an SDL texture for better performance
 * - implement proper window scaling
 */

PPU::PPU(MMU& mmu)
    : mmu(mmu)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "egg-vance",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        480, 320,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, 240, 160);
}

PPU::~PPU()
{
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

    switch (mmu.dispcnt.bg_mode)
    {
    case 0:
        break;

    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        renderBitmap();
        break;

    case 5:
        break;
    }
}

void PPU::hblank()
{
    mmu.dispstat.hblank = true;
    mmu.dispstat.vblank = false;

    if (mmu.dispstat.hblank_irq)
    {
        // Todo: raise interrupt
    }
}

void PPU::vblank()
{
    mmu.dispstat.hblank = false;
    mmu.dispstat.vblank = true;

    if (mmu.dispstat.vblank_irq)
    {
        // Todo: raise interrupt
    }
}

void PPU::next()
{
    bool vcount_match = mmu.vcount.ly == mmu.dispstat.vcount_trigger;

    mmu.vcount.ly = (mmu.vcount.ly + 1) % 228;
    mmu.dispstat.vcount_match = vcount_match;

    if (vcount_match && mmu.dispstat.vcount_irq)
    {
        // Todo: raise interrupt
    }
}

void PPU::update()
{
    SDL_RenderPresent(renderer);
}

void PPU::drawPixel(int x, int y, u16 color)
{
    u8 r = 8 * ((color >>  0) & 0x1F);
    u8 g = 8 * ((color >>  5) & 0x1F);
    u8 b = 8 * ((color >> 10) & 0x1F);

    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(renderer, x, y);
}
