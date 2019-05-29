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

    void renderMode0();
    void renderMode0Layer(int layer);

    enum PixelFormat
    {
        BPP4,  // 4 bits per pixel (16/16 palette)
        BPP8   // 8 bits per pixel (256/1 palette)
    };
    int readTilePixel(u32 addr, int x, int y, bool flip_x, bool flip_y, PixelFormat format);
    int initialMapBlock(const Bgcnt& bgcnt, int offset_x, int offset_y);
    int nextHorizontalMapBlock(const Bgcnt& bgcnt, int block);



    void renderMode1();
    void renderMode2();
    void renderMode3();
    void renderMode4();
    void renderMode5();
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

