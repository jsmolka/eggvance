#pragma once

#include "base/register.h"

template<uint MaskL, uint MaskH, uint MaskX>
class Channel
{
public:
    template<uint Index> u8 readL() { return l.template read<Index>(); }
    template<uint Index> u8 readH() { return h.template read<Index>(); }
    template<uint Index> u8 readX() { return x.template read<Index>(); }

    template<uint Index> void writeL(u8 byte) { l.template write<Index>(byte); }
    template<uint Index> void writeH(u8 byte) { h.template write<Index>(byte); }
    template<uint Index> void writeX(u8 byte) { x.template write<Index>(byte); }

    uint timer   = 0;
    uint sample  = 0;
    uint enabled = 0;

protected:
    Register<u16, MaskL> l;
    Register<u16, MaskH> h;
    Register<u16, MaskX> x;
};
