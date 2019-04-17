#pragma once

#include <SDL.h>

#include "mmu/mmu.h"

class PPU
{
public:
    PPU();
    ~PPU();

    void reset();

    void renderFrame();

    MMU* mmu;

private:
    void renderText();
    void renderBitmap();

    void drawPixel(int x, int y, u16 color);

    SDL_Window* window;
    SDL_Renderer* renderer;
};

