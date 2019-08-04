#pragma once

struct BackgroundControl
{
    int priority;      // BG priority (0-3, 0 = highest)
    int tile_block;    // BG tile base block (0-3, in units of 16kb)
    int mosaic;        // Mosaic (1 = enable)
    int palette_type;  // Palettes type (0 = 16/16, 1 = 256/1)
    int map_block;     // BG map base block (0-31, in units of 2kb)
    int wraparound;    // Display area overflow (1 = enable)
    int screen_size;   // Screen size

    int size() const;
    int width() const;
    int height() const;
};
