#include "ppu.h"

#include <shell/macros.h>
#include <shell/operators.h>

#include "color.h"
#include "base/config.h"
#include "frontend/videocontext.h"

void Ppu::compose(uint possible)
{
    BackgroundLayers layers;

    for (uint background : bit::iterate(possible & dispcnt.enabled & config.video_layers))
    {
        layers.push_back({
            backgrounds[background].control.priority,
            backgrounds[background].buffer.data(),
            backgrounds[background].flag() });
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
            switch (objects_exist | (bldcnt.mode << 1) | (windows << 3))
            {
            SHELL_CASE64(0,
                composeBW<
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
            switch (objects_exist | (windows << 1))
            {
            SHELL_CASE16(0,
                composeNW<
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
            switch (objects_exist | (bldcnt.mode << 1))
            {
            SHELL_CASE08(0,
                composeBN<
                    bit::seq<0, 1>(kLabel),
                    bit::seq<1, 2>(kLabel)>(layers))

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        else
        {
            switch (objects_exist)
            {
            SHELL_CASE02(0,
                composeNN<
                    bit::seq<0, 1>(kLabel)>(layers))

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
    }
}

template<bool kObjects>
void Ppu::composeNN(const BackgroundLayers& layers)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        color = Color::toArgb(findUpperLayer<kObjects>(layers, x, 0xFFFF'FFFF).color);
    }
}

template<bool kObjects, uint kWindows>
void Ppu::composeNW(const BackgroundLayers& layers)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        const auto& window = activeWindow<kWindows>(x);

        color = Color::toArgb(findUpperLayer<kObjects>(layers, x, window.enabled).color);
    }
}

template<bool kObjects, uint kBlendMode>
void Ppu::composeBN(const BackgroundLayers& layers)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        ComposeLayer upper;
        ComposeLayer lower;

        bool alpha_object = kObjects && objects[x].alpha;
        if ( alpha_object)
        {
            std::tie(upper, lower) = findUpperLayers<kObjects>(layers, x);

            if (upper.flag == Layer::Flag::Obj && (bldcnt.lower & lower.flag))
            {
                color = Color::toArgb(bldalpha.blendAlpha(upper.color, lower.color));
                continue;
            }
        }

        switch (BlendMode(kBlendMode))
        {
        case BlendMode::Alpha:
            if (!alpha_object)
                std::tie(upper, lower) = findUpperLayers<kObjects>(layers, x);

            if ((bldcnt.upper & upper.flag) && (bldcnt.lower & lower.flag))
                upper.color = bldalpha.blendAlpha(upper.color, lower.color);
            break;

        case BlendMode::White:
            if (!alpha_object)
                upper = findUpperLayer<kObjects>(layers, x);

            if (bldcnt.upper & upper.flag)
                upper.color = bldfade.blendWhite(upper.color);
            break;

        case BlendMode::Black:
            if (!alpha_object)
                upper = findUpperLayer<kObjects>(layers, x);

            if (bldcnt.upper & upper.flag)
                upper.color = bldfade.blendBlack(upper.color);
            break;

        case BlendMode::Disabled:
            if (!alpha_object)
                upper = findUpperLayer<kObjects>(layers, x);
            break;

        default:
            SHELL_UNREACHABLE;
            break;
        }
        color = Color::toArgb(upper.color);
    }
}

template<bool kObjects, uint kBlendMode, uint kWindows>
void Ppu::composeBW(const BackgroundLayers& layers)
{
    for (auto [x, color] : shell::enumerate(video_ctx.scanline(vcount)))
    {
        const auto& window = activeWindow<kWindows>(x);

        ComposeLayer upper;
        ComposeLayer lower;

        bool alpha_object = kObjects && objects[x].alpha;
        if ( alpha_object)
        {
            std::tie(upper, lower) = findUpperLayers<kObjects>(layers, x, window.enabled);

            if (upper.flag == Layer::Flag::Obj && (bldcnt.lower & lower.flag))
            {
                color = Color::toArgb(bldalpha.blendAlpha(upper.color, lower.color));
                continue;
            }
        }

        if (window.blend)
        {
            switch (BlendMode(kBlendMode))
            {
            case BlendMode::Alpha:
                if (!alpha_object)
                    std::tie(upper, lower) = findUpperLayers<kObjects>(layers, x, window.enabled);

                if ((bldcnt.upper & upper.flag) && (bldcnt.lower & lower.flag))
                    upper.color = bldalpha.blendAlpha(upper.color, lower.color);
                break;

            case BlendMode::White:
                if (!alpha_object)
                    upper = findUpperLayer<kObjects>(layers, x, window.enabled);

                if (bldcnt.upper & upper.flag)
                    upper.color = bldfade.blendWhite(upper.color);
                break;

            case BlendMode::Black:
                if (!alpha_object)
                    upper = findUpperLayer<kObjects>(layers, x, window.enabled);

                if (bldcnt.upper & upper.flag)
                    upper.color = bldfade.blendBlack(upper.color);
                break;

            case BlendMode::Disabled:
                if (!alpha_object)
                    upper = findUpperLayer<kObjects>(layers, x, window.enabled);
                break;

            default:
                SHELL_UNREACHABLE;
                break;
            }
        }
        else if (!alpha_object)
        {
            upper = findUpperLayer<kObjects>(layers, x, window.enabled);
        }
        color = Color::toArgb(upper.color);
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
Ppu::ComposeLayer Ppu::findUpperLayer(const BackgroundLayers& layers, uint x, uint enabled)
{
    auto& object = objects[x];
    auto  object_visible = kObjects && (enabled & Layer::Flag::Obj) && object.isOpaque();

    for (const auto& layer : layers)
    {
        if (object_visible && object <= layer)
            return ComposeLayer(Layer::Flag::Obj, object.color);

        if ((enabled & layer.flag) && layer.isOpaque(x))
            return ComposeLayer(layer.flag, layer.color(x));
    }

    if (object_visible)
        return ComposeLayer(Layer::Flag::Obj, object.color);

    return ComposeLayer(Layer::Flag::Bdp, pram.backdrop());
}

template<bool kObjects>
Ppu::ComposeLayers Ppu::findUpperLayers(const BackgroundLayers& layers, uint x, uint enabled)
{
    auto& object = objects[x];
    auto  object_used = false;
    auto  object_visible = kObjects && (enabled & Layer::Flag::Obj) && object.isOpaque();

    std::optional<ComposeLayer> upper;

    for (const auto& layer : layers)
    {
        if (object_visible && !object_used && object <= layer)
        {
            if (upper)
                return ComposeLayers(*upper, ComposeLayer(Layer::Flag::Obj, object.color));
            else
                upper = ComposeLayer(Layer::Flag::Obj, object.color);

            object_used = true;
        }

        if ((enabled & layer.flag) && layer.isOpaque(x))
        {
            if (upper)
                return ComposeLayers(*upper, ComposeLayer(layer.flag, layer.color(x)));
            else
                upper = ComposeLayer(layer.flag, layer.color(x));
        }
    }

    if (object_visible && !object_used)
    {
        if (upper)
            return ComposeLayers(*upper, ComposeLayer(Layer::Flag::Obj, object.color));
        else
            upper = ComposeLayer(Layer::Flag::Obj, object.color);
    }

    if (!upper)
        upper = ComposeLayer(Layer::Flag::Bdp, pram.backdrop());

    return ComposeLayers(*upper, ComposeLayer(Layer::Flag::Bdp, pram.backdrop()));
}
