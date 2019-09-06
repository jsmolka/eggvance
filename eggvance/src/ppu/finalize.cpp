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
            Layer layer;
            layer.data = &bgs[bg][0];
            layer.prio = mmio.bgcnt[bg].priority;
            layer.flag = 1 << bg;
            layers.data.push_back(layer);
        }
    }
    layers.sort();

    switch ((e << 1) | (w << 0))
    {
    case 0b00: finalize__(layers); break;
    case 0b01: finalize_W(layers); break;

    default:
        fmt::printf("%d", ++f);
        finalizeOld();
    }
}

// Todo: remove goto
// Todo: think about cleaner and more performant layer version
void PPU::finalize__(Layers& layers)
{
    u16  backdrop = mmu.palette.readHalf(0);
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    for (int x = 0; x < WIDTH; ++x)
    {
        scanline[x] = backdrop;
        for (const Layer& layer : layers.data)
        {
            if (obj[x].priority <= layer.prio && obj[x].color != COLOR_T)
            {
                scanline[x] = obj[x].color;
                goto end;
            }
            if (layer.set(scanline, x))
            {
                goto end;
            }
        }
        if (obj[x].color != COLOR_T)
        {
            scanline[x] = obj[x].color;
        }
    end: ;
    }
}

void PPU::finalize_W(Layers& layers)
{
    u16  backdrop = mmu.palette.readHalf(0);
    u16* scanline = &backend.buffer[WIDTH * mmio.vcount];

    bool win0 = mmio.winv[0].contains(mmio.vcount);
    bool win1 = mmio.winv[1].contains(mmio.vcount);

    const auto& winh   = mmio.winh;
    const auto& winin  = mmio.winin;
    const auto& winout = mmio.winout;

    for (int x = 0; x < WIDTH; ++x)
    {
        scanline[x] = backdrop;

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

        for (const Layer& layer : layers.data)
        {
            if (flags & LF_OBJ && obj[x].priority <= layer.prio && obj[x].color != COLOR_T)
            {
                scanline[x] = obj[x].color;
                goto end;
            }
            if (layer.flag & flags && layer.set(scanline, x))
            {
                goto end;
            }
        }
        if (flags & LF_OBJ && obj[x].color != COLOR_T)
        {
            scanline[x] = obj[x].color;
        }
        end: ;
    }
}
