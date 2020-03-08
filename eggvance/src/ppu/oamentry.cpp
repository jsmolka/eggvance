#include "oamentry.h"

#include "common/bits.h"
#include "common/constants.h"
#include "common/macros.h"

static constexpr Dimensions sizes[4][4] =
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

void OAMEntry::reset()
{
    *this = {};

    update();
}

void OAMEntry::writeHalf(int attr, u16 half)
{
    ASSERT((attr & 0x6) < 0x6, "Invalid attribute");

    switch (attr)
    {
    case 0:
        origin.y    = bits< 0, 8>(half);
        affine      = bits< 8, 1>(half);
        double_size = bits< 9, 1>(half);
        disabled    = bits< 9, 1>(half);
        mode        = bits<10, 2>(half);
        mosaic      = bits<12, 1>(half);
        color_mode  = bits<13, 1>(half);
        shape       = bits<14, 2>(half);
        break;

    case 2:
        origin.x = bits< 0, 9>(half);
        matrix   = bits< 9, 5>(half);
        flip_x   = bits<12, 1>(half);
        flip_y   = bits<13, 1>(half);
        size     = bits<14, 2>(half);
        break;

    case 4:
        tile  = bits< 0, 10>(half);
        prio  = bits<10,  2>(half);
        bank  = bits<12,  4>(half);
        break;

    default:
        UNREACHABLE;
        break;
    }

    update();
}

int OAMEntry::tileSize() const
{
    return 0x20 << color_mode;
}

int OAMEntry::paletteBank() const
{
    return color_mode == int(ColorMode::C256x1) ? 0 : bank;
}

int OAMEntry::tilesPerRow(ObjectMapping mapping) const
{
    return mapping == ObjectMapping::OneDim ? (dims.w / 8) : (32 >> color_mode);
}

bool OAMEntry::flipX() const
{
    return !affine && flip_x;
}

bool OAMEntry::flipY() const
{
    return !affine && flip_y;
}

bool OAMEntry::isDisabled() const
{
    return !affine && disabled;
}

bool OAMEntry::isVisible(int vcount) const
{
    return isVisibleY(vcount) && isVisibleX();
}

bool OAMEntry::isVisibleX() const
{
    return (origin.x + bounds.w) >= 0 && origin.x < SCREEN_W;
}

bool OAMEntry::isVisibleY(int vcount) const
{
    return vcount >= origin.y && vcount < (origin.y + bounds.h);
}

void OAMEntry::update()
{
    if (origin.x >= SCREEN_W) origin.x -= 512;
    if (origin.y >= SCREEN_H) origin.y -= 256;

    dims = sizes[shape][size];
    bounds = dims << double_size;
    center = origin + bounds / 2;

    base_tile = 0x10000 + 0x20 * tile;
}
