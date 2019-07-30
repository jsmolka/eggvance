#pragma once

struct BlendControl
{
    struct BlendLayer
    {
        union
        {
            struct
            {
                int bg0;  // BG0 considered
                int bg1;  // BG1 considered
                int bg2;  // BG2 considered
                int bg3;  // BG3 considered
            };
            int bg[4];
        };
        int obj;  // Objects considered
        int bdp;  // Backdrop considered
    };
    BlendLayer upper;  // Upper blend layer
    BlendLayer lower;  // Lower blend layer
    int mode;          // Mode (0 = none, 1 = alpha blending, 2 = brightness increase, 3 = brightness decrease)
};
