#pragma once

#include <vector>

#include "mmu/mmu.h"
#include "buffer.h"
#include "doublebuffer.h"
#include "common.h"

enum LayerFlag
{
    LF_NON = 1 << 0,
    LF_BG0 = 1 << 1,
    LF_BG1 = 1 << 2,
    LF_BG2 = 1 << 3,
    LF_BG3 = 1 << 4,
    LF_OBJ = 1 << 5,
    LF_BDP = 1 << 6
};

struct Layer
{
    Layer(u16 color, LayerFlag flag)
        : color(color), flag(flag) { }

    u16 color;
    LayerFlag flag;
};

class Layers
{
public:
    Layers(DoubleBuffer<u16>(&bgs)[4], Buffer<ObjPixel>& obj, MMU& mmu);

    void arrange(int x);

    bool getBlendLayers(u16& a);
    bool getBlendLayers(u16& a, u16& b);

    std::vector<Layer>::iterator begin();
    std::vector<Layer>::iterator end();

private:
    MMU& mmu;

    DoubleBuffer<u16>(&bgs)[4];
    Buffer<ObjPixel>& objs;

    int x;
    bool on[4];
    bool on_obj;

    int prios[4];

    int blend_mask_a;
    int blend_mask_b;

    constexpr static LayerFlag flags[4] = {
        LF_BG0, LF_BG1, LF_BG2, LF_BG3
    };

    std::vector<Layer> layers;
};
