#pragma once

#include <array>
#include <SDL.h>

#include "mmu/mmu.h"
#include "bgbuffer.h"

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

    MMU& mmu;

    int initialMapBlock(const Bgcnt& bgcnt, int offset_x, int offset_y);
    int nextHorizontalMapBlock(const Bgcnt& bgcnt, int block);

    void renderMode0();
    void renderMode1();
    void renderMode2();
    void renderMode3();
    void renderMode4();
    void renderMode5();
    void renderSprites();

    void renderBackgroundMode0(int layer);
    void renderBackgroundMode2(int layer);

    void generateScanline();

    int readBgColor(int index, int palette);
    int readFgColor(int index, int palette);
    int readPixel(u32 addr, int x, int y, PixelFormat format);

    int readTilePixel(u32 addr, int x, int y, bool flip_x, bool flip_y, PixelFormat format);

    void applyEffects();
    void applyMosaic();
    void applyMosaicBg(BgBuffer& buffer);
       
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    union
    {
        struct
        {
            BgBuffer buffer_bg0;
            BgBuffer buffer_bg1;
            BgBuffer buffer_bg2;
            BgBuffer buffer_bg3;
        };
        BgBuffer buffer_bg[4];
    };

    std::array<std::array<u16, WIDTH>, 4> buffer_sprites;

    std::array<u16, WIDTH * HEIGHT> buffer_screen;
};
