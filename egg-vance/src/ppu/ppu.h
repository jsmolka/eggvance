#pragma once

#include <array>
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

    void clear();
    void render();

private:
    enum Screen
    {
        WIDTH  = 240,
        HEIGHT = 160
    };
    enum PixelFormat
    {
        BPP4,  // 4 bits per pixel (16/16 palette)
        BPP8   // 8 bits per pixel (256/1 palette)
    };

    MMU& mmu;

    void renderMode0();
    void renderMode0Layer(int layer);

    int initialMapBlock(const Bgcnt& bgcnt, int offset_x, int offset_y);
    int nextHorizontalMapBlock(const Bgcnt& bgcnt, int block);

    void renderMode1();
    void renderMode2();
    void renderMode3();
    void renderMode4();
    void renderMode5();
    void renderSprites();

    void draw(int x, int y, int color);

    int readBgColor(int index, int palette);
    int readFgColor(int index, int palette);
    int readPixel(u32 addr, int x, int y, PixelFormat format);

    int readTilePixel(u32 addr, int x, int y, bool flip_x, bool flip_y, PixelFormat format);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::array<u16, WIDTH * HEIGHT> buffer;
};
