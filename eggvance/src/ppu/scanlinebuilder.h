#pragma once

#include <vector>

#include "mmu/mmu.h"
#include "buffer.h"
#include "objectdata.h"

enum OldLayerFlag
{
    OLF_BG0  = 1 << 0,  // BG0 layer
    OLF_BG1  = 1 << 1,  // BG1 layer
    OLF_BG2  = 1 << 2,  // BG2 layer
    OLF_BG3  = 1 << 3,  // BG3 layer
    OLF_OBJ  = 1 << 4,  // OBJ layer
    OLF_BDP  = 1 << 5   // Backdrop
};

enum WindowFlag
{
    WF_DISABLED = 1 << 0,  // No window
    WF_WIN0     = 1 << 1,  // Window 0
    WF_WIN1     = 1 << 2,  // Window 1
    WF_WINOBJ   = 1 << 3,  // Object window
    WF_WINOUT   = 1 << 4   // Outside window
};

struct OldLayer
{
    OldLayer(int color, OldLayerFlag flag)
        : color(color), flag(flag) { }

    int color;
    OldLayerFlag flag;
};

class ScanlineBuilder
{
public:
    ScanlineBuilder(DoubleBuffer<u16>(&bgs)[4], Buffer<ObjectData>& obj, MMU& mmu);

    void build(int x);

    bool getBlendLayers(int& a);
    bool getBlendLayers(int& a, int& b);

    bool windowSfx();

    std::vector<OldLayer>::iterator begin();
    std::vector<OldLayer>::iterator end();

private:
    constexpr static OldLayerFlag flags[4] = { OLF_BG0, OLF_BG1, OLF_BG2, OLF_BG3 };

    int masterMask() const;
    int blendAMask() const;
    int blendBMask() const;
    int windowMask() const;

    WindowFlag activeWindow() const;
    bool insideWindow(int window) const;

    MMU& mmu;
    MMIO& mmio;

    DoubleBuffer<u16>(&bgs)[4];
    Buffer<ObjectData>& obj;

    int x;
    int y;

    int mask_master;
    int mask_blend_a;
    int mask_blend_b;

    WindowFlag window;

    std::vector<OldLayer> layers;
};
