#include "oamentry.h"

#include "base/bit.h"
#include "base/constants.h"
#include "base/int.h"
#include "base/macros.h"

static constexpr Point sizes[4][4] =
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
    SHELL_ASSERT((attr & 0x6) < 0x6, "Invalid attribute");

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
    return mapping == ObjectMapping::OneDim ? (dims.x / 8) : (32 >> color_mode);
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
    return (origin.x + bounds.x) >= 0 && origin.x < kScreenW;
}

bool OAMEntry::isVisibleY(int vcount) const
{
    return vcount >= origin.y && vcount < (origin.y + bounds.y);
}

void OAMEntry::update()
{
    if (origin.x >= kScreenW) origin.x -= 512;
    if (origin.y >= kScreenH) origin.y -= 256;

    dims = sizes[shape][size];
    bounds = dims << double_size;
    center = origin + bounds / 2;

    base_tile = 0x10000 + 0x20 * tile;
}
