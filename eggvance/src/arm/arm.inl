#pragma once

#include "common/macros.h"
#include "arm.h"

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
    // Todo: actually implement
    // Check if last cycle was on same page, use previous result if possible

    cycles += 3;
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
