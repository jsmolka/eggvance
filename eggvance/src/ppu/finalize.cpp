#include "ppu.h"

int f = 0;
#include <fmt/printf.h>

void PPU::finalize(int begin, int end)
{
    int w = mmio.dispcnt.win0 || mmio.dispcnt.win1 || mmio.dispcnt.winobj;
    int e = mmio.bldcnt.mode != BLD_DISABLED || obj_alpha;

    Layers layers;
    for (int bg = begin; bg < end; ++bg)
    {
        if (mmio.dispcnt.bg[bg])
        {
            auto layer = std::make_shared<BackgroundLayer>(&bgs[bg][0], mmio.bgcnt[bg].priority, 1 << bg);
            layers.push_back(layer);
        }
    }
    layers.sort();
    layers.pushBackdrop(std::make_shared<BackdropLayer>(mmu.palette.readHalf(0)));

    if (obj_exist && mmio.dispcnt.obj)
    {
        layers.pushObjects(std::make_shared<ObjectLayer>(&obj[0]));
        layers.resort();
    }

    switch ((e << 1) | (w << 0))
    {
    case 0b00: 
        finalize__(layers); 
        //fmt::printf("__");
        break;
    case 0b01: 
        finalize_W(layers); 
        //fmt::printf("_W");
        break;
    case 0b10: 
        finalizeB_(layers); 
        //fmt::printf("B_");
        break;
    case 0b11: 
        finalizeBW(layers); 
        //fmt::printf("BW");
        break;
    }
}

void PPU::finalize__(Layers& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        bool set = false;
        for (auto& layer : layers)
        {
            if (!set && layer->opaque())
            {
                scanline[x] = layer->color;
                set = true;
            }
            layer->next();
        }
        layers.resort();
    }
}

void PPU::finalize_W(Layers& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    bool win0 = mmio.winv[0].contains(mmio.vcount);
    bool win1 = mmio.winv[1].contains(mmio.vcount);

    const auto& winh   = mmio.winh;
    const auto& winin  = mmio.winin;
    const auto& winout = mmio.winout;

    for (int x = 0; x < WIDTH; ++x)
    {
        int flags;
        if (win0 && winh[0].contains(x))
        {
            flags = winin.win0.flags;
        }
        else if (win1 && winh[1].contains(x))
        {
            flags = winin.win1.flags;
        }
        else if (mmio.dispcnt.winobj && obj[x].window)
        {
            flags = winout.winobj.flags;
        }
        else
        {
            flags = winout.winout.flags;
        }

        bool set = false;
        for (auto& layer : layers)
        {
            if (!set && layer->opaque() && layer->matches(flags))
            {
                scanline[x] = layer->color;
                set = true;
            }
            layer->next();
        }
        layers.resort();
    }
}

void PPU::finalizeB_(Layers& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        for (auto& layer : layers)
        {
            if (layer->opaque())
            {
                scanline[x] = layer->color;
                break;
            }
        }

        int upper = COLOR_T;
        int lower = COLOR_T;

        bool blended = false;
        if (obj[x].mode == GFX_ALPHA)
        {
            if (blended = getBlendLayers(x, 0xFFFF, layers, upper, lower))
            {
                scanline[x] = blendAlpha(upper, lower);
            }
            else
            {
                upper = COLOR_T;
                lower = COLOR_T;
            }
        }
        if (!blended)
        {
            switch (mmio.bldcnt.mode)
            {
            case BLD_ALPHA:
                if (getBlendLayers(x, 0xFFFF, layers, upper, lower))
                    scanline[x] = blendAlpha(upper, lower);
                break;

            case BLD_WHITE:
                if (getBlendLayers(x, 0xFFFF, layers, upper))
                    scanline[x] = blendWhite(upper);
                break;

            case BLD_BLACK:
                if (getBlendLayers(x, 0xFFFF, layers, upper))
                    scanline[x] = blendBlack(upper);
                break;
            }
        }

        for (auto& layer : layers)
            layer->next();

        layers.resort();
    }
}

void PPU::finalizeBW(Layers& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    bool win0 = mmio.winv[0].contains(mmio.vcount);
    bool win1 = mmio.winv[1].contains(mmio.vcount);

    const auto& winh   = mmio.winh;
    const auto& winin  = mmio.winin;
    const auto& winout = mmio.winout;

    for (int x = 0; x < WIDTH; ++x)
    {
        for (auto& layer : layers)
        {
            if (layer->opaque())
            {
                scanline[x] = layer->color;
                break;
            }
        }

        int flags;
        int effects;
        if (win0 && winh[0].contains(x))
        {
            flags = winin.win0.flags;
            effects = winin.win0.sfx;
        }
        else if (win1 && winh[1].contains(x))
        {
            flags = winin.win1.flags;
            effects = winin.win1.sfx;
        }
        else if (mmio.dispcnt.winobj && obj[x].window)
        {
            flags = winout.winobj.flags;
            effects = winout.winobj.sfx;
        }
        else
        {
            flags = winout.winout.flags;
            effects = winout.winout.sfx;
        }

        int upper = COLOR_T;
        int lower = COLOR_T;

        bool blended = false;
        if (obj[x].mode == GFX_ALPHA)
        {
            if (blended = getBlendLayers(x, flags, layers, upper, lower))
            {
                scanline[x] = blendAlpha(upper, lower);
            }
            else
            {
                upper = COLOR_T;
                lower = COLOR_T;
            }
        }
        if (effects)
        {
            if (!blended)
            {
                switch (mmio.bldcnt.mode)
                {
                case BLD_ALPHA:
                    if (getBlendLayers(x, flags, layers, upper, lower))
                        scanline[x] = blendAlpha(upper, lower);
                    break;

                case BLD_WHITE:
                    if (getBlendLayers(x, flags, layers, upper))
                        scanline[x] = blendWhite(upper);
                    break;

                case BLD_BLACK:
                    if (getBlendLayers(x, flags, layers, upper))
                        scanline[x] = blendBlack(upper);
                    break;
                }
            }
        }

        for (auto& layer : layers)
            layer->next();

        layers.resort();
    }
}

bool PPU::getBlendLayers(int x, int win_flags, Layers& layers, int& upper)
{
    if (obj[x].mode == GFX_ALPHA)
    {
        for (auto& layer : layers)
        {
            if (layer->opaque() && layer->flag == LF_OBJ)
            {
                upper = layer->color;
                return true;
            }
        }
    }
    else
    {
        int upper_flags = mmio.bldcnt.upper.flags & win_flags;

        for (auto& layer : layers)
        {
            if (layer->opaque())
            {
                if (layer->flag & upper_flags)
                {
                    upper = layer->color;
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return false;
}

bool PPU::getBlendLayers(int x, int win_flags, Layers& layers, int& upper, int& lower)
{
    int upper_flags = obj[x].mode == GFX_ALPHA ? LF_OBJ : (mmio.bldcnt.upper.flags & win_flags);
    int lower_flags = mmio.bldcnt.lower.flags & win_flags;

    bool upper_found = false;

    for (auto& layer : layers)
    {
        if (layer->opaque())
        {
            if (!upper_found && layer->flag & upper_flags)
            {
                upper = layer->color;
                upper_found = true;
            }
            else if (layer->flag & lower_flags)
            {
                lower = layer->color;
                return upper_found;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}
