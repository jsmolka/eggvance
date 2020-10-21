#pragma once

#include "base/bit.h"
#include "base/int.h"

class Psr
{
public:
    enum Mode
    {
        kModeUsr = 0b10000,
        kModeFiq = 0b10001,
        kModeIrq = 0b10010,
        kModeSvc = 0b10011,
        kModeAbt = 0b10111,
        kModeSys = 0b11111,
        kModeUnd = 0b11011
    };

    Psr& operator=(u32 value)
    {
        m = bit::seq< 0, 5>(value);
        t = bit::seq< 5, 1>(value);
        f = bit::seq< 6, 1>(value);
        i = bit::seq< 7, 1>(value);
        v = bit::seq<28, 1>(value);
        c = bit::seq<29, 1>(value);
        z = bit::seq<30, 1>(value);
        n = bit::seq<31, 1>(value);

        return *this;
    }

    operator u32() const
    {
        return (m << 0) | (t << 5) | (f << 6) | (i << 7) | (v << 28) | (c << 29) | (z << 30) | (n << 31);
    }

    template<typename Integral>
    void setZ(Integral value)
    {
        static_assert(shell::is_any_of_v<Integral, u32, u64>);

        z = value == 0;
    }

    template<typename Integral>
    void setN(Integral value)
    {
        static_assert(shell::is_any_of_v<Integral, u32, u64>);

        n = bit::msb(value);
    }

    void setCAdd(u64 op1, u64 op2)
    {
        c = op1 + op2 > 0xFFFF'FFFFULL;
    }

    void setCSub(u64 op1, u64 op2)
    {
        c = op2 <= op1;
    }

    void setVAdd(u32 op1, u32 op2, u32 res)
    {
        v = bit::msb((op1 ^ res) & (~op1 ^ op2));
    }

    void setVSub(u32 op1, u32 op2, u32 res)
    {
        v = bit::msb((op1 ^ op2) & (~op2 ^ res));
    }

    uint size() const
    {
        return 4 >> t;
    }

    bool check(uint condition) const
    {
        enum Condition
        {
            kConditionEQ,
            kConditionNE,
            kConditionCS,
            kConditionCC,
            kConditionMI,
            kConditionPL,
            kConditionVS,
            kConditionVC,
            kConditionHI,
            kConditionLS,
            kConditionGE,
            kConditionLT,
            kConditionGT,
            kConditionLE,
            kConditionAL,
            kConditionNV
        };

        switch (condition)
        {
        case kConditionEQ: return z;
        case kConditionNE: return !z;
        case kConditionCS: return c;
        case kConditionCC: return !c;
        case kConditionMI: return n;
        case kConditionPL: return !n;
        case kConditionVS: return v;
        case kConditionVC: return !v;
        case kConditionHI: return c && !z;
        case kConditionLS: return !c || z;
        case kConditionGE: return n == v;
        case kConditionLT: return n != v;
        case kConditionGT: return !z && (n == v);
        case kConditionLE: return z || (n != v);
        case kConditionAL: return true;
        case kConditionNV: return false;

        default:
            SHELL_UNREACHABLE;
            return false;
        }
    }

    uint m = kModeSys;
    uint t = 0;
    uint f = 0;
    uint i = 0;
    uint v = 0;
    uint c = 0;
    uint z = 0;
    uint n = 0;
};
