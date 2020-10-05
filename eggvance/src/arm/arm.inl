#pragma once

template<typename Integral>
Integral Arm::log(Integral value, bool flags)
{
    static_assert(shell::is_any_of_v<Integral, u32, u64>);

    if (flags)
    {
        cpsr.setZ(value);
        cpsr.setN(value);
    }
    return value;
}

template<bool Immediate>
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

template<bool Immediate>
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
    else if (Immediate)
    {
        if (flags) cpsr.c = value >> 31;
        value = 0;
    }
    return value;
}

template<bool Immediate>
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
    else if (Immediate)
    {
        value = bit::sar(value, 31);
        if (flags) cpsr.c = value & 0x1;
    }
    return value;
}

template<bool Immediate>
u32 Arm::ror(u32 value, u32 amount, bool flags)
{
    if (amount != 0)
    {
        value = bit::ror(value, amount);
        if (flags) cpsr.c = value >> 31;
    }
    else if (Immediate)
    {
        uint c = cpsr.c;
        if (flags) cpsr.c = value & 0x1;
        value = (c << 31) | (value >> 1);
    }
    return value;
}

template<bool Signed>
void Arm::booth(u32 multiplier)
{
    for (s32 mask = 0xFF00'0000; mask != 0xFFFF'FFFF; mask >>= 8)
    {
        u32 bits = multiplier & mask;
        if (bits == 0 || (Signed && bits == mask))
            cycles++;
        else
            break;
    }
    cycles -= 4;
}
