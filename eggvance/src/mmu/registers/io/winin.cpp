#include "winin.h"

Winin::Winin(u16& data)
    : Register<u16>(data)
    , win0_bg0(data)
    , win0_bg1(data)
    , win0_bg2(data)
    , win0_bg3(data)
    , win0_obj(data)
    , win0_sfx(data)
    , win1_bg0(data)
    , win1_bg1(data)
    , win1_bg2(data)
    , win1_bg3(data)
    , win1_obj(data)
    , win1_sfx(data)
{

}
