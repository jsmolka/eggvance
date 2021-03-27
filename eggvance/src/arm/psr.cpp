#include "psr.h"

#include <shell/operators.h>

#include "base/bit.h"

Psr& Psr::operator=(u32 value)
{
    m = Mode(bit::seq< 0, 5>(value));
    t = bit::seq< 5, 1>(value);
    f = bit::seq< 6, 1>(value);
    i = bit::seq< 7, 1>(value);
    v = bit::seq<28, 1>(value);
    c = bit::seq<29, 1>(value);
    z = bit::seq<30, 1>(value);
    n = bit::seq<31, 1>(value);

    return *this;
}

Psr::operator u32() const
{
    return 0 
        | (m <<  0)
        | (t <<  5)
        | (f <<  6)
        | (i <<  7)
        | (v << 28)
        | (c << 29)
        | (z << 30)
        | (n << 31);
}

uint Psr::size() const
{
    return 4 >> t;
}

void Psr::setZ(u32 value)
{
    z = value == 0;
}

void Psr::setN(u32 value)
{
    n = bit::msb(value);
}

void Psr::setCAdd(u64 op1, u64 op2)
{
    c = op1 + op2 > 0xFFFF'FFFF;
}

void Psr::setCSub(u64 op1, u64 op2)
{
    c = op2 <= op1;
}

void Psr::setVAdd(u32 op1, u32 op2, u32 res)
{
    v = bit::msb((op1 ^ res) & (~op1 ^ op2));
}

void Psr::setVSub(u32 op1, u32 op2, u32 res)
{
    v = bit::msb((op1 ^ op2) & (~op2 ^ res));
}
