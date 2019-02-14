#include "PPU.h"

PPU::PPU()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "egg-vance",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        240,
        160,
        SDL_WINDOW_OPENGL
    );
    renderer = SDL_CreateRenderer(window, -1, 0);
    surface = SDL_GetWindowSurface(window);
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

void PPU::draw(int x, int y, u16 color)
{
    u8 r = 8 * (color & 0x1F);
    u8 g = 8 * (color >> 5 & 0x1F);
    u8 b = 8 * (color >> 10 & 0x1F);

    SDL_Rect rect = { x, y, 1, 1 };
    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, r, g, b));
    SDL_RenderDrawRect(renderer, &rect);
}

void PPU::redraw()
{
    SDL_UpdateWindowSurface(window);
}
