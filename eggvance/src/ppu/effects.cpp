#include "ppu.h"

#include "mmu/map.h"

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
    for (int x = 0; x < WIDTH; ++x)
    {
        u16* a = nullptr;
        int  b = -1;

        if (sprites[x].semi_transparent)
        {
            // Semi-transparent sprites use alpha blending if possible
            if (findBlendLayers(x, a, b))
            {
                alphaBlend(a, b);
                continue;
            }
            else  // Reset layers
            {
                a = nullptr;
                b = -1;
            }
        }

        switch (mmu.bldcnt.mode)
        {
        // Alpha blend
        case 0b01:
            if (findBlendLayers(x, a, b))
                alphaBlend(a, b);
            break;

        // Fade to white
        case 0b10:
            if (findBlendLayers(x, a))
                fadeToWhite(a);
            break;

        // Fade to black
        case 0b11:
            if (findBlendLayers(x, a))
                fadeToBlack(a);
            break;
        }
    }
}

bool PPU::findBlendLayers(int x, u16*& a)
{
    SpritePixel& sprite = sprites[x];

    bool opaque[4] = {
        buffer[0][x] != COLOR_TRANSPARENT,
        buffer[1][x] != COLOR_TRANSPARENT,
        buffer[2][x] != COLOR_TRANSPARENT,
        buffer[3][x] != COLOR_TRANSPARENT
    };
    bool opaque_sprite = sprite.pixel != COLOR_TRANSPARENT;

    if (mmu.dispcnt.sprites && opaque_sprite && sprite.semi_transparent)
    {
        // Semi-transparent sprites are always layer A
        a = &sprite.pixel;
        return true;
    }
    else
    {
        for (int priority = 0; priority < 4; ++priority)
        {
            if (mmu.dispcnt.sprites && opaque_sprite && sprite.priority == priority)
            {
                if (mmu.bldcnt.a_sprites)
                {
                    a = &sprite.pixel;
                    return true;
                }
                else
                {
                    // Layer A is not on top
                    return false;
                }
            }

            for (int bg = 0; bg < 4; ++bg)
            {
                if (mmu.dispcnt.bg(bg) && opaque[bg] && mmu.bgcnt[bg].priority == priority)
                {
                    if (mmu.bldcnt.a_bg(bg))
                    {
                        a = &buffer[bg][x];
                        return true;
                    }
                    else
                    {
                        // Layer A is not on top
                        return false;
                    }
                }
            }
        }

        // All other layers are transparent
        if (mmu.bldcnt.a_backdrop)
        {
            for (int bg = 0; bg < 4; ++bg)
            {
                if (mmu.dispcnt.bg(bg))
                {
                    // Todo: find a better way to do this
                    // Assign backdrop to first enabled background
                    buffer[bg][x] = mmu.readHalfFast(MAP_PALETTE);
                    a = &buffer[bg][x];
                    return true;
                }
            }
        }
    }
    return false;
}

bool PPU::findBlendLayers(int x, u16*& a, int& b)
{
    SpritePixel& sprite = sprites[x];

    bool opaque[4] = {
        buffer[0][x] != COLOR_TRANSPARENT,
        buffer[1][x] != COLOR_TRANSPARENT,
        buffer[2][x] != COLOR_TRANSPARENT,
        buffer[3][x] != COLOR_TRANSPARENT
    };
    bool opaque_sprite = sprite.pixel != COLOR_TRANSPARENT;

    if (mmu.dispcnt.sprites && opaque_sprite && sprite.semi_transparent)
        // Semi-transparent sprites are always layer A
        a = &sprite.pixel;

    for (int priority = 0; priority < 4; ++priority)
    {
        if (mmu.dispcnt.sprites && opaque_sprite && sprite.priority == priority && !sprite.semi_transparent)
        {
            if (mmu.bldcnt.a_sprites && !a)
            {
                a = &sprite.pixel;
            } 
            else if (mmu.bldcnt.b_sprites && b == -1)
            {
                b = sprite.pixel;
                return a != nullptr;
            } 
            else
            {
                // Non-transparent pixel in front or between layers A and B
                return false;
            }
        }

        for (int bg = 0; bg < 4; ++bg)
        {
            if (mmu.dispcnt.bg(bg) && opaque[bg] && mmu.bgcnt[bg].priority == priority)
            {
                if (mmu.bldcnt.a_bg(bg) && !a)
                {
                    a = &buffer[bg][x];
                }
                else if (mmu.bldcnt.b_bg(bg) && b == -1)
                {
                    b = buffer[bg][x];
                    return a != nullptr;
                }
                else
                {
                    // Non-transparent pixel in front or between layers A and B
                    return false;
                }
            }
        }

        // Backdrop only makes sense for layer B
        if (a && b == -1)
        {
            if (mmu.bldcnt.b_backdrop)
            {
                b = mmu.readHalfFast(MAP_PALETTE);
                return true;
            }
        }
    }
    return false;
}

void PPU::alphaBlend(u16* a, int b)
{
    int a_r = (*a >>  0) & 0x1F;
    int a_g = (*a >>  5) & 0x1F;
    int a_b = (*a >> 10) & 0x1F;
    int b_r = ( b >>  0) & 0x1F;
    int b_g = ( b >>  5) & 0x1F;
    int b_b = ( b >> 10) & 0x1F;

    int eva = std::min(static_cast<int>(mmu.bldalpha.eva), 17);
    int evb = std::min(static_cast<int>(mmu.bldalpha.evb), 17);

    int t_r = std::min(31, (a_r * eva + b_r * evb) >> 4);
    int t_g = std::min(31, (a_g * eva + b_g * evb) >> 4);
    int t_b = std::min(31, (a_b * eva + b_b * evb) >> 4);

    *a = (t_r << 0) | (t_g << 5) | (t_b << 10);
}

void PPU::fadeToWhite(u16* a)
{
    int a_r = (*a >>  0) & 0x1F;
    int a_g = (*a >>  5) & 0x1F;
    int a_b = (*a >> 10) & 0x1F;

    int evy = std::min(static_cast<int>(mmu.bldy.evy), 17);

    int t_r = std::min(31, a_r + (((31 - a_r) * evy) >> 4));
    int t_g = std::min(31, a_g + (((31 - a_g) * evy) >> 4));
    int t_b = std::min(31, a_b + (((31 - a_b) * evy) >> 4));

    *a = (t_r << 0) | (t_g << 5) | (t_b << 10);
}

void PPU::fadeToBlack(u16* a)
{
    int a_r = (*a >>  0) & 0x1F;
    int a_g = (*a >>  5) & 0x1F;
    int a_b = (*a >> 10) & 0x1F;

    int evy = std::min(static_cast<int>(mmu.bldy.evy), 17);

    int t_r = std::min(31, a_r - ((a_r * evy) >> 4));
    int t_g = std::min(31, a_g - ((a_g * evy) >> 4));
    int t_b = std::min(31, a_b - ((a_b * evy) >> 4));

    *a = (t_r << 0) | (t_g << 5) | (t_b << 10);
}
