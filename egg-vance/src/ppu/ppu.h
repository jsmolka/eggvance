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

    void renderMode0();
    void renderMode0Layer(int layer);

    void renderMode1();
    void renderMode2();
    void renderMode3();
    void renderMode4();
    void renderMode5();
    void renderSprites();

    void drawTileMap(int x, int y, int map_block);

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

