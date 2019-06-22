#include "ppu.h"

void PPU::effects()
{
    mosaic();
    blend();
}

void PPU::mosaic()
{
    for (int bg = 0; bg < 4; ++bg)
    {
        if (mmu.dispcnt.bg(bg) && mmu.bgcnt[bg].mosaic)
            mosaicBg(buffer[bg]);
    }
}

void PPU::mosaicBg(DoubleBuffer& buffer)
{
    int mosaic_x = mmu.mosaic.bg_x + 1;
    int mosaic_y = mmu.mosaic.bg_y + 1;

    if (mmu.vcount.line % mosaic_y == 0)
    {
        if (mosaic_x == 1)
            return;

        int color;
        for (int x = 0; x < WIDTH; ++x)
        {
            if (x % mosaic_x == 0)
                color = buffer[x];

            buffer[x] = color;
        }
    }
    else
    {
        buffer.copyPage();
    }
}

void PPU::blend()
{
    if (mmu.bldcnt.mode == 0)
        return;

    for (int x = 0; x < WIDTH; ++x)
    {
        BlendPixel a = blendLayerA(x);
        BlendPixel b = blendLayerB(x);

        // Todo: probably wrong for sprites
        if (a.prio <= b.prio)
        {
            if (a.pixel && b.pixel && *a.pixel != COLOR_TRANSPARENT && *b.pixel != COLOR_TRANSPARENT)
                alphaBlend(a.pixel, b.pixel);
        }
    }
}

PPU::BlendPixel PPU::blendLayerA(int x)
{
    BlendPixel bp;
    if (mmu.bldcnt.a_bg3 && mmu.dispcnt.bg3 && mmu.bg3cnt.priority <= bp.prio)
    {
        bp.pixel = &buffer[3][x];
        bp.prio = mmu.bg3cnt.priority;
    }
    if (mmu.bldcnt.a_bg2 && mmu.dispcnt.bg2 && mmu.bg2cnt.priority <= bp.prio)
    {
        bp.pixel = &buffer[2][x];
        bp.prio = mmu.bg2cnt.priority;
    }
    if (mmu.bldcnt.a_bg1 && mmu.dispcnt.bg1 && mmu.bg1cnt.priority <= bp.prio)
    {
        bp.pixel = &buffer[1][x];
        bp.prio = mmu.bg1cnt.priority;
    }
    if (mmu.bldcnt.a_bg0 && mmu.dispcnt.bg0 && mmu.bg0cnt.priority <= bp.prio)
    {
        bp.pixel = &buffer[0][x];
        bp.prio = mmu.bg0cnt.priority;
    }
    if (mmu.bldcnt.a_obj && mmu.dispcnt.sprites)
    {
        if (sprites[x].priority <= bp.prio)
        {
            bp.pixel = &sprites[x].pixel;
            bp.prio = sprites[x].priority;
        }
    }
    return bp;
}

PPU::BlendPixel PPU::blendLayerB(int x)
{
    BlendPixel bp;
    if (mmu.bldcnt.b_bg3 && mmu.dispcnt.bg3 && mmu.bg3cnt.priority <= bp.prio)
    {
        bp.pixel = &buffer[3][x];
        bp.prio = mmu.bg3cnt.priority;
    }
    if (mmu.bldcnt.b_bg2 && mmu.dispcnt.bg2 && mmu.bg2cnt.priority <= bp.prio)
    {
        bp.pixel = &buffer[2][x];
        bp.prio = mmu.bg2cnt.priority;
    }
    if (mmu.bldcnt.b_bg1 && mmu.dispcnt.bg1 && mmu.bg1cnt.priority <= bp.prio)
    {
        bp.pixel = &buffer[1][x];
        bp.prio = mmu.bg1cnt.priority;
    }
    if (mmu.bldcnt.b_bg0 && mmu.dispcnt.bg0 && mmu.bg0cnt.priority <= bp.prio)
    {
        bp.pixel = &buffer[0][x];
        bp.prio = mmu.bg0cnt.priority;
    }
    if (mmu.bldcnt.b_obj && mmu.dispcnt.sprites)
    {
        if (sprites[x].priority <= bp.prio)
        {
            bp.pixel = &sprites[x].pixel;
            bp.prio = sprites[x].priority;
        }
    }
    return bp;
}

void PPU::alphaBlend(u16* a, u16* b)
{
    u16 a_r = (*a >>  0) & 0x1F;
    u16 a_g = (*a >>  5) & 0x1F;
    u16 a_b = (*a >> 10) & 0x1F;
    u16 b_r = (*b >>  0) & 0x1F;
    u16 b_g = (*b >>  5) & 0x1F;
    u16 b_b = (*b >> 10) & 0x1F;

    int eva = std::min(static_cast<int>(mmu.bldalpha.eva), 17);
    int evb = std::min(static_cast<int>(mmu.bldalpha.evb), 17);

    u16 t_r = std::min(31, (a_r * eva + b_r * evb) >> 4);
    u16 t_g = std::min(31, (a_g * eva + b_g * evb) >> 4);
    u16 t_b = std::min(31, (a_b * eva + b_b * evb) >> 4);

    *a = (t_r << 0) | (t_g << 5) | (t_b << 10);
}
