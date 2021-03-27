#pragma once

#include <shell/macros.h>

#include "base/int.h"

class Psr
{
public:
    enum class Mode
    {
        Usr = 0b10000,
        Fiq = 0b10001,
        Irq = 0b10010,
        Svc = 0b10011,
        Abt = 0b10111,
        Sys = 0b11111,
        Und = 0b11011
    };

    Psr& operator=(u32 value);
    operator u32() const;

    uint size() const;
    void setZ(u32 value);
    void setN(u32 value);
    void setCAdd(u64 op1, u64 op2);
    void setCSub(u64 op1, u64 op2);
    void setVAdd(u32 op1, u32 op2, u32 res);
    void setVSub(u32 op1, u32 op2, u32 res);

    SHELL_INLINE bool check(uint condition) const
    {
        enum class Condition
        { 
            EQ, NE, CS, CC, MI, PL, VS, VC,
            HI, LS, GE, LT, GT, LE, AL, NV
        };

        switch (Condition(condition))
        {
        case Condition::EQ: return z;
        case Condition::NE: return !z;
        case Condition::CS: return c;
        case Condition::CC: return !c;
        case Condition::MI: return n;
        case Condition::PL: return !n;
        case Condition::VS: return v;
        case Condition::VC: return !v;
        case Condition::HI: return c && !z;
        case Condition::LS: return !c || z;
        case Condition::GE: return n == v;
        case Condition::LT: return n != v;
        case Condition::GT: return !z && (n == v);
        case Condition::LE: return z || (n != v);
        case Condition::AL: return true;
        case Condition::NV: return false;

        default:
            SHELL_UNREACHABLE;
            return false;
        }
    }

    Mode m = Mode::Svc;
    uint t = 0;
    uint f = 0;
    uint i = 0;
    uint v = 0;
    uint c = 0;
    uint z = 0;
    uint n = 0;
};
