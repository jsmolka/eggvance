#include "oamentry.h"

#include "constants.h"
#include "base/bit.h"
#include "base/int.h"
#include "base/macros.h"

static constexpr Point kSizes[4][4] =
{
    {
        {  8,  8 },
        { 16, 16 },
        { 32, 32 },
        { 64, 64 },
    },
    {
        { 16,  8 },
        { 32,  8 },
        { 32, 16 },
        { 64, 32 },
    },
    {
        {  8, 16 },
        {  8, 32 },
        { 16, 32 },
        { 32, 64 },
    },
    {
        {  0,  0 },
        {  0,  0 },
        {  0,  0 },
        {  0,  0 }
    }
};

OamEntry::OamEntry()
{
    update();
}

void OamEntry::writeHalf(uint attr, u16 half)
{
    SHELL_ASSERT((attr & 0x6) < 0x6);

    switch (attr)
    {
    case 0:
        origin.y    = bit::seq< 0, 8>(half);
        affine      = bit::seq< 8, 1>(half);
        double_size = bit::seq< 9, 1>(half);
        disabled    = bit::seq< 9, 1>(half);
        mode        = bit::seq<10, 2>(half);
        mosaic      = bit::seq<12, 1>(half);
        color_mode  = bit::seq<13, 1>(half);
        shape       = bit::seq<14, 2>(half);
        break;

    case 2:
        origin.x = bit::seq< 0, 9>(half);
        matrix   = bit::seq< 9, 5>(half);
        flip_x   = bit::seq<12, 1>(half);
        flip_y   = bit::seq<13, 1>(half);
        size     = bit::seq<14, 2>(half);
        break;

    case 4:
        tile  = bit::seq< 0, 10>(half);
        prio  = bit::seq<10,  2>(half);
        bank  = bit::seq<12,  4>(half);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    update();
}

uint OamEntry::tileSize() const
{
    return 0x20 << color_mode;
}

uint OamEntry::paletteBank() const
{
    return color_mode == kColorMode16x16
        ? bank
        : 0;
}

uint OamEntry::tilesPerRow(uint layout) const
{
    return layout == kObjectLayout1d
        ? (dims.x / 8)
        : (32 >> color_mode);
}

bool OamEntry::flipX() const
{
    return !affine && flip_x;
}

bool OamEntry::flipY() const
{
    return !affine && flip_y;
}

bool OamEntry::isDisabled() const
{
    return !affine && disabled;
}

bool OamEntry::isVisible(int vcount) const
{
    return isVisibleY(vcount) && isVisibleX();
}

bool OamEntry::isVisibleX() const
{
    return (origin.x + bounds.x) >= 0 && origin.x < kScreen.x;
}

bool OamEntry::isVisibleY(int vcount) const
{
    return vcount >= origin.y && vcount < (origin.y + bounds.y);
}

void OamEntry::update()
{
    if (origin.x >= kScreen.x) origin.x -= 512;
    if (origin.y >= kScreen.y) origin.y -= 256;

    dims = kSizes[shape][size];
    bounds = dims << double_size;
    center = origin + bounds / 2;

    base_tile = 0x1'0000 + 0x20 * tile;
}
