#pragma once

#include <string>

#include <SDL.h>

#include "arm7.h"
#include "mmu.h"

class GBA
{
public:
    GBA();
    ~GBA();

    void run(const std::string &filepath);

private:
    void reset();

    void drawTiles();
    void drawTile(int x, int y, u16 tile_number, u8 palette_number);
    void drawPixel(int x, int y, u16 c);

    ARM7 arm;
    MMU mmu;

    bool running;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
};

