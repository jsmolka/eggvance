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
        enabled &= sweep.enabled;
        break;

    case 2:
        length = bit::seq<0, 6>(byte);
        form   = bit::seq<6, 2>(byte);
        break;

    case 3:
        envelope.write(byte);
        enabled &= envelope.enabled();
        break;

    case 4:
        frequency = bit::seq<32, 11>(value);
        break;

    case 5:
        frequency     = bit::seq<32, 11>(value);
        length.expire = bit::seq< 6,  1>(byte);

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
