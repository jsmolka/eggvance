#include "arm.h"

constexpr int zero(u32 value)
{
    return value == 0;
}

constexpr int sign(u32 value)
{
    return value >> 31;
}

constexpr int carryAdd(u64 op1, u64 op2)
{
    return (op1 + op2) > 0xFFFF'FFFF;
}

constexpr int carrySub(u64 op1, u64 op2)
{
    return op2 <= op1;
}

constexpr int overflowAdd(u32 op1, u32 op2, u32 res)
{
    return sign(op1) == sign(op2)
        && sign(op1) != sign(res);
}

constexpr int overflowSub(u32 op1, u32 op2, u32 res)
{
    return sign(op2) != sign(op1)
        && sign(op2) == sign(res);
}

u32 ARM::logical(u32 value, bool flags)
{
    if (flags)
    {
        cpsr.z = zero(value);
        cpsr.n = sign(value);
    }
    return value;
}

u32 ARM::logical(u32 value, bool carry, bool flags)
{
    if (flags)
    {
        cpsr.z = zero(value);
        cpsr.n = sign(value);
        cpsr.c = carry;
    }
    return value;
}

u32 ARM::add(u32 op1, u32 op2, bool flags)
{
    u32 res = op1 + op2;

    if (flags)
    {
        cpsr.z = zero(res);
        cpsr.n = sign(res);
        cpsr.c = carryAdd(op1, op2);
        cpsr.v = overflowAdd(op1, op2, res);
    }
    return res;
}

u32 ARM::sub(u32 op1, u32 op2, bool flags)
{
    u32 res = op1 - op2;

    if (flags)
    {
        cpsr.z = zero(res);
        cpsr.n = sign(res);
        cpsr.c = carrySub(op1, op2);
        cpsr.v = overflowSub(op1, op2, res);
    }
    return res;
}

u32 ARM::adc(u32 op1, u32 op2, bool flags)
{
    u64 opc = static_cast<u64>(op2) + cpsr.c;
    u32 res = static_cast<u32>(op1 + opc);

    if (flags)
    {
        cpsr.z = zero(res);
        cpsr.n = sign(res);
        cpsr.c = carryAdd(op1, opc);
        cpsr.v = overflowAdd(op1, op2, res);
    }
    return res;
}

u32 ARM::sbc(u32 op1, u32 op2, bool flags)
{
    u64 opc = static_cast<u64>(op2) - cpsr.c + 1;
    u32 res = static_cast<u32>(op1 - opc);

    if (flags)
    {
        cpsr.z = zero(res);
        cpsr.n = sign(res);
        cpsr.c = carrySub(op1, opc);
        cpsr.v = overflowSub(op1, op2, res);
    }
    return res;
}
