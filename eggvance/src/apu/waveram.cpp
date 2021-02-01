#include "waveram.h"

u8 WaveRam::operator[](uint index) const
{
    return data[16 * bank + index];    
}

u8 WaveRam::read(uint index) const
{
    return data[16 * (bank ^ 0x1) + index];
}

void WaveRam::write(uint index, u8 byte)
{
    data[16 * (bank ^ 0x1) + index] = byte;
}
