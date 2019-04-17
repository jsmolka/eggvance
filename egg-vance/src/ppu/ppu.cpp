#include "ppu.h"

/**
 * Todo:
 * - possibly use an SDL texture for better performance
 * - implement proper window scaling
 */

PPU::PPU()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "egg-vance",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        240, 160,
        SDL_WINDOW_OPENGL
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    // SDL_SetRelativeMouseMode(SDL_TRUE);
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

void PPU::renderFrame()
{
    //renderText();
    renderBitmap();

    SDL_RenderPresent(renderer);
}

void PPU::drawPixel(int x, int y, u16 color)
{
    u8 r = 8 * (color & 0x1F);
    u8 g = 8 * (color >> 5 & 0x1F);
    u8 b = 8 * (color >> 10 & 0x1F);

    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawPoint(renderer, x, y);
}
