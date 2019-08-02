#pragma once

#include "window.h"

struct WindowInside
{
    union
    {
        struct
        {
            Window win0;
            Window win1;
        };
        Window win[2];
    };
};
