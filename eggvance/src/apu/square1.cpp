#include "square1.h"

Square1::Square1()
    : Square(0x0000'4000'FFC0'007F)
{

}

void Square1::write(uint index, u8 byte)
{
    Channel::write(index, byte);

    enum NR { k10 = 0, k11 = 2, k12 = 3, k13 = 4, k14 = 5 };

    switch (index)
    {
    case NR::k10:
        sweep.write(byte);
        break;

    case NR::k11:
        length = seq<16, 6>();
        form   = seq<22, 2>();
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

void Square1::init()
{
    Square::init();

    initSweep();
}
