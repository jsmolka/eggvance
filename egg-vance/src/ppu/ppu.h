#pragma once

#include <SDL.h>

#include "common/internal.h"
#include "mmu/mmu.h"

class PPU : public Internal
{
public:
    PPU();
    ~PPU();

    void reset() final;

    void renderFrame();

    MMU* mmu;

private:
    void renderText();

    void drawPixel(int x, int y, u16 color);

    SDL_Window* window;
    SDL_Renderer* renderer;
};

