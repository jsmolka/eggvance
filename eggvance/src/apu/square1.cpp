#include "square1.h"

#include "constants.h"

Square1::Square1()
    : Square(0x0000'4000'FFC0'007F)
{

}

void Square1::init()
{
    Square::init();
    
    sweep.init(frequency);
    if (sweep.shift)
        doSweep(false);
}

void Square1::write(uint index, u8 byte)
{
    Channel::write(index, byte);

    switch (index)
    {
    case NR::k10:
        sweep.write(byte);
        break;

    case NR::k11:
        length  = seq<16, 6>();
        pattern = seq<22, 2>();
        break;

    case NR::k12:
        envelope.write(byte);
        enabled &= envelope.enabled();
        break;

    case NR::k13:
        frequency = seq<32, 11>();
        break;

    case NR::k14:
        frequency     = seq<32, 11>();
        length.expire = seq<46,  1>();

        if (byte & 0x80)
            init();
        break;
    }
}
