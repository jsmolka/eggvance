#pragma once

#include "common/integer.h"
#include "ppu/dimensions.h"
#include "ppu/point.h"

enum class GraphicsMode
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
    int tilesPerRow(int mapping) const;

    bool flipX() const;
    bool flipY() const;

    bool isDisabled() const;

    Point origin;
    Dimensions dims;

    int affine;
    int double_size;
    int disabled;
    int graphics_mode;
    int mosaic;
    int color_mode;
    int shape;

    int matrix_index;
    int flip_x;
    int flip_y;
    int size;

    int tile;
    int prio;
    int bank;

    u32 base_tile;

private:
    void update();
};
