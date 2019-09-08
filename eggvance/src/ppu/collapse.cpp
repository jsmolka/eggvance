#include "ppu.h"

/**
 * Todo: All the object could be removed here. Templating all the other
 * collapse functions (with obj_exist) could have a positive performance
 * impact.
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
                bgs[bg].data(),
                mmio.bgcnt[bg].priority,
                1 << bg
            );
        }
    }

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

    std::fill_n(scanline, WIDTH, 0x1F << 5);
    return;

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

    //std::fill_n(scanline, WIDTH, 0x1F << 5);
    //return;

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
        if (flags & LF_OBJ && object.visible())
        {
            scanline[x] = object.color;
        }
    next:;
    }
}

void PPU::collapseBN(const std::vector<Layer>& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
    std::fill_n(scanline, WIDTH, 0x1F << 5);
}

void PPU::collapseBW(const std::vector<Layer>& layers)
{
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];
    std::fill_n(scanline, WIDTH, 0x1F << 5);
}
