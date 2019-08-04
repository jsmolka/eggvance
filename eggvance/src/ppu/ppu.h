#pragma once

#include "mmu/mmu.h"
#include "backend.h"
#include "buffer.h"
#include "objectdata.h"

class PPU
{
public:
    PPU(MMU& mmu);

    void reset();

    void scanline();
    void hblank();
    void vblank();
    void next();

    void present();

    Backend backend;

private:
    enum PixelFormat
    {
        BPP4,  // 4 bits per pixel (16/16)
        BPP8   // 8 bits per pixel (256/1)
    };
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
    bool mosaicAffected(int bg) const;
    bool mosaicDominant() const;

    void finalize();

    int blendAlpha(int a, int b) const;
    int blendWhite(int a) const;
    int blendBlack(int a) const;

    int readBgColor(int index, int palette);
    int readFgColor(int index, int palette);
    int readPixel(u32 addr, int x, int y, PixelFormat format);
       
    DoubleBuffer<u16> bgs[4];
    Buffer<ObjectData> obj;
    bool obj_exist;

    std::array<s16*, 32> pas;
    std::array<s16*, 32> pbs;
    std::array<s16*, 32> pcs;
    std::array<s16*, 32> pds;
};
