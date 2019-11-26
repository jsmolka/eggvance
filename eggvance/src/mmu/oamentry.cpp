#include "oamentry.h"

#include "common/constants.h"
#include "common/utility.h"
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
}

void OAMEntry::writeHalf(int attr, u16 half)
{
    EGG_ASSERT(attr % 2 == 0 && attr <= 4, "Invalid attribute");

    switch (attr)
    {
    case 0:
        attr0       = half;
        origin.y    = bits< 0, 8>(half);
        affine      = bits< 8, 1>(half);
        double_size = bits< 9, 1>(half);
        disabled    = bits< 9, 1>(half);
        mode    = bits<10, 2>(half);
        mosaic      = bits<12, 1>(half);
        color_mode  = bits<13, 1>(half);
        shape       = bits<14, 2>(half);
        break;

    case 2:
        attr1     = half;
        origin.x  = bits< 0, 9>(half);
        parameter = bits< 9, 5>(half);
        flip_x    = bits<12, 1>(half);
        flip_y    = bits<13, 1>(half);
        size      = bits<14, 2>(half);
        break;

    case 4:
        attr2        = half;
        tile         = bits< 0, 10>(half);
        prio     = bits<10,  2>(half);
        palette_bank = bits<12,  4>(half);
        break;

    default:
        EGG_UNREACHABLE;
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
    return color_mode ? 0 : palette_bank;
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

bool OAMEntry::isUninitialized() const
{
    return attr0 == 0 && attr1 == 0 && attr2 == 0;
}

void OAMEntry::update()
{
    if (origin.x >= SCREEN_W) origin.x -= 512;
    if (origin.y >= SCREEN_H) origin.y -= 256;

    dims = sizes[shape][size];
}
