#pragma once

#include <array>
#include <SDL2/SDL.h>

#include "mmu/mmu.h"
#include "doublebuffer.h"
#include "enums.h"
#include "objdata.h"

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

    void present();

private:
    MMU& mmu;

    using RenderFunc = void(PPU::*)(int);

    void renderBg(RenderFunc func, int bg);
    void renderBgMode0(int bg);
    void renderBgMode2(int bg);
    void renderBgMode3(int bg);
    void renderBgMode4(int bg);
    void renderBgMode5(int bg);
    void renderObjects();

    void mosaic(int bg);
    bool mosaicDominant() const;

    void generate();

    int blendAlpha(int a, int b) const;
    int blendWhite(int a) const;
    int blendBlack(int a) const;

    int readBgColor(int index, int palette);
    int readFgColor(int index, int palette);
    int readPixel(u32 addr, int x, int y, PixelFormat format);
       
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    DoubleBuffer<u16> bgs[4];
    Buffer<ObjData> obj;
    bool objects_exist;

    std::array<u16, WIDTH * HEIGHT> screen;
};
