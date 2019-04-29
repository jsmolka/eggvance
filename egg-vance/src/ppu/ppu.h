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

    void renderFrame();

    void update();

private:
    MMU& mmu;

    void renderText();
    void renderBitmap();

    void drawPixel(int x, int y, u16 color);

    SDL_Window* window;
    SDL_Renderer* renderer;
};

