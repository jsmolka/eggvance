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
        origin.y    = bits::seq< 0, 8>(half);
        affine      = bits::seq< 8, 1>(half);
        double_size = bits::seq< 9, 1>(half);
        disabled    = bits::seq< 9, 1>(half);
        mode        = bits::seq<10, 2>(half);
        mosaic      = bits::seq<12, 1>(half);
        color_mode  = bits::seq<13, 1>(half);
        shape       = bits::seq<14, 2>(half);
        break;

    case 2:
        origin.x = bits::seq< 0, 9>(half);
        matrix   = bits::seq< 9, 5>(half);
        flip_x   = bits::seq<12, 1>(half);
        flip_y   = bits::seq<13, 1>(half);
        size     = bits::seq<14, 2>(half);
        break;

    case 4:
        tile  = bits::seq< 0, 10>(half);
        prio  = bits::seq<10,  2>(half);
        bank  = bits::seq<12,  4>(half);
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
    return (origin.x + bounds.w) >= 0 && origin.x < kScreenW;
}

bool OAMEntry::isVisibleY(int vcount) const
{
    return vcount >= origin.y && vcount < (origin.y + bounds.h);
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
