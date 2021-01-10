#pragma once

#include "base/register.h"

template<uint MaskL, uint MaskH, uint MaskX>
class Channel
{
public:
    template<uint Index> u8 readL() { return l.read<Index>(); }
    template<uint Index> u8 readH() { return h.read<Index>(); }
    template<uint Index> u8 readX() { return x.read<Index>(); }

    template<uint Index> void writeL(u8 byte) { l.write<Index>(byte); }
    template<uint Index> void writeH(u8 byte) { h.write<Index>(byte); }
    template<uint Index> void writeX(u8 byte) { x.write<Index>(byte); }

protected:
    Register<u16, MaskL> l;
    Register<u16, MaskH> h;
    Register<u16, MaskX> x;
};
