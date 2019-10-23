#include "common/macros.h"

u32 ARM::logical(u32 result, bool flags)
{
    if (flags)
    {
        cpsr.z = result == 0;
        cpsr.n = result >> 31;
    }
    return result;
}

u32 ARM::logical(u32 result, bool carry, bool flags)
{
    if (flags)
    {
        cpsr.z = result == 0;
        cpsr.n = result >> 31;
        cpsr.c = carry;
    }
    return result;
}

u32 ARM::add(u32 op1, u32 op2, bool flags)
{
    u32 result = op1 + op2;

    if (flags)
    {
        int msb_op1 = op1 >> 31;
        int msb_op2 = op2 >> 31;

        cpsr.z = result == 0;
        cpsr.n = result >> 31;
        cpsr.c = op2 > (0xFFFFFFFF - op1);
        cpsr.v = msb_op1 == msb_op2 && (result >> 31) != msb_op1;
    }
    return result;
}

u32 ARM::sub(u32 op1, u32 op2, bool flags)
{
    u32 result = op1 - op2;

    if (flags)
    {
        int msb_op1 = op1 >> 31;
        int msb_op2 = op2 >> 31;

        cpsr.z = result == 0;
        cpsr.n = result >> 31;
        cpsr.c = op2 <= op1;
        cpsr.v = msb_op1 != msb_op2 && (result >> 31) == msb_op2;
    }
    return result;
}

template<ARM::State state>
inline void ARM::advance()
{
    switch (state)
    {
    case State::Arm: 
        pc += 4;
        break;

    case State::Thumb:
        pc += 2;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

inline void ARM::advance()
{
    static constexpr u32 offsets[2] = { 4, 2 };

    pc += offsets[cpsr.thumb];
}

template<ARM::State state>
inline void ARM::refill()
{
    switch (state)
    {
    case State::Arm:
        cycle<Access::Seq>(pc);
        cycle<Access::Seq>(pc + 4);
        break;

    case State::Thumb:
        cycle<Access::Seq>(pc);
        cycle<Access::Seq>(pc + 2);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    advance<state>();
}

template<ARM::Access access>
inline void ARM::cycle(u32 addr)
{
    cycles++;

    int page = addr >> 28;

    cycles += io.waitcnt.cycles32[static_cast<int>(access)][page];

    if (page >= 0x5 && page < 0x8 && !ppu.io.dispstat.vblank && !ppu.io.dispstat.hblank)
        cycles++;
}

inline void ARM::cycle()
{
    cycles++;
}

inline void ARM::cycleBooth(u32 multiplier, bool allow_ones)
{
    static constexpr u32 masks[3] =
    {
        0xFF00'0000,
        0xFFFF'0000,
        0xFFFF'FF00
    };

    int internal = 4;
    for (u32 mask : masks)
    {
        u32 bits = multiplier & mask;
        if (bits == 0 || (allow_ones && bits == mask))
            internal--;
        else
            break;
    }
    cycles += internal;
}
