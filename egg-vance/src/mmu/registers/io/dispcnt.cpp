#include "dispcnt.h"

Dispcnt::Dispcnt(u16& data)
    : bg_mode(data)
    , gbc_mode(data)
    , frame_select(data)
    , process_hblank(data)
    , obj_mapping(data)
    , force_blank(data)
    , bg0_enable(data)
    , bg1_enable(data)
    , bg2_enable(data)
    , bg3_enable(data)
    , obj_enable(data)
    , win0_enable(data)
    , win1_enable(data)
    , objwin_enable(data)
{

}
