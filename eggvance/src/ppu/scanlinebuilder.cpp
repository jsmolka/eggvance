#include "scanlinebuilder.h"

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
    layers.push_back(Layer(mmu.palette.get<u16>(0), LF_BDP));
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
        return mmu.winin.win0.sfx;

    case WF_WIN1: 
        return mmu.winin.win1.sfx;

    case WF_WINOBJ: 
        return mmu.winout.winobj.sfx;

    case WF_WINOUT: 
        return mmu.winout.winout.sfx;

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
    if (mmu.bldcnt.upper.bg0) mask |= LF_BG0;
    if (mmu.bldcnt.upper.bg1) mask |= LF_BG1;
    if (mmu.bldcnt.upper.bg2) mask |= LF_BG2;
    if (mmu.bldcnt.upper.bg3) mask |= LF_BG3;
    if (mmu.bldcnt.upper.obj) mask |= LF_OBJ;
    if (mmu.bldcnt.upper.bdp) mask |= LF_BDP;
    
    return mask;
}

int ScanlineBuilder::blendBMask() const
{
    int mask = 0;
    if (mmu.bldcnt.lower.bg0) mask |= LF_BG0;
    if (mmu.bldcnt.lower.bg1) mask |= LF_BG1;
    if (mmu.bldcnt.lower.bg2) mask |= LF_BG2;
    if (mmu.bldcnt.lower.bg3) mask |= LF_BG3;
    if (mmu.bldcnt.lower.obj) mask |= LF_OBJ;
    if (mmu.bldcnt.lower.bdp) mask |= LF_BDP;
    
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
        if (mmu.winin.win0.bg0) mask |= LF_BG0;
        if (mmu.winin.win0.bg1) mask |= LF_BG1;
        if (mmu.winin.win0.bg2) mask |= LF_BG2;
        if (mmu.winin.win0.bg3) mask |= LF_BG3;
        if (mmu.winin.win0.obj) mask |= LF_OBJ;
        break;

    case WF_WIN1:
        if (mmu.winin.win1.bg0) mask |= LF_BG0;
        if (mmu.winin.win1.bg1) mask |= LF_BG1;
        if (mmu.winin.win1.bg2) mask |= LF_BG2;
        if (mmu.winin.win1.bg3) mask |= LF_BG3;
        if (mmu.winin.win1.obj) mask |= LF_OBJ;
        break;

    case WF_WINOBJ:
        if (mmu.winout.winobj.bg0) mask |= LF_BG0;
        if (mmu.winout.winobj.bg1) mask |= LF_BG1;
        if (mmu.winout.winobj.bg2) mask |= LF_BG2;
        if (mmu.winout.winobj.bg3) mask |= LF_BG3;
        if (mmu.winout.winobj.obj) mask |= LF_OBJ;
        break;

    case WF_WINOUT:
        if (mmu.winout.winout.bg0) mask |= LF_BG0;
        if (mmu.winout.winout.bg1) mask |= LF_BG1;
        if (mmu.winout.winout.bg2) mask |= LF_BG2;
        if (mmu.winout.winout.bg3) mask |= LF_BG3;
        if (mmu.winout.winout.obj) mask |= LF_OBJ;
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
    int x_min = mmu.winh[window].min;
    int x_max = mmu.winh[window].max;
    int y_min = mmu.winv[window].min;
    int y_max = mmu.winv[window].max;

    if (x_max > WIDTH  || x_max < x_min) x_max = WIDTH;
    if (y_max > HEIGHT || y_max < y_min) y_max = HEIGHT;

    bool inside_h = x >= x_min && x < x_max;
    bool inside_v = y >= y_min && y < y_max;

    return inside_h && inside_v;
}
