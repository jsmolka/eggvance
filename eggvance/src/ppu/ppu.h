#pragma once

#include <array>
#include <SDL.h>

#include "mmu/mmu.h"
#include "common.h"
#include "doublebuffer.h"

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

    void render();

private:
    enum Screen
    {
        WIDTH  = 240,
        HEIGHT = 160
    };
    enum PixelFormat
    {
        BPP4,  // 4 bits per pixel (16/16)
        BPP8   // 8 bits per pixel (256/1)
    };

    MMU& mmu;

    void renderMode0();
    void renderMode1();
    void renderMode2();
    void renderMode3();
    void renderMode4();
    void renderMode5();
    void renderSprites();

    void renderBackgroundMode0(int layer);
    void renderBackgroundMode2(int layer);

    void mosaic();

    void generate();
    
    int blendAlpha(int a, int b);
    int blendWhite(int a);
    int blendBlack(int a);

    int readBgColor(int index, int palette);
    int readFgColor(int index, int palette);
    int readPixel(u32 addr, int x, int y, PixelFormat format);
       
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    DoubleBuffer<u16> bgs[4];
    Buffer<ObjPixel> objs;

    std::array<u16, WIDTH * HEIGHT> screen;
};
