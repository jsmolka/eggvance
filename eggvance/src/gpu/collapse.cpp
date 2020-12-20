#include "gpu.h"

#include <algorithm>

#include "constants.h"
#include "base/macros.h"
#include "core/videocontext.h"

void Gpu::collapse(uint bgs)
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
void Gpu::collapse(const BgLayers& layers)
{
    uint window = dispcnt.win0 || dispcnt.win1 || dispcnt.winobj;
    uint blend  = bldcnt.mode != kBlendModeDisabled || objects_alpha;

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
void Gpu::collapseNN(const BgLayers& layers)
{
    u32* scanline = video_ctx.scanline(vcount.value);

    for (uint x = 0; x < kScreen.x; ++x)
    {
        scanline[x] = argb[upperLayer<Objects>(layers, x)];
    }
}

template<bool Objects>
void Gpu::collapseNW(const BgLayers& layers)
{
    switch (possibleWindows<Objects>())
    {
    INDEXED_CASE8(0, collapseNW<Objects, kLabel>(layers));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool Objects, uint Windows>
void Gpu::collapseNW(const BgLayers& layers)
{
    u32* scanline = video_ctx.scanline(vcount.value);

    for (uint x = 0; x < kScreen.x; ++x)
    {
        const auto& window = activeWindow<Windows>(x);

        scanline[x] = argb[upperLayer<Objects>(layers, x, window.flags)];
    }
}

template<bool Objects>
void Gpu::collapseBN(const BgLayers& layers)
{
    switch (bldcnt.mode)
    {
    INDEXED_CASE4(0, collapseBN<Objects, kLabel>(layers));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool Objects, uint BlendMode>
void Gpu::collapseBN(const BgLayers& layers)
{
    constexpr uint kFlags = 0xFFFF;

    u32* scanline = video_ctx.scanline(vcount.value);

    for (uint x = 0; x < kScreen.x; ++x)
    {
        u16 upper = 0;
        u16 lower = 0;

        const auto& object = objects[x];

        if (Objects && object.alpha && findBlendLayers<Objects>(layers, x, kFlags, upper, lower))
        {
            upper = bldalpha.blendAlpha(upper, lower);
        }
        else
        {
            switch (BlendMode)
            {
            case kBlendModeAlpha:
                if (findBlendLayers<Objects>(layers, x, kFlags, upper, lower))
                    upper = bldalpha.blendAlpha(upper, lower);
                break;

            case kBlendModeWhite:
                if (findBlendLayers<Objects>(layers, x, kFlags, upper))
                    upper = bldfade.blendWhite(upper);
                break;

            case kBlendModeBlack:
                if (findBlendLayers<Objects>(layers, x, kFlags, upper))
                    upper = bldfade.blendBlack(upper);
                break;

            case kBlendModeDisabled:
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
void Gpu::collapseBW(const BgLayers& layers)
{
    switch (bldcnt.mode)
    {
    INDEXED_CASE4(0, collapseBW<Objects, kLabel>(layers));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool Objects, uint BlendMode>
void Gpu::collapseBW(const BgLayers& layers)
{
    switch (possibleWindows<Objects>())
    {
    INDEXED_CASE8(0, collapseBW<Objects, BlendMode, kLabel>(layers));

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<bool Objects, uint BlendMode, uint Windows>
void Gpu::collapseBW(const BgLayers& layers)
{
    u32* scanline = video_ctx.scanline(vcount.value);

    for (uint x = 0; x < kScreen.x; ++x)
    {
        u16 upper = 0;
        u16 lower = 0;

        const auto& object = objects[x];
        const auto& window = activeWindow<Windows>(x);

        if (Objects && object.alpha && findBlendLayers<Objects>(layers, x, window.flags, upper, lower))
        {
            upper = bldalpha.blendAlpha(upper, lower);
        }
        else if (window.blend)
        {
            switch (BlendMode)
            {
            case kBlendModeAlpha:
                if (findBlendLayers<Objects>(layers, x, window.flags, upper, lower))
                    upper = bldalpha.blendAlpha(upper, lower);
                break;

            case kBlendModeWhite:
                if (findBlendLayers<Objects>(layers, x, window.flags, upper))
                    upper = bldfade.blendWhite(upper);
                break;

            case kBlendModeBlack:
                if (findBlendLayers<Objects>(layers, x, window.flags, upper))
                    upper = bldfade.blendBlack(upper);
                break;

            case kBlendModeDisabled:
                upper = upperLayer<Objects>(layers, x, window.flags);
                break;

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        else if (!(Objects && object.alpha))
        {
            upper = upperLayer<Objects>(layers, x, window.flags);
        }
        scanline[x] = argb[upper];
    }
}

template<bool Objects>
uint Gpu::possibleWindows() const
{
    uint windows = 0;

    if (dispcnt.win0 && winv[0].contains(vcount.value))
        windows |= kWindow0;
    if (dispcnt.win1 && winv[1].contains(vcount.value))
        windows |= kWindow1;
    if (dispcnt.winobj && Objects)
        windows |= kWindowObj;

    return windows;
}

template<uint Windows>
const Window& Gpu::activeWindow(uint x) const
{
    if (Windows & kWindow0 && winh[0].contains(x))
        return winin.win0;

    if (Windows & kWindow1 && winh[1].contains(x))
        return winin.win1;

    if (Windows & kWindowObj && objects[x].window)
        return winout.winobj;

    return winout.winout;
}

template<bool Objects>
u16 Gpu::upperLayer(const BgLayers& layers, uint x)
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
u16 Gpu::upperLayer(const BgLayers& layers, uint x, uint flags)
{    
    const auto& object = objects[x];

    for (const auto& layer : layers)
    {
        if (Objects && flags & kLayerObj && object.opaque() && object <= layer)
            return object.color;

        if (flags & layer.flag && layer.opaque(x))
            return layer.color(x);
    }
    
    if (Objects && flags & kLayerObj && object.opaque())
        return object.color;

    return pram.backdrop();
}

template<bool Objects>
bool Gpu::findBlendLayers(const BgLayers& layers, uint x, uint flags, u16& upper)
{
    const auto& object = objects[x];

    uint flags_upper = object.alpha ? kLayerObj : bldcnt.upper;

    for (const auto& layer : layers)
    {
        if (Objects && flags & kLayerObj && object.opaque() && object <= layer)
        {
            upper = object.color;
            return flags_upper & kLayerObj;
        }
        
        if (flags & layer.flag && layer.opaque(x))
        {
            upper = layer.color(x);
            return flags_upper & layer.flag;
        }
    }
    
    if (Objects && flags & kLayerObj && object.opaque())
    {
        upper = object.color;
        return flags_upper & kLayerObj;
    }
    
    upper = pram.backdrop();
    return flags_upper & kLayerBdp;
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
bool Gpu::findBlendLayers(const BgLayers& layers, uint x, uint flags, u16& upper, u16& lower)
{
    const auto& object = objects[x];

    uint flags_upper = object.alpha ? kLayerObj : bldcnt.upper;
    uint flags_lower = bldcnt.lower;

    bool upper_found = false;
    bool object_used = false;

    for (const auto& layer : layers)
    {
        if (Objects && flags & kLayerObj && !object_used && object.opaque() && object <= layer)
        {
            PROCESS_LAYER(object.color, kLayerObj);
            object_used = true;
        }
        
        if (flags & layer.flag && layer.opaque(x))
            PROCESS_LAYER(layer.color(x), layer.flag);
    }
    
    if (Objects && flags & kLayerObj && !object_used && object.opaque())
        PROCESS_LAYER(object.color, kLayerObj);
    
    if (upper_found)
    {
        lower = pram.backdrop();
        return flags_lower & kLayerBdp;
    }
    else
    {
        upper = pram.backdrop();
        return false;
    }
}

#undef PROCESS_LAYER
