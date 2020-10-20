#include "oamentry.h"

#include "constants.h"
#include "base/bit.h"
#include "base/macros.h"

OamEntry::OamEntry()
{
    compute();
}

void OamEntry::write(uint attr, u16 half)
{
    enum Attribute
    {
        kAttribute0 = 0,
        kAttribute1 = 2,
        kAttribute2 = 4
    };

    switch (attr)
    {
    case kAttribute0:
        origin.y    = bit::seq< 0, 8>(half);
        affine      = bit::seq< 8, 1>(half);
        double_size = bit::seq< 9, 1>(half);
        disabled    = bit::seq< 9, 1>(half) && !affine;
        object_mode = bit::seq<10, 2>(half);
        mosaic      = bit::seq<12, 1>(half);
        color_mode  = bit::seq<13, 1>(half);
        shape       = bit::seq<14, 2>(half);
        break;

    case kAttribute1:
        origin.x   = bit::seq< 0, 9>(half);
        matrix_idx = bit::seq< 9, 5>(half);
        flip_x     = bit::seq<12, 1>(half);
        flip_y     = bit::seq<13, 1>(half);
        size       = bit::seq<14, 2>(half);
        break;

    case kAttribute2:
        base_tile = bit::seq< 0, 10>(half);
        priority  = bit::seq<10,  2>(half);
        bank      = bit::seq<12,  4>(half);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }

    compute();
}

bool OamEntry::isVisible(uint line) const
{
    return visible_x
        && static_cast<int>(line) >= origin.y
        && static_cast<int>(line) < (origin.y + screen_size.y);
}

uint OamEntry::tileSize() const
{
    return 0x20 << color_mode;
}

uint OamEntry::tilesInRow(uint layout) const
{
    return layout == kObjectLayout2d
        ? 32 >> color_mode
        : sprite_size.x / 8;
}

uint OamEntry::paletteBank() const
{
    return color_mode == kColorMode16x16
        ? bank
        : 0;
}

void OamEntry::compute()
{
    static constexpr Point kSpriteSizes[4][4] =
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

    base_addr = 0x1'0000 + 0x20 * base_tile;

    if (origin.x >= kScreen.x) origin.x -= 512;
    if (origin.y >= kScreen.y) origin.y -= 256;

    sprite_size = kSpriteSizes[shape][size];
    screen_size = sprite_size << double_size;
    center      = origin + screen_size / 2;

    if (flip_x) flip_x = sprite_size.x - 1;
    if (flip_y) flip_y = sprite_size.y - 1;

    visible_x = (origin.x + screen_size.x) >= 0 && origin.x < kScreen.x;
}