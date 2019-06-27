#pragma once

#include <vector>

#include "mmu/mmu.h"
#include "buffer.h"
#include "doublebuffer.h"
#include "objdata.h"

struct Layer
{
    Layer(u16 color, LayerFlag flag)
        : color(color)
        , flag(flag) 
    { }

    u16 color;
    LayerFlag flag;
};

class ScanlineBuilder
{
public:
    ScanlineBuilder(DoubleBuffer<u16>(&bgs)[4], Buffer<ObjData>& obj, MMU& mmu);

    void build(int x);

    bool getBlendLayers(int& a);
    bool getBlendLayers(int& a, int& b);

    bool canBlend();

    std::vector<Layer>::iterator begin();
    std::vector<Layer>::iterator end();

private:
    bool inWin(int win);

    int allowedMask();

    MMU& mmu;

    DoubleBuffer<u16>(&bgs)[4];
    Buffer<ObjData>& objs;

    int x;
    int y;

    bool on[4];
    bool on_obj;

    int priorities[4];

    int blend_mask_a;
    int blend_mask_b;

    constexpr static LayerFlag flags[4] = {
        LF_BG0, LF_BG1, LF_BG2, LF_BG3
    };

    std::vector<Layer> layers;
};
