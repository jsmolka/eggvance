#pragma once

#include "common/bitvalue.h"

class OAMEntry
{
public:
    OAMEntry(int attr0, int attr1, int attr2);

    // Attribute 0
    BitValue< 0, 8> y;            // Y coordiante
    BitValue< 8, 1> affine;       // Affine flag
    BitValue< 9, 1> double_size;  // If affine: double size flag
    BitValue< 9, 1> disabled;     // If not affine: disabled flag
    BitValue<10, 2> gfx_mode;     // Graphics mode (0 = normal, 1 = alpha blending, 2 = window, 3 = prohibited)
    BitValue<12, 1> mosaic;       // Mosaic flag
    BitValue<13, 1> color_mode;   // Color mode (1 = 256/1, 0 = 16/16)
    BitValue<14, 2> shape;        // Object shape (0 = square, 1 = horizontal, 2 = vertical, 3 = prohibited)

    // Attribute 1
    BitValue< 0, 9> x;         // X coordinate
    BitValue< 9, 5> paramter;  // If affine: affine paramter
    BitValue<12, 1> flip_y;    // If not affine: horizontal flip
    BitValue<13, 1> flip_x;    // If not affine: vertical flip
    BitValue<14, 2> size;      // Object size

    // Attribute 2
    BitValue< 0, 10> tile;          // Base tile number
    BitValue<10,  2> priority;      // Priority
    BitValue<12,  4> palette_bank;  // Palette bank for 16/16 colors

    int width() const;
    int height() const;

private:
    static constexpr int dimensions[4][4][2] = 
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
};
