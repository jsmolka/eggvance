#include "dispcnt.h"

#include "common/memory_map.h"

Dispcnt::Dispcnt(u16& data)
    : data(data)
    , bg_mode(data)
    , gbc_mode(data)
    , frame_select(data)
    , process_hblank(data)
    , obj_mapping(data)
    , force_blank(data)
    , bg0(data)
    , bg1(data)
    , bg2(data)
    , bg3(data)
    , obj(data)
    , win0(data)
    , win1(data)
    , objwin(data)
{

}

u32 Dispcnt::frameAddr() const
{
    return  MAP_VRAM + 0xA000 * frame_select;
}