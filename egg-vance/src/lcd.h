#pragma once

#include <SDL.h>

#include "internal.h"
#include "lcdstat.h"
#include "mmu.h"

class Lcd : public Internal
{
public:
    Lcd();
    ~Lcd();

    void reset() final;

    void drawBg0();

    LcdStat stat;

    Mmu* mmu;

private:
    void draw(int x, int y, u16 color);
    void redraw();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
};

