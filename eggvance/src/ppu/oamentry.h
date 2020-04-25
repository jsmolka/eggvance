#pragma once

#include "dimensions.h"
#include "base/enums.h"
#include "base/integer.h"

enum class ObjectMode
{
    Normal  = 0b00,
    Alpha   = 0b01,
    Window  = 0b10,
    Invalid = 0b11
};

struct OAMEntry
{
    void reset();

    void writeHalf(int attr, u16 half);

    int tileSize() const;
    int paletteBank() const;
    int tilesPerRow(ObjectMapping mapping) const;

    bool flipX() const;
    bool flipY() const;

    bool isDisabled() const;
    bool isVisible(int vcount) const;

    int affine;
    int double_size;
    int disabled;
    int mode;
    int mosaic;
    int color_mode;
    int shape;

    int matrix;
    int flip_x;
    int flip_y;
    int size;

    int tile;
    int prio;
    int bank;

    Point origin;
    Point center;
    Dimensions dims;
    Dimensions bounds;
    u32 base_tile;

private:
    bool isVisibleX() const;
    bool isVisibleY(int vcount) const;

    void update();
};
