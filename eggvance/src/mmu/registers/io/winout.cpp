#include "winout.h"

Winout::Winout(u16& data)
    : Register<u16>(data)
    , winout_bg0(data)
    , winout_bg1(data)
    , winout_bg2(data)
    , winout_bg3(data)
    , winout_obj(data)
    , winout_sfx(data)
    , winobj_bg0(data)
    , winobj_bg1(data)
    , winobj_bg2(data)
    , winobj_bg3(data)
    , winobj_obj(data)
    , winobj_sfx(data)
{

}
