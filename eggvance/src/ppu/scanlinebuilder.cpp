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

    findActiveWindow();

    bool opaque[4] = {
        bgs[0][x] != TRANSPARENT,
        bgs[1][x] != TRANSPARENT,
        bgs[2][x] != TRANSPARENT,
        bgs[3][x] != TRANSPARENT,
    };
    bool opaque_obj = objs[x].color != TRANSPARENT;

    int window_flags = windowLayerFlags();

    layers.clear();

    for (int priority = 0; priority < 4; ++priority)
    {
        if (on_obj && opaque_obj && objs[x].priority == priority)
        {
            if (window_flags & LF_OBJ)
                layers.push_back(Layer(objs[x].color, LF_OBJ));
        }

        for (int bg = 0; bg < 4; ++bg)
        {
            if (on[bg] && opaque[bg] && priorities[bg] == priority)
            {
                if (window_flags & flag_lut[bg])
                    layers.push_back(Layer(bgs[bg][x], flag_lut[bg]));
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
    bool valid = false;

    int mask_a = objs[x].mode == GFX_ALPHA ? LF_OBJ : blend_mask_a;

    for (Layer& layer : layers)
    {
        if (layer.flag & mask_a && !valid)
        {
            a = layer.color;
            valid = true;
        }
        else if (layer.flag & blend_mask_b)
        {
            b = layer.color;
            return valid;
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
    switch (window)
    {
    case WF_DISABLED: 
        return true;

    case WF_WIN0: 
        return mmu.winin.win0_sfx;

    case WF_WIN1: 
        return mmu.winin.win1_sfx;

    case WF_WINOBJ: 
        return mmu.winout.winobj_sfx;

    case WF_WINOUT: 
        return mmu.winout.winout_sfx;
    }
}

std::vector<Layer>::iterator ScanlineBuilder::begin()
{
    return layers.begin();
}

std::vector<Layer>::iterator ScanlineBuilder::end()
{
    return layers.end();
}

void ScanlineBuilder::findActiveWindow()
{
    if (mmu.dispcnt.win0 || mmu.dispcnt.win1 || mmu.dispcnt.winobj)
    {
        if (insideWindow(0))
        {
            window = WF_WIN0;
        }
        else if (insideWindow(1))
        {
            window = WF_WIN1;
        }
        else if (objs[x].mode == GFX_WINDOW && objs[x].color != TRANSPARENT)
        {
            window = WF_WINOBJ;
        }
        else
        {
            window = WF_WINOUT;
        }
    }
    else
    {
        window = WF_DISABLED;
    }
}

bool ScanlineBuilder::insideWindow(int window) const
{
    int x1 = mmu.winh[window].x1;
    int x2 = mmu.winh[window].x2;
    int y1 = mmu.winv[window].y1;
    int y2 = mmu.winv[window].y2;

    if (x2 > WIDTH  || x2 < x1) x2 = WIDTH;
    if (y2 > HEIGHT || y2 < y1) y2 = HEIGHT;

    bool inside_h = x >= x1 && x < x2;
    bool inside_v = y >= y1 && y < y2;

    return inside_h && inside_v;
}

int ScanlineBuilder::windowLayerFlags() const
{
    if (window == WF_DISABLED)
        return 0xFF;

    int flags = 0;
    switch (window)
    {
    case WF_WIN0:
        if (mmu.winin.win0_bg0) flags |= LF_BG0;
        if (mmu.winin.win0_bg1) flags |= LF_BG1;
        if (mmu.winin.win0_bg2) flags |= LF_BG2;
        if (mmu.winin.win0_bg3) flags |= LF_BG3;
        if (mmu.winin.win0_obj) flags |= LF_OBJ;
        break;

    case WF_WIN1:
        if (mmu.winin.win1_bg0) flags |= LF_BG0;
        if (mmu.winin.win1_bg1) flags |= LF_BG1;
        if (mmu.winin.win1_bg2) flags |= LF_BG2;
        if (mmu.winin.win1_bg3) flags |= LF_BG3;
        if (mmu.winin.win1_obj) flags |= LF_OBJ;
        break;

    case WF_WINOBJ:
        if (mmu.winout.winobj_bg0) flags |= LF_BG0;
        if (mmu.winout.winobj_bg1) flags |= LF_BG1;
        if (mmu.winout.winobj_bg2) flags |= LF_BG2;
        if (mmu.winout.winobj_bg3) flags |= LF_BG3;
        if (mmu.winout.winobj_obj) flags |= LF_OBJ;
        break;

    case WF_WINOUT:
        if (mmu.winout.winout_bg0) flags |= LF_BG0;
        if (mmu.winout.winout_bg1) flags |= LF_BG1;
        if (mmu.winout.winout_bg2) flags |= LF_BG2;
        if (mmu.winout.winout_bg3) flags |= LF_BG3;
        if (mmu.winout.winout_obj) flags |= LF_OBJ;
        break;
    }
    return flags;
}
