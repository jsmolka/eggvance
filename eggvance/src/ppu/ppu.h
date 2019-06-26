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
    struct SpriteMeta
    {
        int entry;
        int semi_transparent;
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
    void mosaicBg(DoubleBuffer<u16>& buffer);

    void blend();
    
    bool findBlendLayers(int x, u16*& a);
    bool findBlendLayers(int x, u16*& a, int& b);
    
    void alphaBlend(u16* a, int b);
    void fadeToWhite(u16* a);
    void fadeToBlack(u16* a);

    void generateScanline();

    int readBgColor(int index, int palette);
    int readFgColor(int index, int palette);
    int readPixel(u32 addr, int x, int y, PixelFormat format);
       
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    DoubleBuffer<u16> buffer[4];
    Buffer<u16> sprites;
    Buffer<SpriteMeta> sprites_meta;

    std::array<u16, WIDTH * HEIGHT> screen;
};
