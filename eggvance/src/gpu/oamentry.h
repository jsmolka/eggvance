#pragma once

#include "point.h"
#include "base/int.h"

class OamEntry
{
public:
    OamEntry();

    void writeHalf(uint attr, u16 half);

    uint tileSize() const;
    uint paletteBank() const;
    uint tilesPerRow(uint layout) const;

    bool flipX() const;
    bool flipY() const;

    bool isDisabled() const;
    bool isVisible(int vcount) const;

    uint affine;
    uint double_size;
    uint disabled;
    uint mode;
    uint mosaic;
    uint color_mode;
    uint shape;
    uint matrix;
    uint flip_x;
    uint flip_y;
    uint size;
    uint tile;
    uint prio;
    uint bank;

    Point origin;
    Point center;
    Point dims;
    Point bounds;
    u32 base_tile;

private:
    bool isVisibleX() const;
    bool isVisibleY(int vcount) const;

    void update();
};
