#include "arm.h"

u32 Arm::lsl(u32 value, u32 amount, bool flags)
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
u32 Arm::lsr(u32 value, u32 amount, bool flags)
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

template u32 Arm::lsr<true> (u32, u32, bool);
template u32 Arm::lsr<false>(u32, u32, bool);

template<bool immediate>
u32 Arm::asr(u32 value, u32 amount, bool flags)
{
    if (amount != 0)
    {
        if (amount < 32)
        {
            if (flags) cpsr.c = (value >> (amount - 1)) & 0x1;
            value = bit::sar(value, amount);
        }
        else
        {
            value = bit::sar(value, 31);
            if (flags) cpsr.c = value & 0x1;
        }
    }
    else if (immediate)
    {
        value = bit::sar(value, 31);
        if (flags) cpsr.c = value & 0x1;
    }
    return value;
}

template u32 Arm::asr<true> (u32, u32, bool);
template u32 Arm::asr<false>(u32, u32, bool);

template<bool immediate>
u32 Arm::ror(u32 value, u32 amount, bool flags)
{
    if (amount != 0)
    {
        value = bit::ror(value, amount);
        if (flags) cpsr.c = value >> 31;
    }
    else if (immediate)
    {
        uint c = cpsr.c;
        if (flags) cpsr.c = value & 0x1;
        value = (c << 31) | (value >> 1);
    }
    return value;
}

template u32 Arm::ror<true> (u32, u32, bool);
template u32 Arm::ror<false>(u32, u32, bool);

template<typename T>
T Arm::log(T value, bool flags)
{
    if (flags)
    {
        cpsr.setZ(value);
        cpsr.setN(value);
    }
    return value;
}

template u32 Arm::log<u32>(u32, bool);
template u64 Arm::log<u64>(u64, bool);

u32 Arm::add(u32 op1, u32 op2, bool flags)
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

u32 Arm::sub(u32 op1, u32 op2, bool flags)
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

u32 Arm::adc(u32 op1, u32 op2, bool flags)
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

u32 Arm::sbc(u32 op1, u32 op2, bool flags)
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
