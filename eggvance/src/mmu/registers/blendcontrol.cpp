#include "blendcontrol.h"

#include "ppu/layers.h"

void BlendControl::Layer::reset()
{
    bg0   = 0;
    bg1   = 0;
    bg2   = 0;
    bg3   = 0;
    obj   = 0;
    bdp   = 0;
    flags = 0;
}

void BlendControl::reset()
{
    mode = 0;
    upper.reset();
    lower.reset();
}

void BlendControl::Layer::updateFlags()
{
    flags = 0;
    if (bg0) flags |= LF_BG0;
    if (bg1) flags |= LF_BG1;
    if (bg2) flags |= LF_BG2;
    if (bg3) flags |= LF_BG3;
    if (obj) flags |= LF_OBJ;
    if (bdp) flags |= LF_BDP;
}
