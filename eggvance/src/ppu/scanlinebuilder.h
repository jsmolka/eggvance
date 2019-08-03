#pragma once

#include <vector>

#include "mmu/mmu.h"
#include "buffer.h"
#include "doublebuffer.h"
#include "objectdata.h"

struct Layer
{
    Layer(int color, LayerFlag flag)
        : color(color), flag(flag) { }

    int color;
    LayerFlag flag;
};

class ScanlineBuilder
{
public:
    ScanlineBuilder(DoubleBuffer<u16>(&bgs)[4], Buffer<ObjectData>& obj, MMU& mmu);

    void build(int x);

    bool getBlendLayers(int& a);
    bool getBlendLayers(int& a, int& b);

    bool windowSfx();

    std::vector<Layer>::iterator begin();
    std::vector<Layer>::iterator end();

private:
    constexpr static LayerFlag flags[4] = { LF_BG0, LF_BG1, LF_BG2, LF_BG3 };

    int masterMask() const;
    int blendAMask() const;
    int blendBMask() const;
    int windowMask() const;

    WindowFlag activeWindow() const;
    bool insideWindow(int window) const;

    MMU& mmu;

    DoubleBuffer<u16>(&bgs)[4];
    Buffer<ObjectData>& obj;

    int x;
    int y;

    int mask_master;
    int mask_blend_a;
    int mask_blend_b;

    WindowFlag window;

    std::vector<Layer> layers;
};
