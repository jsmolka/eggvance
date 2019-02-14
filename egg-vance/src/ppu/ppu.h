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

    void renderText();

    MMU* mmu;

private:
    void draw(int x, int y, u16 color);
    void redraw();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
};

