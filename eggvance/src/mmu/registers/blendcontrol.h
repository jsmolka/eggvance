#pragma once

struct BlendControl
{
    struct BlendLayer
    {
        union
        {
            struct
            {
                int bg0;  // BG0 enable
                int bg1;  // BG1 enable
                int bg2;  // BG2 enable
                int bg3;  // BG3 enable
            };
            int bg[4];
        };
        int obj;  // Objects enable
        int bdp;  // Backdrop enable
    };

    int mode;          // Mode (0 = none, 1 = alpha blending, 2 = brightness increase, 3 = brightness decrease)
    BlendLayer upper;  // Upper blend layer
    BlendLayer lower;  // Lower blend layer
};
