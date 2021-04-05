#include "oamentry.h"

#include <shell/operators.h>

#include "constants.h"
#include "base/bit.h"

OamEntry::OamEntry()
{
    compute();
}

void OamEntry::writeAttr0(u16 half)
{
    origin.y    = bit::seq< 0, 8>(half);
    affine      = bit::seq< 8, 1>(half);
    double_size = bit::seq< 9, 1>(half);
    disabled    = bit::seq< 9, 1>(half) && !affine;
    object_mode = bit::seq<10, 2>(half);
    mosaic      = bit::seq<12, 1>(half);
    color_mode  = bit::seq<13, 1>(half);
    shape       = bit::seq<14, 2>(half);

    compute();
}

void OamEntry::writeAttr1(u16 half)
{
    origin.x = bit::seq< 0, 9>(half);
    matrix   = bit::seq< 9, 5>(half);
    flip.x   = bit::seq<12, 1>(half);
    flip.y   = bit::seq<13, 1>(half);
    size     = bit::seq<14, 2>(half);

    compute();
}

void OamEntry::writeAttr2(u16 half)
{
    base_tile = bit::seq< 0, 10>(half);
    priority  = bit::seq<10,  2>(half);
    bank      = bit::seq<12,  4>(half);

    compute();
}

bool OamEntry::isVisible(uint line) const
{
    return visible_x
        && static_cast<int>(line) >= origin.y
        && static_cast<int>(line) < (origin.y + screen_size.y);
}

uint OamEntry::tileBytes() const
{
    return kTileBytes[color_mode];
}

uint OamEntry::tilesInRow(uint layout) const
{
    static constexpr Point kTileLayout2d[2] = {
        Point(32, 32), Point(16, 32)
    };

    return layout == ObjectLayout::TwoDim
        ? kTileLayout2d[color_mode].x
        : sprite_size.x / kTileSize;
}

uint OamEntry::paletteBank() const
{
    return color_mode == ColorMode::C16x16
        ? bank
        : 0;
}

int OamEntry::cycles() const
{
    return affine
        ? screen_size.x * 2 + 10
        : screen_size.x;
}

void OamEntry::compute()
{
    static constexpr Point kSpriteSizes[4][4] =
    {
        { {  8,  8 }, { 16, 16 }, { 32, 32 }, { 64, 64 }, },
        { { 16,  8 }, { 32,  8 }, { 32, 16 }, { 64, 32 }, },
        { {  8, 16 }, {  8, 32 }, { 16, 32 }, { 32, 64 }, },
        { {  8,  8 }, {  8,  8 }, {  8,  8 }, {  8,  8 }  }
    };

    base_addr = kObjectBase + kTileBytes[uint(ColorMode::C16x16)] * base_tile;

    if (origin.x >= kScreen.x) origin.x -= 512;
    if (origin.y >= kScreen.y) origin.y -= 256;

    sprite_size = kSpriteSizes[shape][size];
    screen_size = sprite_size << double_size;
    center      = origin + screen_size / 2;

    if (flip.x) flip.x = sprite_size.x - 1;
    if (flip.y) flip.y = sprite_size.y - 1;

    visible_x = (origin.x + screen_size.x) >= 0 && origin.x < kScreen.x;
}
