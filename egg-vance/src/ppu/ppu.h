#pragma once

#include <SDL.h>

#include "mmu/mmu.h"

class PPU
{
public:
    PPU(MMU& mmu);
    ~PPU();

    void reset();

    void scanline();
    void hblank();
    void vblank();
    void next();

    void render();

private:
    MMU& mmu;

    void renderText();
    void renderBitmap();

    void pixel(int x, int y, int color);

    SDL_Window* window;
    SDL_Renderer* renderer;
};

