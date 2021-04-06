#include "ppu.h"

#include <algorithm>
#include <shell/macros.h>
#include <shell/operators.h>

#include "frontend/videocontext.h"

void Ppu::collapse(uint bgs)
{
    BackgroundLayers backgrounds;

    for (uint bg : bit::iterate(bgs & dispcnt.layers))
    {
        backgrounds.push_back({ bgcnt[bg].priority, this->backgrounds[bg].data(), 1U << bg });
    }

    std::sort(backgrounds.begin(), backgrounds.end());

    if (objects_exist)
        collapse<1>(backgrounds);
    else
        collapse<0>(backgrounds);
}

template<bool kObjects>
void Ppu::collapse(const BackgroundLayers& backgrounds)
{
    uint window = dispcnt.win0 || dispcnt.win1 || dispcnt.winobj;
    uint blend  = bldcnt.mode != BlendMode::Disabled || objects_alpha;

    switch ((blend << 1) | window)
    {
    case 0b00: collapseNN<kObjects>(backgrounds); break;
    case 0b01: collapseNW<kObjects>(backgrounds); break;
    case 0b10: collapseBN<kObjects>(backgrounds); break;
    case 0b11: collapseBW<kObjects>(backgrounds); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool kObjects>
void Ppu::collapseNN(const BackgroundLayers& backgrounds)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        color = argb[upperLayer<kObjects>(backgrounds, x)];
    }
}

template<bool kObjects>
void Ppu::collapseNW(const BackgroundLayers& backgrounds)
{
    switch (possibleWindows<kObjects>())
    {
    SHELL_CASE08(0, collapseNW<kObjects, kLabel>(backgrounds));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool kObjects, uint kWindows>
void Ppu::collapseNW(const BackgroundLayers& backgrounds)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        const auto& window = activeWindow<kWindows>(x);

        color = argb[upperLayer<kObjects>(backgrounds, x, window.layers)];
    }
}

template<bool kObjects>
void Ppu::collapseBN(const BackgroundLayers& backgrounds)
{
    switch (bldcnt.mode)
    {
    SHELL_CASE04(0, collapseBN<kObjects, BlendMode(kLabel)>(backgrounds));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool kObjects, BlendMode kBlendMode>
void Ppu::collapseBN(const BackgroundLayers& backgrounds)
{
    constexpr auto kEnabled = 0xFFFF;

    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        u16 upper = 0;
        u16 lower = 0;

        const auto& object = objects[x];

        if (kObjects && object.alpha)
        {
            if (findBlendLayers<kObjects>(backgrounds, x, kEnabled, upper, lower))
                upper = bldalpha.blendAlpha(upper, lower);
        }
        else
        {
            switch (kBlendMode)
            {
            case BlendMode::Alpha:
                if (findBlendLayers<kObjects>(backgrounds, x, kEnabled, upper, lower))
                    upper = bldalpha.blendAlpha(upper, lower);
                break;

            case BlendMode::White:
                if (findBlendLayers<kObjects>(backgrounds, x, kEnabled, upper))
                    upper = bldfade.blendWhite(upper);
                break;

            case BlendMode::Black:
                if (findBlendLayers<kObjects>(backgrounds, x, kEnabled, upper))
                    upper = bldfade.blendBlack(upper);
                break;

            case BlendMode::Disabled:
                upper = upperLayer<kObjects>(backgrounds, x);
                break;

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        color = argb[upper];
    }
}

template<bool kObjects>
void Ppu::collapseBW(const BackgroundLayers& backgrounds)
{
    switch (bldcnt.mode)
    {
    SHELL_CASE04(0, collapseBW<kObjects, BlendMode(kLabel)>(backgrounds));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool kObjects, BlendMode kBlendMode>
void Ppu::collapseBW(const BackgroundLayers& backgrounds)
{
    switch (possibleWindows<kObjects>())
    {
    SHELL_CASE08(0, collapseBW<kObjects, kBlendMode, kLabel>(backgrounds));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool kObjects, BlendMode kBlendMode, uint Windows>
void Ppu::collapseBW(const BackgroundLayers& backgrounds)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        u16 upper = 0;
        u16 lower = 0;

        const auto& object = objects[x];
        const auto& window = activeWindow<Windows>(x);

        if (kObjects && object.alpha)
        {
            if (findBlendLayers<kObjects>(backgrounds, x, window.layers, upper, lower))
                upper = bldalpha.blendAlpha(upper, lower);
        }
        else if (window.blend)
        {
            switch (kBlendMode)
            {
            case BlendMode::Alpha:
                if (findBlendLayers<kObjects>(backgrounds, x, window.layers, upper, lower))
                    upper = bldalpha.blendAlpha(upper, lower);
                break;

            case BlendMode::White:
                if (findBlendLayers<kObjects>(backgrounds, x, window.layers, upper))
                    upper = bldfade.blendWhite(upper);
                break;

            case BlendMode::Black:
                if (findBlendLayers<kObjects>(backgrounds, x, window.layers, upper))
                    upper = bldfade.blendBlack(upper);
                break;

            case BlendMode::Disabled:
                upper = upperLayer<kObjects>(backgrounds, x, window.layers);
                break;

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        else
        {
            upper = upperLayer<kObjects>(backgrounds, x, window.layers);
        }
        color = argb[upper];
    }
}

template<bool kObjects>
uint Ppu::possibleWindows() const
{
    uint windows = 0;

    if (dispcnt.win0 && winv[0].contains(vcount))
        windows |= Window::Flag::Zero;
    
    if (dispcnt.win1 && winv[1].contains(vcount))
        windows |= Window::Flag::One;
    
    if (dispcnt.winobj && kObjects)
        windows |= Window::Flag::Obj;

    return windows;
}

template<uint kWindows>
const Window& Ppu::activeWindow(uint x) const
{
    if ((kWindows & Window::Flag::Zero) && winh[0].contains(x))
        return winin.win0;

    if ((kWindows & Window::Flag::One) && winh[1].contains(x))
        return winin.win1;

    if ((kWindows & Window::Flag::Obj) && objects[x].window)
        return winout.winobj;

    return winout.winout;
}

template<bool kObjects>
u16 Ppu::upperLayer(const BackgroundLayers& backgrounds, uint x)
{
    const auto& object = objects[x];
    const auto  object_visible = kObjects && object.opaque();

    for (const auto& layer : backgrounds)
    {
        if (object_visible && object <= layer)
            return object.color;

        if (layer.opaque(x))
            return layer.color(x);
    }

    if (object_visible)
        return object.color;

    return pram.backdrop();
}

template<bool kObjects>
u16 Ppu::upperLayer(const BackgroundLayers& backgrounds, uint x, uint enabled)
{    
    const auto& object = objects[x];
    const auto  object_visible = kObjects && (enabled & Layer::Flag::Obj) && object.opaque();

    for (const auto& background : backgrounds)
    {
        if (object_visible && object <= background)
            return object.color;

        if ((enabled & background.flag) && background.opaque(x))
            return background.color(x);
    }
    
    if (object_visible)
        return object.color;

    return pram.backdrop();
}

template<bool kObjects>
bool Ppu::findBlendLayers(const BackgroundLayers& backgrounds, uint x, uint enabled, u16& upper)
{
    const auto& object = objects[x];
    const auto  object_visible = kObjects && (enabled & Layer::Flag::Obj) && object.opaque();

    uint blend_upper = object.alpha ? uint(Layer::Flag::Obj) : bldcnt.upper;

    for (const auto& background : backgrounds)
    {
        if (object_visible && object <= background)
        {
            upper = object.color;
            return blend_upper & Layer::Flag::Obj;
        }
        
        if ((enabled & background.flag) && background.opaque(x))
        {
            upper = background.color(x);
            return blend_upper & background.flag;
        }
    }
    
    if (object_visible)
    {
        upper = object.color;
        return blend_upper & Layer::Flag::Obj;
    }
    
    upper = pram.backdrop();
    return blend_upper & Layer::Flag::Bdp;
}

template<bool kObjects>
bool Ppu::findBlendLayers(const BackgroundLayers& backgrounds, uint x, uint enabled, u16& upper, u16& lower)
{
    #define PROCESS_LAYER(color, flag)      \
        if (upper_found)                    \
        {                                   \
            lower = color;                  \
            return blend_lower & flag;      \
        }                                   \
        else                                \
        {                                   \
            upper = color;                  \
            upper_found = true;             \
            if ((blend_upper & flag) == 0)  \
                return false;               \
        }

    const auto& object = objects[x];
    const auto  object_visible = kObjects && (enabled & Layer::Flag::Obj) && object.opaque();

    uint blend_upper = object.alpha ? uint(Layer::Flag::Obj) : bldcnt.upper;
    uint blend_lower = bldcnt.lower;

    bool upper_found = false;
    bool object_used = false;

    for (const auto& background : backgrounds)
    {
        if (object_visible && !object_used && object <= background)
        {
            PROCESS_LAYER(object.color, Layer::Flag::Obj);
            object_used = true;
        }
        
        if ((enabled & background.flag) && background.opaque(x))
            PROCESS_LAYER(background.color(x), background.flag);
    }
    
    if (object_visible && !object_used)
        PROCESS_LAYER(object.color, Layer::Flag::Obj);
    
    PROCESS_LAYER(pram.backdrop(), Layer::Flag::Bdp);
    
    return false;

    #undef PROCESS_LAYER
}
