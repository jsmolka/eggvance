#include "blendcontrol.h"

void BlendControl::Layer::reset()
{
    bg0 = 0;
    bg1 = 0;
    bg2 = 0;
    bg3 = 0;
    obj = 0;
    bdp = 0;
}

void BlendControl::reset()
{
    mode = 0;
    upper.reset();
    lower.reset();
}
