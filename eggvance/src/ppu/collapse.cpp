#include "ppu.h"

// !!! Make sure that objects won't be reused

/**
 * Todo: All the object could be removed here. Templating all the other
 * collapse functions (with obj_exist) could have a positive performance
 * impact.
 * Todo: Call labels 'draw' and assign pixel there (in preparation for ARGB8888)
 */
void PPU::collapse(int begin, int end)
{
    int windows = mmio.dispcnt.win0 || mmio.dispcnt.win1 || mmio.dispcnt.winobj;
    int effects = mmio.bldcnt.mode != BLD_DISABLED || obj_alpha;

    std::vector<Layer> layers;
    layers.reserve(end - begin);

    for (int bg = begin; bg < end; ++bg)
    {
        if (mmio.dispcnt.bg[bg])
        {
            layers.emplace_back(
                bg,
                bgs[bg].data(),
                mmio.bgcnt[bg].priority,
                1 << bg
            );
        }
    }

    std::sort(layers.begin(), layers.end(), [](const Layer& lhs, const Layer& rhs) {
        if (lhs.prio == rhs.prio)
            return lhs.id < rhs.id;
        return lhs.prio < rhs.prio;
    });

    switch ((effects << 1) | (windows << 0))
    {
    case 0b00:
        collapseNN(layers);
        break;

    case 0b01:
        collapseNW(layers);
        break;

    case 0b10:
        collapseBN(layers);
        break;

    case 0b11:
        collapseBW(layers);
        break;

    default:
        UNREACHABLE;
        break;
    }
}

void PPU::collapseNN(const std::vector<Layer>& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
    u16  backdrop = mmu.palette.readHalf(0);

    for (int x = 0; x < WIDTH; ++x)
    {
        scanline[x] = backdrop;
        const auto& object = obj[x];

        for (const auto& layer : layers)
        {
            if (object.visible(layer.prio))
            {
                scanline[x] = object.color;
                goto next;
            }
            if (layer.opaque(x))
            {
                scanline[x] = layer.data[x];
                goto next;
            }
        }
        if (object.visible())
        {
            scanline[x] = object.color;
        }
    next:;
    }
}

/**
 * Todo: small performance gains are possible when using templates. The active
 * windows (win0, win1, winobj) can be determined beforehand and reduce the number
 * of branches to the minimum required.
 */
void PPU::collapseNW(const std::vector<Layer>& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
    u16  backdrop = mmu.palette.readHalf(0);

    bool win0   = mmio.winv[0].contains(mmio.vcount);
    bool win1   = mmio.winv[1].contains(mmio.vcount);
    bool winobj = mmio.dispcnt.winobj;

    const auto& winh   = mmio.winh;
    const auto& winin  = mmio.winin;
    const auto& winout = mmio.winout;

    for (int x = 0; x < WIDTH; ++x)
    {
        int flags = winout.winout.flags;
        if (win0 && winh[0].contains(x))
        {
            flags = winin.win0.flags;
        }
        else if (win1 && winh[1].contains(x))
        {
            flags = winin.win1.flags;
        }
        else if (winobj && obj[x].window)
        {
            flags = winout.winobj.flags;
        }

        scanline[x] = backdrop;
        const auto& object = obj[x];

        for (const auto& layer : layers)
        {
            if (flags & LF_OBJ && object.visible(layer.prio))
            {
                scanline[x] = object.color;
                goto next;
            }
            if (flags & layer.flag && layer.opaque(x))
            {
                scanline[x] = layer.data[x];
                goto next;
            }
        }
        if (flags & LF_OBJ && object.visible())  // Opaque?
        {
            scanline[x] = object.color;
        }
    next:;
    }
}

void PPU::collapseBN(const std::vector<Layer>& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        int upper = 0;
        int lower = 0;

        const auto& object = obj[x];

        if (object.mode == GFX_ALPHA && findUpperLayers(layers, x, upper, lower))
        {
            scanline[x] = blendAlpha(upper, lower);
        }
        else
        {
            switch (mmio.bldcnt.mode)
            {
            case BLD_ALPHA:
                if (findUpperLayers(layers, x, upper, lower))
                    scanline[x] = blendAlpha(upper, lower);
                else
                    scanline[x] = upper;
                break;

            case BLD_WHITE:
                if (findUpperLayers(layers, x, upper))
                    scanline[x] = blendWhite(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_BLACK:
                if (findUpperLayers(layers, x, upper))
                    scanline[x] = blendBlack(upper);
                else
                    scanline[x] = upper;
                break;

            case BLD_DISABLED:
                for (const auto& layer : layers)
                {
                    if (object.visible(layer.prio))
                    {
                        scanline[x] = object.color;
                        goto next;
                    }
                    if (layer.opaque(x))
                    {
                        scanline[x] = layer.data[x];
                        goto next;
                    }
                }
                if (object.visible())
                {
                    scanline[x] = object.color;
                }
                else
                {
                    scanline[x] = mmu.palette.readHalf(0);
                }
                break;
            }
        }
    next:;
    }
}

void PPU::collapseBW(const std::vector<Layer>& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
    std::fill_n(scanline, WIDTH, 0x1F << 5);
}

bool PPU::findUpperLayers(const std::vector<Layer>& layers, int x, int& upper)
{
    const auto& object = obj[x];

    int flags = mmio.bldcnt.upper.flags;

    if (object.mode == GFX_ALPHA)
    {
        if (object.opaque())
        {
            // Todo: what should happen if the object is NOT the topmost layer?

            upper = object.color;
            return true;
        }
        else  // Object can be ignored because it's transparent
        {
            for (const auto& layer : layers)
            {
                if (layer.opaque(x))
                {
                    upper = layer.data[x];
                    return flags & layer.flag;
                }
            }
            upper = mmu.palette.readHalf(0);
            return flags & LF_BDP;
        }
    }
    else
    {
        for (const auto& layer : layers)
        {
            if (object.visible(layer.prio))
            {
                upper = object.color;
                return flags & LF_OBJ;
            }
            if (layer.opaque(x))
            {
                upper = layer.data[x];
                return flags & layer.flag;
            }
        }
        if (object.visible())  // Opaque?
        {
            upper = object.color;
            return flags & LF_OBJ;
        }
        upper = mmu.palette.readHalf(0);
        return flags & LF_BDP;
    }
}

bool PPU::findUpperLayers(const std::vector<Layer>& layers, int x, int& upper, int& lower)
{
    const auto& object = obj[x];

    int upper_flags = object.mode == GFX_ALPHA ? LF_OBJ : mmio.bldcnt.upper.flags;
    int lower_flags = mmio.bldcnt.lower.flags;

    bool upper_found = false;

    bool used = false;
    for (const auto& layer : layers)
    {
        if (!used && object.visible(layer.prio))
        {
            if (upper_found)
            {
                lower = object.color;
                return upper_found && lower_flags & LF_OBJ;
            }
            else
            {
                upper_found = true;
                upper = object.color;
                if ((upper_flags & LF_OBJ) == 0)
                    return false;
            }
            used = true;
        }
        if (layer.opaque(x))
        {
            if (upper_found)
            {
                lower = layer.data[x];
                return upper_found && lower_flags & layer.flag;
            }
            else
            {
                upper_found = true;
                upper = layer.data[x];
                if ((upper_flags & layer.flag) == 0)
                    return false;
            }
        }
    }
    if (!used && object.visible())
    {
        if (upper_found)
        {
            lower = object.color;
            return upper_found && lower_flags & LF_OBJ;
        }
        else
        {
            upper_found = true;
            upper = object.color;
            if ((upper_flags & LF_OBJ) == 0)
                return false;
        }
    }
    if (upper_found)
    {
        lower = mmu.palette.readHalf(0);
        return lower_flags & LF_BDP;
    }
    else
    {
        upper = mmu.palette.readHalf(0);
        return false;
    }
}
