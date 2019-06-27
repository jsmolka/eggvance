#include "scanlinebuilder.h"

#include "mmu/map.h"

ScanlineBuilder::ScanlineBuilder(DoubleBuffer<u16>(&bgs)[4], Buffer<ObjData>& obj, MMU& mmu)
    : bgs(bgs)
    , objs(obj)
    , mmu(mmu)
{
    y = mmu.vcount.line;

    layers.reserve(6);

    for (int x = 0; x < 4; ++x)
    {
        on[x] = mmu.dispcnt.bg(x);
        priorities[x] = mmu.bgcnt[x].priority;
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

void ScanlineBuilder::build(int x)
{
    this->x = x;

    bool opaque[4] = {
        bgs[0][x] != TRANSPARENT,
        bgs[1][x] != TRANSPARENT,
        bgs[2][x] != TRANSPARENT,
        bgs[3][x] != TRANSPARENT,
    };
    bool opaque_obj = objs[x].color != TRANSPARENT;

    int mask = allowedMask();

    layers.clear();

    for (int priority = 0; priority < 4; ++priority)
    {
        if (on_obj && opaque_obj && objs[x].priority == priority)
        {
            if (mask & LF_OBJ)
                layers.push_back(Layer(objs[x].color, LF_OBJ));
        }

        for (int bg = 0; bg < 4; ++bg)
        {
            if (on[bg] && opaque[bg] && priorities[bg] == priority)
            {
                if (mask & flags[bg])
                layers.push_back(Layer(bgs[bg][x], flags[bg]));
            }
        }
    }
    layers.push_back(Layer(mmu.readHalfFast(MAP_PALETTE), LF_BDP));
}

bool ScanlineBuilder::getBlendLayers(int& a)
{
    if (objs[x].mode == GFX_ALPHA)
    {
        for (Layer& layer : layers)
        {
            if (layer.flag == LF_OBJ)
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

bool ScanlineBuilder::getBlendLayers(int& a, int& b)
{
    bool found_a = false;

    int mask_a = objs[x].mode == GFX_ALPHA
        ? LF_OBJ 
        : blend_mask_a;

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

bool ScanlineBuilder::canBlend()
{
    bool windows = mmu.dispcnt.win0 || mmu.dispcnt.win1 || mmu.dispcnt.winobj;

    if (windows)
    {
        if (inWin(0))
            return mmu.winin.win0_sfx;
        else if (inWin(1))
            return mmu.winin.win1_sfx;
        else
            return mmu.winout.winobj_sfx;
    }
    return true;
}

std::vector<Layer>::iterator ScanlineBuilder::begin()
{
    return layers.begin();
}

std::vector<Layer>::iterator ScanlineBuilder::end()
{
    return layers.end();
}

bool ScanlineBuilder::inWin(int win)
{
    int x1 = mmu.winh[win].x1;
    int x2 = mmu.winh[win].x2;
    int y1 = mmu.winv[win].y1;
    int y2 = mmu.winv[win].y2;

    if (x2 > WIDTH  || x2 < x1) x2 = WIDTH;
    if (y2 > HEIGHT || y2 < y1) y2 = HEIGHT;

    bool h_ok = x >= x1 && x < x2;
    bool v_ok = y >= y1 && y < y2;

    return h_ok && v_ok;
}

int ScanlineBuilder::allowedMask()
{
    int x = this->x;
    int y = mmu.vcount.line;

    bool windows = mmu.dispcnt.win0 || mmu.dispcnt.win1 || mmu.dispcnt.winobj;

    if (!windows)
        return 0b111111;

    if (inWin(0))
    {
        int mask = 0;
        if (mmu.winin.win0_bg0) mask |= LF_BG0;
        if (mmu.winin.win0_bg1) mask |= LF_BG1;
        if (mmu.winin.win0_bg2) mask |= LF_BG2;
        if (mmu.winin.win0_bg3) mask |= LF_BG3;
        if (mmu.winin.win0_obj) mask |= LF_OBJ;
        return mask;
    }
    else if (inWin(1))
    {
        int mask = 0;
        if (mmu.winin.win1_bg0) mask |= LF_BG0;
        if (mmu.winin.win1_bg1) mask |= LF_BG1;
        if (mmu.winin.win1_bg2) mask |= LF_BG2;
        if (mmu.winin.win1_bg3) mask |= LF_BG3;
        if (mmu.winin.win1_obj) mask |= LF_OBJ;
        return mask;
    }
    //else if (isObjWin)
    //{

    //}
    else
    {
        int mask = 0;
        if (mmu.winout.winout_bg0) mask |= LF_BG0;
        if (mmu.winout.winout_bg1) mask |= LF_BG1;
        if (mmu.winout.winout_bg2) mask |= LF_BG2;
        if (mmu.winout.winout_bg3) mask |= LF_BG3;
        if (mmu.winout.winout_obj) mask |= LF_OBJ;
        return mask;
    }
}
