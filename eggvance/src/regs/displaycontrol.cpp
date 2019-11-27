#include "displaycontrol.h"

#include "common/config.h"
#include "common/macros.h"
#include "common/utility.h"

void DisplayControl::reset()
{
    *this = {};

    force_blank = config.bios_skip;

    update();
}

u8 DisplayControl::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return data[index];
}

void DisplayControl::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        mode        = bits<0, 3>(byte);
        frame       = bits<4, 1>(byte);
        oam_hblank  = bits<5, 1>(byte);
        obj_mapping = bits<6, 1>(byte);
        force_blank = bits<7, 1>(byte);
    }
    else
    {
        layers = bits<0, 5>(byte);
        win0   = bits<5, 1>(byte);
        win1   = bits<6, 1>(byte);
        winobj = bits<7, 1>(byte);
    }
    data[index] = byte;

    update();
}

bool DisplayControl::isActive() const
{
    static constexpr int masks[8] = {
        0b11111, 
        0b10111, 
        0b11100, 
        0b10100,
        0b10100, 
        0b10100, 
        0b00000, 
        0b00000
    };
    return layers & masks[mode];
}

bool DisplayControl::isBitmap() const
{
    return mode > 2;
}

void DisplayControl::update()
{
    base_frame = 0xA000 * frame;
}
