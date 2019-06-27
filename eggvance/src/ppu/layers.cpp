#include "layers.h"

#include "mmu/map.h"

Layers::Layers(DoubleBuffer<u16>(&bgs)[4], Buffer<ObjPixel>& obj, MMU& mmu)
    : bgs(bgs)
    , objs(obj)
    , mmu(mmu)
{
    layers.reserve(6);

    for (int x = 0; x < 4; ++x)
    {
        on[x] = mmu.dispcnt.bg(x);
        prios[x] = mmu.bgcnt[x].priority;
    }
    on_obj = mmu.dispcnt.sprites;

    if (mmu.bldcnt.a_bg0) blend_mask_a |= LF_BG0;
    if (mmu.bldcnt.a_bg1) blend_mask_a |= LF_BG1;
    if (mmu.bldcnt.a_bg2) blend_mask_a |= LF_BG2;
    if (mmu.bldcnt.a_bg3) blend_mask_a |= LF_BG3;
    if (mmu.bldcnt.a_obj) blend_mask_a |= LF_OBJ;
    if (mmu.bldcnt.a_bdp) blend_mask_a |= LF_BDP;
    if (mmu.bldcnt.b_bg0) blend_mask_b |= LF_BG0;
    if (mmu.bldcnt.b_bg1) blend_mask_b |= LF_BG1;
    if (mmu.bldcnt.b_bg2) blend_mask_b |= LF_BG2;
    if (mmu.bldcnt.b_bg3) blend_mask_b |= LF_BG3;
    if (mmu.bldcnt.b_obj) blend_mask_b |= LF_OBJ;
    if (mmu.bldcnt.b_bdp) blend_mask_b |= LF_BDP;
}

void Layers::arrange(int x)
{
    this->x = x;

    bool opaque[4] = {
        bgs[0][x] != COLOR_TRANSPARENT,
        bgs[1][x] != COLOR_TRANSPARENT,
        bgs[2][x] != COLOR_TRANSPARENT,
        bgs[3][x] != COLOR_TRANSPARENT,
    };
    bool opaque_obj = objs[x].color != COLOR_TRANSPARENT;

    layers.clear();

    for (int prio = 0; prio < 4; ++prio)
    {
        if (on_obj && opaque_obj && objs[x].prio == prio)
        {
            layers.push_back(Layer(objs[x].color, LF_OBJ));
        }

        for (int bg = 0; bg < 4; ++bg)
        {
            if (on[bg] && opaque[bg] && prios[bg] == prio)
            {
                layers.push_back(Layer(bgs[bg][x], flags[bg]));
            }
        }
    }
    layers.push_back(Layer(mmu.readHalfFast(MAP_PALETTE), LF_BDP));
}

bool Layers::getBlendLayers(u16& a)
{
    if (objs[x].semi)
    {
        for (Layer& layer : layers)
        {
            if (layer.flag & LF_OBJ)
            {
                a = layer.color;
                return true;
            }
        }
    }
    else
    {
        if (layers[0].flag & blend_mask_a)
        {
            a = layers[0].color;
            return true;
        }
    }
    return false;
}

bool Layers::getBlendLayers(u16& a, u16& b)
{
    bool found_a = false;

    int mask_a = objs[x].semi ? LF_OBJ : blend_mask_a;

    for (Layer& layer : layers)
    {
        if (layer.flag & mask_a && !found_a)
        {
            a = layer.color;
            found_a = true;
        }
        else if (layer.flag & blend_mask_b)
        {
            b = layer.color;
            return found_a;
        }
        else
        {
            return false;
        }
    }
    return false;
}

std::vector<Layer>::iterator Layers::begin()
{
    return layers.begin();
}

std::vector<Layer>::iterator Layers::end()
{
    return layers.end();
}
