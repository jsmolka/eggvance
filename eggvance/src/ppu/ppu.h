#pragma once

#include <array>
#include <SDL.h>

#include "mmu/mmu.h"
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
    enum Color
    { 
        COLOR_TRANSPARENT = 0x8000 
    };
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
    struct SpritePixel
    {
        u16 pixel;
        int entry;
        int priority;
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

    void effects();

    void mosaic();
    void mosaicBg(DoubleBuffer& buffer);

    struct BlendPixel
    {
        u16* pixel = nullptr;
        int prio = 4;
    };
    void blend();
    BlendPixel blendLayerA(int x);
    BlendPixel blendLayerB(int x);
    void alphaBlend(u16* a, u16* b);

    void generateScanline();

    int readBgColor(int index, int palette);
    int readFgColor(int index, int palette);
    int readPixel(u32 addr, int x, int y, PixelFormat format);
       
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    DoubleBuffer buffer[4];
    std::array<SpritePixel, WIDTH> sprites;
    std::array<u16, WIDTH * HEIGHT> screen;
};
