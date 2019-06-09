#include "dispcnt.h"

#include "mmu/map.h"

Dispcnt::Dispcnt(u16& data)
    : Register<u16>(data)
    , bg_mode(data)
    , gbc_mode(data)
    , frame(data)
    , access_oam(data)
    , mapping_1d(data)
    , force_blank(data)
    , bg0(data)
    , bg1(data)
    , bg2(data)
    , bg3(data)
    , sprites(data)
    , win0(data)
    , win1(data)
    , winobj(data)
{ 

}

u32 Dispcnt::frameAddr() const
{
    return  MAP_VRAM + 0xA000 * frame;
}
