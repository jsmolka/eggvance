#include "square2.h"

#include "constants.h"

Square2::Square2()
    : Square(0x0000'4000'0000'FFC0)
{

}

void Square2::write(uint index, u8 byte)
{
    Channel::write(index, byte);

    enum NR { k21 = 0, k22 = 1, k23 = 4, k24 = 5 };

    switch (index)
    {
    case NR::k21:
        length = seq<0, 6>();
        form   = seq<6, 2>();
        break;

    case NR::k22:
        envelope.write(byte);
        enabled &= envelope.enabled();
        break;

    case NR::k23:
        frequency = seq<32, 11>();
        break;

    case NR::k24:
        frequency     = seq<32, 11>();
        length.expire = seq<46,  1>();

        if (byte & 0x80)
            init();
        break;
    }
}
