#include "bldcnt.h"

Bldcnt::Bldcnt(u16& data)
    : Register<u16>(data)
    , a_bg0(data)
    , a_bg1(data)
    , a_bg2(data)
    , a_bg3(data)
    , a_sprites(data)
    , a_backdrop(data)
    , mode(data)
    , b_bg0(data)
    , b_bg1(data)
    , b_bg2(data)
    , b_bg3(data)
    , b_sprites(data)
    , b_backdrop(data)
{

}

int Bldcnt::a_bg(int bg) const
{
    switch (bg)
    {
    case 0: return a_bg0;
    case 1: return a_bg1;
    case 2: return a_bg2;
    case 3: return a_bg3;
    }
    return 0;
}

int Bldcnt::b_bg(int bg) const
{
    switch (bg)
    {
    case 0: return b_bg0;
    case 1: return b_bg1;
    case 2: return b_bg2;
    case 3: return b_bg3;
    }
    return 0;
}
