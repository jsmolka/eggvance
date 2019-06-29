#pragma once

enum LayerFlag
{
    LF_BG0  = 1 << 0,  // BG0 layer
    LF_BG1  = 1 << 1,  // BG1 layer
    LF_BG2  = 1 << 2,  // BG2 layer
    LF_BG3  = 1 << 3,  // BG3 layer
    LF_OBJ  = 1 << 4,  // OBJ layer
    LF_BDP  = 1 << 5   // Backdrop
};

enum WindowFlag
{
    WF_DISABLED = 1 << 0,  // No window
    WF_WIN0     = 1 << 1,  // Window 0
    WF_WIN1     = 1 << 2,  // Window 1
    WF_WINOBJ   = 1 << 3,  // Object window
    WF_WINOUT   = 1 << 4   // Outside window
};

enum BlendMode
{
    BLD_DISABLED = 0b00,  // Blend disabled
    BLD_ALPHA    = 0b01,  // Blend alpha
    BLD_WHITE    = 0b10,  // Blend with white
    BLD_BLACK    = 0b11   // Blend with black
};

enum GraphicsMode
{
    GFX_NORMAL     = 0b00,  // Normal
    GFX_ALPHA      = 0b01,  // Blend alpha
    GFX_WINDOW     = 0b10,  // Window
    GFX_PROHIBITED = 0b11   // Prohibited
};

enum PixelFormat
{
    BPP4,  // 4 bits per pixel (16/16)
    BPP8   // 8 bits per pixel (256/1)
};
