#include "bldcnt.h"

Bldcnt::Bldcnt(u16& data)
    : Register<u16>(data)
    , a_bg0(data)
    , a_bg1(data)
    , a_bg2(data)
    , a_bg3(data)
    , a_obj(data)
    , a_bd(data)
    , mode(data)
    , b_bg0(data)
    , b_bg1(data)
    , b_bg2(data)
    , b_bg3(data)
    , b_obj(data)
    , b_bd(data)
{

}