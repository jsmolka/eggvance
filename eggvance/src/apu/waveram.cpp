#include "waveram.h"

#include "base/bit.h"

u8 WaveRam::operator[](uint index) const
{
    SHELL_ASSERT(index < 64);

    return bit::nibble(data[index >> 1], index & 0x1 ^ 0x1);
}

u8 WaveRam::read(uint index) const
{
    SHELL_ASSERT(index < 16);

    return data[16 * (bank ^ 0x1) + index];
}

void WaveRam::write(uint index, u8 byte)
{
    SHELL_ASSERT(index < 16);

    data[16 * (bank ^ 0x1) + index] = byte;
}
