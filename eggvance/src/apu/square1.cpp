#include "square1.h"

Square1::Square1()
    : Square(0x0000'4000'FFC0'007F)
{

}

void Square1::write(uint index, u8 byte)
{
    Channel::write(index, byte);

    switch (index)
    {
    case 0:
        sweep.write(byte);
        break;

    case 2:
        length = seq<16, 6>();
        form   = seq<22, 2>();
        break;

    case 3:
        envelope.write(byte);
        enabled &= envelope.enabled();
        break;

    case 4:
        frequency = seq<32, 11>();
        break;

    case 5:
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
