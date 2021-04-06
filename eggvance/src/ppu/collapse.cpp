#include "ppu.h"

#include <algorithm>
#include <shell/macros.h>
#include <shell/operators.h>

#include "constants.h"
#include "frontend/videocontext.h"

void Ppu::collapse(uint bgs)
{
    BgLayers layers;

    for (uint bg : bit::iterate(bgs & dispcnt.layers))
    {
        layers.push_back({
            bgcnt[bg].priority,
            backgrounds[bg].data(),
            1U << bg
        });
    }

    std::sort(layers.begin(), layers.end());

    if (objects_exist)
        collapse<1>(layers);
    else
        collapse<0>(layers);
}

template<bool Objects>
void Ppu::collapse(const BgLayers& layers)
{
    uint window = dispcnt.win0 || dispcnt.win1 || dispcnt.winobj;
    uint blend  = bldcnt.mode != BlendMode::Disabled || objects_alpha;

    switch ((blend << 1) | window)
    {
    case 0b00: collapseNN<Objects>(layers); break;
    case 0b01: collapseNW<Objects>(layers); break;
    case 0b10: collapseBN<Objects>(layers); break;
    case 0b11: collapseBW<Objects>(layers); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool Objects>
void Ppu::collapseNN(const BgLayers& layers)
{
    auto& scanline = video_ctx.scanline(vcount);

    for (uint x = 0; x < kScreen.x; ++x)
    {
        scanline[x] = argb[upperLayer<Objects>(layers, x)];
    }
}

template<bool Objects>
void Ppu::collapseNW(const BgLayers& layers)
{
    switch (possibleWindows<Objects>())
    {
    SHELL_CASE08(0, collapseNW<Objects, kLabel>(layers));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool Objects, uint Windows>
void Ppu::collapseNW(const BgLayers& layers)
{
    auto& scanline = video_ctx.scanline(vcount);

    for (uint x = 0; x < kScreen.x; ++x)
    {
        const auto& window = activeWindow<Windows>(x);

        scanline[x] = argb[upperLayer<Objects>(layers, x, window.flags)];
    }
}

template<bool Objects>
void Ppu::collapseBN(const BgLayers& layers)
{
    switch (bldcnt.mode)
    {
    SHELL_CASE04(0, collapseBN<Objects, BlendMode(kLabel)>(layers));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool Objects, BlendMode kBlendMode>
void Ppu::collapseBN(const BgLayers& layers)
{
    constexpr uint kFlags = 0xFFFF;

    auto& scanline = video_ctx.scanline(vcount);

    for (uint x = 0; x < kScreen.x; ++x)
    {
        u16 upper = 0;
        u16 lower = 0;

        const auto& object = objects[x];

        if (Objects && object.alpha)
        {
            if (findBlendLayers<Objects>(layers, x, kFlags, upper, lower))
                upper = bldalpha.blendAlpha(upper, lower);
        }
        else
        {
            switch (kBlendMode)
            {
            case BlendMode::Alpha:
                if (findBlendLayers<Objects>(layers, x, kFlags, upper, lower))
                    upper = bldalpha.blendAlpha(upper, lower);
                break;

            case BlendMode::White:
                if (findBlendLayers<Objects>(layers, x, kFlags, upper))
                    upper = bldfade.blendWhite(upper);
                break;

            case BlendMode::Black:
                if (findBlendLayers<Objects>(layers, x, kFlags, upper))
                    upper = bldfade.blendBlack(upper);
                break;

            case BlendMode::Disabled:
                upper = upperLayer<Objects>(layers, x);
                break;

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        scanline[x] = argb[upper];
    }
}

template<bool Objects>
void Ppu::collapseBW(const BgLayers& layers)
{
    switch (bldcnt.mode)
    {
    SHELL_CASE04(0, collapseBW<Objects, BlendMode(kLabel)>(layers));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool Objects, BlendMode kBlendMode>
void Ppu::collapseBW(const BgLayers& layers)
{
    switch (possibleWindows<Objects>())
    {
    SHELL_CASE08(0, collapseBW<Objects, kBlendMode, kLabel>(layers));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool Objects, BlendMode kBlendMode, uint Windows>
void Ppu::collapseBW(const BgLayers& layers)
{
    auto& scanline = video_ctx.scanline(vcount);

    for (uint x = 0; x < kScreen.x; ++x)
    {
        u16 upper = 0;
        u16 lower = 0;

        const auto& object = objects[x];
        const auto& window = activeWindow<Windows>(x);

        if (Objects && object.alpha)
        {
            if (findBlendLayers<Objects>(layers, x, window.flags, upper, lower))
                upper = bldalpha.blendAlpha(upper, lower);
        }
        else if (window.blend)
        {
            switch (kBlendMode)
            {
            case BlendMode::Alpha:
                if (findBlendLayers<Objects>(layers, x, window.flags, upper, lower))
                    upper = bldalpha.blendAlpha(upper, lower);
                break;

            case BlendMode::White:
                if (findBlendLayers<Objects>(layers, x, window.flags, upper))
                    upper = bldfade.blendWhite(upper);
                break;

            case BlendMode::Black:
                if (findBlendLayers<Objects>(layers, x, window.flags, upper))
                    upper = bldfade.blendBlack(upper);
                break;

            case BlendMode::Disabled:
                upper = upperLayer<Objects>(layers, x, window.flags);
                break;

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        else
        {
            upper = upperLayer<Objects>(layers, x, window.flags);
        }
        scanline[x] = argb[upper];
    }
}

template<bool Objects>
uint Ppu::possibleWindows() const
{
    uint windows = 0;

    if (dispcnt.win0 && winv[0].contains(vcount))
        windows |= Window::Flag::Zero;
    if (dispcnt.win1 && winv[1].contains(vcount))
        windows |= Window::Flag::One;
    if (dispcnt.winobj && Objects)
        windows |= Window::Flag::Obj;

    return windows;
}

template<uint Windows>
const Window& Ppu::activeWindow(uint x) const
{
    if (Windows & Window::Flag::Zero && winh[0].contains(x))
        return winin.win0;

    if (Windows & Window::Flag::One && winh[1].contains(x))
        return winin.win1;

    if (Windows & Window::Flag::Obj && objects[x].window)
        return winout.winobj;

    return winout.winout;
}

template<bool Objects>
u16 Ppu::upperLayer(const BgLayers& layers, uint x)
{
    const auto& object = objects[x];

    for (const auto& layer : layers)
    {
        if (Objects && object.opaque() && object <= layer)
            return object.color;

        if (layer.opaque(x))
            return layer.color(x);
    }

    if (Objects && object.opaque())
        return object.color;

    return pram.backdrop();
}

template<bool Objects>
u16 Ppu::upperLayer(const BgLayers& layers, uint x, uint flags)
{    
    const auto& object = objects[x];

    for (const auto& layer : layers)
    {
        if (Objects && flags & Layer::Flag::Obj && object.opaque() && object <= layer)
            return object.color;

        if (flags & layer.flag && layer.opaque(x))
            return layer.color(x);
    }
    
    if (Objects && flags & Layer::Flag::Obj && object.opaque())
        return object.color;

    return pram.backdrop();
}

template<bool Objects>
bool Ppu::findBlendLayers(const BgLayers& layers, uint x, uint flags, u16& upper)
{
    const auto& object = objects[x];

    uint flags_upper = object.alpha ? uint(Layer::Flag::Obj) : bldcnt.upper;

    for (const auto& layer : layers)
    {
        if (Objects && flags & Layer::Flag::Obj && object.opaque() && object <= layer)
        {
            upper = object.color;
            return flags_upper & Layer::Flag::Obj;
        }
        
        if (flags & layer.flag && layer.opaque(x))
        {
            upper = layer.color(x);
            return flags_upper & layer.flag;
        }
    }
    
    if (Objects && flags & Layer::Flag::Obj && object.opaque())
    {
        upper = object.color;
        return flags_upper & Layer::Flag::Obj;
    }
    
    upper = pram.backdrop();
    return flags_upper & Layer::Flag::Bdp;
}

#define PROCESS_LAYER(color, flag)      \
    if (upper_found)                    \
    {                                   \
        lower = color;                  \
        return flags_lower & flag;      \
    }                                   \
    else                                \
    {                                   \
        upper = color;                  \
        upper_found = true;             \
        if ((flags_upper & flag) == 0)  \
            return false;               \
    }

template<bool Objects>
bool Ppu::findBlendLayers(const BgLayers& layers, uint x, uint flags, u16& upper, u16& lower)
{
    const auto& object = objects[x];

    uint flags_upper = object.alpha ? uint(Layer::Flag::Obj) : bldcnt.upper;
    uint flags_lower = bldcnt.lower;

    bool upper_found = false;
    bool object_used = false;

    for (const auto& layer : layers)
    {
        if (Objects && flags & Layer::Flag::Obj && !object_used && object.opaque() && object <= layer)
        {
            PROCESS_LAYER(object.color, Layer::Flag::Obj);
            object_used = true;
        }
        
        if (flags & layer.flag && layer.opaque(x))
            PROCESS_LAYER(layer.color(x), layer.flag);
    }
    
    if (Objects && flags & Layer::Flag::Obj && !object_used && object.opaque())
        PROCESS_LAYER(object.color, Layer::Flag::Obj);
    
    if (upper_found)
    {
        lower = pram.backdrop();
        return flags_lower & Layer::Flag::Bdp;
    }
    else
    {
        upper = pram.backdrop();
        return false;
    }
}

#undef PROCESS_LAYER
