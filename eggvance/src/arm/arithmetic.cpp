#include "arm.h"

#pragma warning(push)
#pragma warning(disable:4244)

constexpr int zero(u32 value)
{
    return value == 0;
}

constexpr int sign(u32 value)
{
    return value >> 31;
}

constexpr int carryAdd(u32 op1, u32 op2)
{
    return op2 > (0xFFFF'FFFF - op1);
}

constexpr int carrySub(u32 op1, u32 op2)
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
    return sign(op1) != sign(op2)
        && sign(res) == sign(op2);
}

u32 ARM::logical(u32 result, bool flags)
{
    if (flags)
    {
        cpsr.z = zero(result);
        cpsr.n = sign(result);
    }
    return result;
}

u32 ARM::logical(u32 result, bool carry, bool flags)
{
    if (flags)
    {
        cpsr.z = zero(result);
        cpsr.n = sign(result);
        cpsr.c = carry;
    }
    return result;
}

u32 ARM::add(u32 op1, u32 op2, bool flags)
{
    u32 result = op1 + op2;

    if (flags)
    {
        cpsr.z = zero(result);
        cpsr.n = zero(result);
        cpsr.c = carryAdd(op1, op2);
        cpsr.v = overflowAdd(op1, op2, result);
    }
    return result;
}

u32 ARM::sub(u32 op1, u32 op2, bool flags)
{
    u32 result = op1 - op2;

    if (flags)
    {
        cpsr.z = zero(result);
        cpsr.n = sign(result);
        cpsr.c = carrySub(op1, op2);
        cpsr.v = overflowSub(op1, op2, result);
    }
    return result;
}

u32 ARM::adc(u64 op1, u64 op2, bool flags)
{
    u64 opc = op2 + cpsr.c;

    u32 result = static_cast<u32>(op1 + opc);

    if (flags)
    {
        cpsr.z = zero(result);
        cpsr.n = sign(result);
        cpsr.c = carryAdd(op1, opc);
        cpsr.v = overflowAdd(op1, op2, result);
    }
    return result;
}

u32 ARM::sbc(u64 op1, u64 op2, bool flags)
{
    u64 opc = op2 - cpsr.c + 1;

    u32 result = static_cast<u32>(op1 - opc);

    if (flags)
    {
        cpsr.z = zero(result);
        cpsr.n = sign(result);
        cpsr.c = carrySub(op1, opc);
        cpsr.v = overflowSub(op1, op2, result);
    }
    return result;
}

#pragma warning(pop)
