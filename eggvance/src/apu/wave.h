#pragma once

#include "channel.h"
#include "length.h"

class Wave : public Channel<0x00E0, 0xE03F, 0x47FF>
{
public:
    void init();
    void tick();
    void tickLength();

    u8 readBank(uint index) const;
    void writeBank(uint index, u8 byte);

    template<uint Index> void writeL(u8 byte);
    template<uint Index> void writeH(u8 byte);
    template<uint Index> void writeX(u8 byte);

private:
    void updateTimer();

    Length<256> length;

    uint timer     = 0;
    uint nibble    = 0;
    uint two_banks = 0;
    uint bank      = 0;
    uint active    = 0;
    uint volume    = 2;
    uint frequency = 0;

    u8 ram[32] = {};
};

template<uint Index>
void Wave::writeL(u8 byte)
{
    Channel::writeL<Index>(byte);

    if (Index == 0)
    {
        two_banks = bit::seq<5, 1>(byte);
        bank      = bit::seq<6, 1>(byte);
        active    = bit::seq<7, 1>(byte);
    }
}

template<uint Index>
void Wave::writeH(u8 byte)
{
    Channel::writeH<Index>(byte);

    if (Index == 0)
    {
        length.length = byte;
    }
    if (Index == 1)
    {
        // Todo: set volume
    }
}

template<uint Index>
void Wave::writeX(u8 byte)
{
    Channel::writeX<Index>(byte);

    frequency = bit::seq<0, 11>(x.value);

    if (Index == 1)
    {
        length.expire = bit::seq<6, 1>(byte);

        if (byte & 0x80)
            init();
    }
}
