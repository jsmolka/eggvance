#include "scanlinebuilder.h"

#include "mmu/map.h"

ScanlineBuilder::ScanlineBuilder(DoubleBuffer<u16>(&bgs)[4], Buffer<ObjData>& obj, MMU& mmu)
    : bgs(bgs)
    , obj(obj)
    , mmu(mmu)
{
    y = mmu.vcount;

    mask_master  = masterMask();
    mask_blend_a = blendAMask();
    mask_blend_b = blendBMask();
}

void ScanlineBuilder::build(int x)
{
    layers.clear();
    
    this->x = x;

    window = activeWindow();

    int mask = mask_master & windowMask();

    for (int priority = 0; priority < 4; ++priority)
    {
        if (mask & LF_OBJ && obj[x].color != TRANSPARENT && obj[x].priority == priority)
        {
            layers.push_back(Layer(obj[x].color, LF_OBJ));
        }

        for (int bg = 0; bg < 4; ++bg)
        {
            if (mask & flags[bg] && bgs[bg][x] != TRANSPARENT && mmu.bgcnt[bg].priority == priority)
            {
                layers.push_back(Layer(bgs[bg][x], flags[bg]));
            }
        }
    }
    layers.push_back(Layer(mmu.readHalfFast(MAP_PALETTE), LF_BDP));
}

bool ScanlineBuilder::getBlendLayers(int& a)
{
    if (obj[x].mode == GFX_ALPHA)
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
        if (layers[0].flag & mask_blend_a)
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

    int mask_a = obj[x].mode == GFX_ALPHA ? LF_OBJ : mask_blend_a;

    for (Layer& layer : layers)
    {
        if (layer.flag & mask_a && !found_a)
        {
            a = layer.color;
            found_a = true;
        }
        else if (layer.flag & mask_blend_b)
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

bool ScanlineBuilder::windowSfx()
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

    default:
        return true;
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

int ScanlineBuilder::masterMask() const
{
    int mask = LF_BDP;
    switch (mmu.dispcnt.mode)
    {
    case 0:
        if (mmu.dispcnt.bg0) mask |= LF_BG0;
        if (mmu.dispcnt.bg1) mask |= LF_BG1;
        if (mmu.dispcnt.bg2) mask |= LF_BG2;
        if (mmu.dispcnt.bg3) mask |= LF_BG3;
        break;

    case 1:
        if (mmu.dispcnt.bg0) mask |= LF_BG0;
        if (mmu.dispcnt.bg1) mask |= LF_BG1;
        if (mmu.dispcnt.bg2) mask |= LF_BG2;
        break;

    case 2:
        if (mmu.dispcnt.bg2) mask |= LF_BG2;
        if (mmu.dispcnt.bg3) mask |= LF_BG3;
        break;

    case 3:
    case 4:
    case 5:
        if (mmu.dispcnt.bg2) mask |= LF_BG2;
        break;
    }

    if (mmu.dispcnt.obj)
        mask |= LF_OBJ;

    return mask;
}

int ScanlineBuilder::blendAMask() const
{
    int mask = 0;
    if (mmu.bldcnt.a_bg0) mask |= LF_BG0;
    if (mmu.bldcnt.a_bg1) mask |= LF_BG1;
    if (mmu.bldcnt.a_bg2) mask |= LF_BG2;
    if (mmu.bldcnt.a_bg3) mask |= LF_BG3;
    if (mmu.bldcnt.a_obj) mask |= LF_OBJ;
    if (mmu.bldcnt.a_bdp) mask |= LF_BDP;
    
    return mask;
}

int ScanlineBuilder::blendBMask() const
{
    int mask = 0;
    if (mmu.bldcnt.b_bg0) mask |= LF_BG0;
    if (mmu.bldcnt.b_bg1) mask |= LF_BG1;
    if (mmu.bldcnt.b_bg2) mask |= LF_BG2;
    if (mmu.bldcnt.b_bg3) mask |= LF_BG3;
    if (mmu.bldcnt.b_obj) mask |= LF_OBJ;
    if (mmu.bldcnt.b_bdp) mask |= LF_BDP;
    
    return mask;
}

int ScanlineBuilder::windowMask() const
{
    if (window == WF_DISABLED)
        return 0xFF;

    int mask = 0;
    switch (window)
    {
    case WF_WIN0:
        if (mmu.winin.win0_bg0) mask |= LF_BG0;
        if (mmu.winin.win0_bg1) mask |= LF_BG1;
        if (mmu.winin.win0_bg2) mask |= LF_BG2;
        if (mmu.winin.win0_bg3) mask |= LF_BG3;
        if (mmu.winin.win0_obj) mask |= LF_OBJ;
        break;

    case WF_WIN1:
        if (mmu.winin.win1_bg0) mask |= LF_BG0;
        if (mmu.winin.win1_bg1) mask |= LF_BG1;
        if (mmu.winin.win1_bg2) mask |= LF_BG2;
        if (mmu.winin.win1_bg3) mask |= LF_BG3;
        if (mmu.winin.win1_obj) mask |= LF_OBJ;
        break;

    case WF_WINOBJ:
        if (mmu.winout.winobj_bg0) mask |= LF_BG0;
        if (mmu.winout.winobj_bg1) mask |= LF_BG1;
        if (mmu.winout.winobj_bg2) mask |= LF_BG2;
        if (mmu.winout.winobj_bg3) mask |= LF_BG3;
        if (mmu.winout.winobj_obj) mask |= LF_OBJ;
        break;

    case WF_WINOUT:
        if (mmu.winout.winout_bg0) mask |= LF_BG0;
        if (mmu.winout.winout_bg1) mask |= LF_BG1;
        if (mmu.winout.winout_bg2) mask |= LF_BG2;
        if (mmu.winout.winout_bg3) mask |= LF_BG3;
        if (mmu.winout.winout_obj) mask |= LF_OBJ;
        break;
    }
    return mask;
}

WindowFlag ScanlineBuilder::activeWindow() const
{
    if (mmu.dispcnt.win0 || mmu.dispcnt.win1 || mmu.dispcnt.winobj)
    {
        if (mmu.dispcnt.win0 && insideWindow(0))
            return WF_WIN0;

        if (mmu.dispcnt.win1 && insideWindow(1))
            return WF_WIN1;

        if (mmu.dispcnt.winobj && obj[x].window && obj[x].color != TRANSPARENT)
            return WF_WINOBJ;
            
        return WF_WINOUT;
    }
    else
    {
        return WF_DISABLED;
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
