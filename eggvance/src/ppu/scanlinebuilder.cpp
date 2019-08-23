#include "scanlinebuilder.h"

ScanlineBuilder::ScanlineBuilder(DoubleBuffer<u16>(&bgs)[4], Buffer<ObjectData>& obj, MMU& mmu)
    : bgs(bgs)
    , obj(obj)
    , mmu(mmu)
    , mmio(mmu.mmio)
{
    y = mmio.vcount;

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
            if (mask & flags[bg] && bgs[bg][x] != TRANSPARENT && mmio.bgcnt[bg].priority == priority)
            {
                layers.push_back(Layer(bgs[bg][x], flags[bg]));
            }
        }
    }
    layers.push_back(Layer(mmu.palette.readHalf(0), LF_BDP));
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
        return mmio.winin.win0.sfx;

    case WF_WIN1: 
        return mmio.winin.win1.sfx;

    case WF_WINOBJ: 
        return mmio.winout.winobj.sfx;

    case WF_WINOUT: 
        return mmio.winout.winout.sfx;

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
    switch (mmio.dispcnt.mode)
    {
    case 0:
        if (mmio.dispcnt.bg0) mask |= LF_BG0;
        if (mmio.dispcnt.bg1) mask |= LF_BG1;
        if (mmio.dispcnt.bg2) mask |= LF_BG2;
        if (mmio.dispcnt.bg3) mask |= LF_BG3;
        break;

    case 1:
        if (mmio.dispcnt.bg0) mask |= LF_BG0;
        if (mmio.dispcnt.bg1) mask |= LF_BG1;
        if (mmio.dispcnt.bg2) mask |= LF_BG2;
        break;

    case 2:
        if (mmio.dispcnt.bg2) mask |= LF_BG2;
        if (mmio.dispcnt.bg3) mask |= LF_BG3;
        break;

    case 3:
    case 4:
    case 5:
        if (mmio.dispcnt.bg2) mask |= LF_BG2;
        break;
    }

    if (mmio.dispcnt.obj)
        mask |= LF_OBJ;

    return mask;
}

int ScanlineBuilder::blendAMask() const
{
    int mask = 0;
    if (mmio.bldcnt.upper.bg0) mask |= LF_BG0;
    if (mmio.bldcnt.upper.bg1) mask |= LF_BG1;
    if (mmio.bldcnt.upper.bg2) mask |= LF_BG2;
    if (mmio.bldcnt.upper.bg3) mask |= LF_BG3;
    if (mmio.bldcnt.upper.obj) mask |= LF_OBJ;
    if (mmio.bldcnt.upper.bdp) mask |= LF_BDP;
    
    return mask;
}

int ScanlineBuilder::blendBMask() const
{
    int mask = 0;
    if (mmio.bldcnt.lower.bg0) mask |= LF_BG0;
    if (mmio.bldcnt.lower.bg1) mask |= LF_BG1;
    if (mmio.bldcnt.lower.bg2) mask |= LF_BG2;
    if (mmio.bldcnt.lower.bg3) mask |= LF_BG3;
    if (mmio.bldcnt.lower.obj) mask |= LF_OBJ;
    if (mmio.bldcnt.lower.bdp) mask |= LF_BDP;
    
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
        if (mmio.winin.win0.bg0) mask |= LF_BG0;
        if (mmio.winin.win0.bg1) mask |= LF_BG1;
        if (mmio.winin.win0.bg2) mask |= LF_BG2;
        if (mmio.winin.win0.bg3) mask |= LF_BG3;
        if (mmio.winin.win0.obj) mask |= LF_OBJ;
        break;

    case WF_WIN1:
        if (mmio.winin.win1.bg0) mask |= LF_BG0;
        if (mmio.winin.win1.bg1) mask |= LF_BG1;
        if (mmio.winin.win1.bg2) mask |= LF_BG2;
        if (mmio.winin.win1.bg3) mask |= LF_BG3;
        if (mmio.winin.win1.obj) mask |= LF_OBJ;
        break;

    case WF_WINOBJ:
        if (mmio.winout.winobj.bg0) mask |= LF_BG0;
        if (mmio.winout.winobj.bg1) mask |= LF_BG1;
        if (mmio.winout.winobj.bg2) mask |= LF_BG2;
        if (mmio.winout.winobj.bg3) mask |= LF_BG3;
        if (mmio.winout.winobj.obj) mask |= LF_OBJ;
        break;

    case WF_WINOUT:
        if (mmio.winout.winout.bg0) mask |= LF_BG0;
        if (mmio.winout.winout.bg1) mask |= LF_BG1;
        if (mmio.winout.winout.bg2) mask |= LF_BG2;
        if (mmio.winout.winout.bg3) mask |= LF_BG3;
        if (mmio.winout.winout.obj) mask |= LF_OBJ;
        break;
    }
    return mask;
}

WindowFlag ScanlineBuilder::activeWindow() const
{
    if (mmio.dispcnt.win0 || mmio.dispcnt.win1 || mmio.dispcnt.winobj)
    {
        if (mmio.dispcnt.win0 && insideWindow(0))
            return WF_WIN0;

        if (mmio.dispcnt.win1 && insideWindow(1))
            return WF_WIN1;

        if (mmio.dispcnt.winobj && obj[x].window && obj[x].color != TRANSPARENT)
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
    int x_min = mmio.winh[window].min;
    int x_max = mmio.winh[window].max;
    int y_min = mmio.winv[window].min;
    int y_max = mmio.winv[window].max;

    if (x_max > WIDTH  || x_max < x_min) x_max = WIDTH;
    if (y_max > HEIGHT || y_max < y_min) y_max = HEIGHT;

    bool inside_h = x >= x_min && x < x_max;
    bool inside_v = y >= y_min && y < y_max;

    return inside_h && inside_v;
}
