#pragma once

#include "window.h"

struct WindowOutside
{
    union
    {
        struct
        {
            Window winout;
            Window winobj;
        };
        Window win[2];
    };
};
