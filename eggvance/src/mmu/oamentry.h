#pragma once

#include "common/integer.h"
#include "ppu/dimensions.h"
#include "ppu/point.h"

// Todo: rename values
// Todo: move uneeded to private
struct OAMEntry
{
    enum Mode
    {
        NORMAL  = 0b00,
        ALPHA   = 0b01,
        WINDOW  = 0b10,
        INVALID = 0b11
    };

    void reset();

    void writeHalf(int attr, u16 half);

    int tileSize() const;
    int paletteBank() const;

    bool flipX() const;
    bool flipY() const;

    bool isDisabled() const;
    bool isUninitialized() const;

    Point origin;
    Dimensions dims;

    int affine;       // Affine flag
    int double_size;  // Double size flag (if affine)
    int disabled;     // Disabled flag (if not affine)
    int mode;     // Graphics mode (0 = normal, 1 = alpha blending, 2 = window, 3 = prohibited)
    int mosaic;       // Mosaic flag
    int color_mode;   // Color mode (1 = 256/1, 0 = 16/16)
    int shape;        // Object shape (0 = square, 1 = horizontal, 2 = vertical, 3 = prohibited)

    int parameter;  // Affine paramter (if affine)
    int flip_x;     // Horizontal flip (if not affine)
    int flip_y;     // Vertical flip (if not affine)
    int size;       // Object size

    int tile;          // Base tile number
    int prio;      // Priority
    int palette_bank;  // Palette bank for 16/16 color mode

private:
    void update();

    u16 attr0;
    u16 attr1;
    u16 attr2;
};
