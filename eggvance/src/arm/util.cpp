#include "arm.h"

u32 ARM::lsl(u32 value, u32 amount, bool flags)
{
    if (amount != 0)
    {
        if (amount < 32)
        {
            if (flags) cpsr.c = (value << (amount - 1)) >> 31;
            value <<= amount;
        }
        else
        {
            if (flags)
            {
                if (amount == 32)
                    cpsr.c = value & 0x1;
                else
                    cpsr.c = 0;
            }
            value = 0;
        }
    }
    return value;
}

template<bool immediate>
u32 ARM::lsr(u32 value, u32 amount, bool flags)
{
    if (amount != 0)
    {
        if (amount < 32)
        {
            if (flags) cpsr.c = (value >> (amount - 1)) & 0x1;
            value >>= amount;
        }
        else
        {
            if (flags)
            {
                if (amount == 32)
                    cpsr.c = value >> 31;
                else
                    cpsr.c = 0;
            }
            value = 0;
        }
    }
    else if (immediate)
    {
        if (flags) cpsr.c = value >> 31;
        value = 0;
    }
    return value;
}

template u32 ARM::lsr<true> (u32, u32, bool);
template u32 ARM::lsr<false>(u32, u32, bool);

template<bool immediate>
u32 ARM::asr(u32 value, u32 amount, bool flags)
{
    if (amount != 0)
    {
        if (amount < 32)
        {
            if (flags) cpsr.c = (value >> (amount - 1)) & 0x1;
            value = bits::sar(value, amount);
        }
        else
        {
            value = bits::sar(value, 31);
            if (flags) cpsr.c = value & 0x1;
        }
    }
    else if (immediate)
    {
        value = bits::sar(value, 31);
        if (flags) cpsr.c = value & 0x1;
    }
    return value;
}

template u32 ARM::asr<true> (u32, u32, bool);
template u32 ARM::asr<false>(u32, u32, bool);

template<bool immediate>
u32 ARM::ror(u32 value, u32 amount, bool flags)
{
    if (amount != 0)
    {
        value = bits::ror(value, amount);
        if (flags) cpsr.c = value >> 31;
    }
    else if (immediate)
    {
        uint c = cpsr.c;
        if (flags) cpsr.c = value & 0x1;
        value = (c << 31) | (value >> 1);
    }
    return value;}

template u32 ARM::ror<true> (u32, u32, bool);
template u32 ARM::ror<false>(u32, u32, bool);

template<typename T>
T ARM::log(T value, bool flags)
{
    if (flags)
    {
        cpsr.setZ(value);
        cpsr.setN(value);
    }
    return value;
}

template u32 ARM::log<u32>(u32, bool);
template u64 ARM::log<u64>(u64, bool);

u32 ARM::add(u32 op1, u32 op2, bool flags)
{
    u32 res = op1 + op2;

    if (flags)
    {
        cpsr.setZ(res);
        cpsr.setN(res);
        cpsr.setCAdd(op1, op2);
        cpsr.setVAdd(op1, op2, res);
    }
    return res;
}

u32 ARM::sub(u32 op1, u32 op2, bool flags)
{
    u32 res = op1 - op2;

    if (flags)
    {
        cpsr.setZ(res);
        cpsr.setN(res);
        cpsr.setCSub(op1, op2);
        cpsr.setVSub(op1, op2, res);
    }
    return res;
}

u32 ARM::adc(u32 op1, u32 op2, bool flags)
{
    u64 opc = static_cast<u64>(op2) + cpsr.c;
    u32 res = static_cast<u32>(op1 + opc);

    if (flags)
    {
        cpsr.setZ(res);
        cpsr.setN(res);
        cpsr.setCAdd(op1, opc);
        cpsr.setVAdd(op1, op2, res);
    }
    return res;
}

u32 ARM::sbc(u32 op1, u32 op2, bool flags)
{
    u64 opc = static_cast<u64>(op2) - cpsr.c + 1;
    u32 res = static_cast<u32>(op1 - opc);

    if (flags)
    {
        cpsr.setZ(res);
        cpsr.setN(res);
        cpsr.setCSub(op1, opc);
        cpsr.setVSub(op1, op2, res);
    }
    return res;
}
