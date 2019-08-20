#pragma once

#include "common/integer.h"

enum GraphicsMode
{
    GFX_NORMAL     = 0b00,  // Normal
    GFX_ALPHA      = 0b01,  // Blend alpha
    GFX_WINDOW     = 0b10,  // Window
    GFX_PROHIBITED = 0b11   // Prohibited
};

struct OAMEntry
{
    int y;            // Y coordiante
    int affine;       // Affine flag
    int double_size;  // Double size flag (if affine)
    int disabled;     // Disabled flag (if not affine)
    int gfx_mode;     // Graphics mode (0 = normal, 1 = alpha blending, 2 = window, 3 = prohibited)
    int mosaic;       // Mosaic flag
    int color_mode;   // Color mode (1 = 256/1, 0 = 16/16)
    int shape;        // Object shape (0 = square, 1 = horizontal, 2 = vertical, 3 = prohibited)

    int x;         // X coordinate
    int paramter;  // Affine paramter (if affine)
    int flip_x;    // Horizontal flip (if not affine)
    int flip_y;    // Vertical flip (if not affine)
    int size;      // Object size

    int tile;      // Base tile number
    int priority;  // Priority
    int palette;   // Palette bank for 16/16 color mode

    int width() const;
    int height() const;
};
