#pragma once

struct BlendControl
{
    union
    {
        struct
        {
            int a_bg0;  // Top layer BG0
            int a_bg1;  // Top layer BG1
            int a_bg2;  // Top layer BG2
            int a_bg3;  // Top layer BG3
        };
        int a_bg[4];
    };

    int a_obj;  // Top layer OBJ
    int a_bdp;  // Top layer backdrop
    int mode;   // Mode (0 = none, 1 = alpha blending, 2 = brightness increase, 3 = brightness decrease)
    
    union
    {
        struct
        {
            int b_bg0;  // Bottom layer BG0
            int b_bg1;  // Bottom layer BG1
            int b_bg2;  // Bottom layer BG2
            int b_bg3;  // Bottom layer BG3
        };
        int b_bg[4];
    };
    int b_obj;  // Bottom layer OBJ
    int b_bdp;  // Bottom layer backdrop
};
