#pragma once

#include <SDL.h>

#include "lcd_stat.h"
#include "mmu.h"

class LCD
{
public:
    LCD();
    ~LCD();

    void reset();

    void drawBg0();

    lcd_stat stat;

    MMU* mmu;

private:
    struct Rgb
    {
        u8 r;
        u8 g;
        u8 b;
    };

    static Rgb decodeColor(u16 color);

    void draw(int x, int y, u16 color);
    void redraw();

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
};

