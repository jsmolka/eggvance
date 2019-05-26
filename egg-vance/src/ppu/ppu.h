#pragma once

#include <array>
#include <SDL.h>

#include "mmu/mmu.h"
#include "sprites/sprite.h"

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

    void clear();
    void render();

private:
    enum Screen
    {
        WIDTH  = 240,
        HEIGHT = 160
    };

    MMU& mmu;

    void renderText();
    void renderBitmapMode3();
    void renderBitmapMode4();
    void renderBitmapMode5();
    void renderSprites();

    void updateSprites();

    void draw(int x, int y, int color);

    int readBgColor(int index, int palette);
    int readFgColor(int index, int palette);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::array<u16, WIDTH * HEIGHT> buffer;
    std::array<Sprite, 128> sprites;
};

