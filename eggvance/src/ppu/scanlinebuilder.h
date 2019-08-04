#pragma once

#include <vector>

#include "mmu/mmu.h"
#include "buffer.h"
#include "objectdata.h"

enum LayerFlag
{
    LF_BG0  = 1 << 0,  // BG0 layer
    LF_BG1  = 1 << 1,  // BG1 layer
    LF_BG2  = 1 << 2,  // BG2 layer
    LF_BG3  = 1 << 3,  // BG3 layer
    LF_OBJ  = 1 << 4,  // OBJ layer
    LF_BDP  = 1 << 5   // Backdrop
};

enum WindowFlag
{
    WF_DISABLED = 1 << 0,  // No window
    WF_WIN0     = 1 << 1,  // Window 0
    WF_WIN1     = 1 << 2,  // Window 1
    WF_WINOBJ   = 1 << 3,  // Object window
    WF_WINOUT   = 1 << 4   // Outside window
};

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
