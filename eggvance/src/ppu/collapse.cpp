#include "ppu.h"

#include <algorithm>
#include <shell/macros.h>
#include <shell/operators.h>

#include "color.h"
#include "frontend/videocontext.h"

void Ppu::collapse(uint bgs)
{
    BackgroundLayers layers;

    for (uint bg : bit::iterate(bgs & dispcnt.layers))
    {
        layers.push_back({ this->backgrounds[bg].control.priority, this->backgrounds[bg].buffer.data(), 1U << bg });
    }

    std::sort(layers.begin(), layers.end());

    if (dispcnt.win0 || dispcnt.win1 || dispcnt.winobj)
    {
        uint windows = 0;

        if (dispcnt.win0 && winv[0].contains(vcount))
            windows |= Window::Flag::Win0;

        if (dispcnt.win1 && winv[1].contains(vcount))
            windows |= Window::Flag::Win1;

        if (dispcnt.winobj && objects_exist)
            windows |= Window::Flag::WinObj;

        if (bldcnt.mode != BlendMode::Disabled || objects_alpha)
        {
            switch (static_cast<uint>(objects_exist) | (bldcnt.mode << 1) | (windows << 3))
            {
            SHELL_CASE64(0,
                collapseBW<
                    bit::seq<0, 1>(kLabel),
                    bit::seq<1, 2>(kLabel),
                    bit::seq<3, 3>(kLabel)>(layers))

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        else
        {
            switch (static_cast<uint>(objects_exist) | (windows << 1))
            {
            SHELL_CASE16(0,
                collapseNW<
                    bit::seq<0, 1>(kLabel),
                    bit::seq<1, 3>(kLabel)>(layers))

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
    }
    else
    {
        if (bldcnt.mode != BlendMode::Disabled || objects_alpha)
        {
            switch (static_cast<uint>(objects_exist) | (bldcnt.mode << 1))
            {
            SHELL_CASE08(0,
                collapseBN<
                    bit::seq<0, 1>(kLabel),
                    bit::seq<1, 2>(kLabel)>(layers))

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        else
        {
            switch (static_cast<uint>(objects_exist))
            {
            SHELL_CASE02(0,
                collapseNN<
                    bit::seq<0, 1>(kLabel)>(layers))

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
    }
}

template<bool kObjects>
void Ppu::collapseNN(const BackgroundLayers& backgrounds)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        color = Color::toArgb(findUpperLayer<kObjects>(backgrounds, x));
    }
}

template<bool kObjects, uint kWindows>
void Ppu::collapseNW(const BackgroundLayers& backgrounds)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        const auto& window = activeWindow<kWindows>(x);

        color = Color::toArgb(findUpperLayer<kObjects>(backgrounds, x, window.layers));
    }
}

template<bool kObjects, uint kBlendMode>
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
            if (findBlendLayer<kObjects>(backgrounds, x, kEnabled, upper, lower))
                upper = bldalpha.blendAlpha(upper, lower);
        }
        else
        {
            switch (BlendMode(kBlendMode))
            {
            case BlendMode::Alpha:
                if (findBlendLayer<kObjects>(backgrounds, x, kEnabled, upper, lower))
                    upper = bldalpha.blendAlpha(upper, lower);
                break;

            case BlendMode::White:
                if (findBlendLayer<kObjects>(backgrounds, x, kEnabled, upper))
                    upper = bldfade.blendWhite(upper);
                break;

            case BlendMode::Black:
                if (findBlendLayer<kObjects>(backgrounds, x, kEnabled, upper))
                    upper = bldfade.blendBlack(upper);
                break;

            case BlendMode::Disabled:
                upper = findUpperLayer<kObjects>(backgrounds, x);
                break;

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        color = Color::toArgb(upper);
    }
}

template<bool kObjects, uint kBlendMode, uint kWindows>
void Ppu::collapseBW(const BackgroundLayers& backgrounds)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        u16 upper = 0;
        u16 lower = 0;

        const auto& object = objects[x];
        const auto& window = activeWindow<kWindows>(x);

        if (kObjects && object.alpha)
        {
            if (findBlendLayer<kObjects>(backgrounds, x, window.layers, upper, lower))
                upper = bldalpha.blendAlpha(upper, lower);
        }
        else if (window.blend)
        {
            switch (BlendMode(kBlendMode))
            {
            case BlendMode::Alpha:
                if (findBlendLayer<kObjects>(backgrounds, x, window.layers, upper, lower))
                    upper = bldalpha.blendAlpha(upper, lower);
                break;

            case BlendMode::White:
                if (findBlendLayer<kObjects>(backgrounds, x, window.layers, upper))
                    upper = bldfade.blendWhite(upper);
                break;

            case BlendMode::Black:
                if (findBlendLayer<kObjects>(backgrounds, x, window.layers, upper))
                    upper = bldfade.blendBlack(upper);
                break;

            case BlendMode::Disabled:
                upper = findUpperLayer<kObjects>(backgrounds, x, window.layers);
                break;

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        else
        {
            upper = findUpperLayer<kObjects>(backgrounds, x, window.layers);
        }
        color = Color::toArgb(upper);
    }
}

template<uint kWindows>
const Window& Ppu::activeWindow(uint x) const
{
    if ((kWindows & Window::Flag::Win0) && winh[0].contains(x))
        return winin.win0;

    if ((kWindows & Window::Flag::Win1) && winh[1].contains(x))
        return winin.win1;

    if ((kWindows & Window::Flag::WinObj) && objects[x].window)
        return winout.winobj;

    return winout.winout;
}

template<bool kObjects>
u16 Ppu::findUpperLayer(const BackgroundLayers& layers, uint x)
{
    const auto& object = objects[x];
    const auto  object_visible = kObjects && object.opaque();

    for (const auto& layer : layers)
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
u16 Ppu::findUpperLayer(const BackgroundLayers& layers, uint x, uint enabled)
{    
    const auto& object = objects[x];
    const auto  object_visible = kObjects && (enabled & Layer::Flag::Obj) && object.opaque();

    for (const auto& layer : layers)
    {
        if (object_visible && object <= layer)
            return object.color;

        if ((enabled & layer.flag) && layer.opaque(x))
            return layer.color(x);
    }
    
    if (object_visible)
        return object.color;

    return pram.backdrop();
}

template<bool kObjects>
bool Ppu::findBlendLayer(const BackgroundLayers& layers, uint x, uint enabled, u16& upper)
{
    const auto& object = objects[x];
    const auto  object_visible = kObjects && (enabled & Layer::Flag::Obj) && object.opaque();

    uint blend_upper = object.alpha ? uint(Layer::Flag::Obj) : bldcnt.upper;

    for (const auto& layer : layers)
    {
        if (object_visible && object <= layer)
        {
            upper = object.color;
            return blend_upper & Layer::Flag::Obj;
        }
        
        if ((enabled & layer.flag) && layer.opaque(x))
        {
            upper = layer.color(x);
            return blend_upper & layer.flag;
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
bool Ppu::findBlendLayer(const BackgroundLayers& layers, uint x, uint enabled, u16& upper, u16& lower)
{
    const auto& object = objects[x];
    const auto  object_visible = kObjects && (enabled & Layer::Flag::Obj) && object.opaque();

    uint blend_upper = object.alpha ? uint(Layer::Flag::Obj) : bldcnt.upper;
    uint blend_lower = bldcnt.lower;

    bool upper_found = false;
    bool object_used = false;

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

    for (const auto& layer : layers)
    {
        if (object_visible && !object_used && object <= layer)
        {
            PROCESS_LAYER(object.color, Layer::Flag::Obj);
            object_used = true;
        }
        
        if ((enabled & layer.flag) && layer.opaque(x))
            PROCESS_LAYER(layer.color(x), layer.flag);
    }
    
    if (object_visible && !object_used)
        PROCESS_LAYER(object.color, Layer::Flag::Obj);
    
    PROCESS_LAYER(pram.backdrop(), Layer::Flag::Bdp);

    #undef PROCESS_LAYER
    
    return false;
}
