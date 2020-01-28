#pragma once

#include "common/bits.h"
#include "common/integer.h"
#include "common/macros.h"

class PSR
{
public:
    enum Mode : uint
    {
        kModeUsr = 0b10000,
        kModeFiq = 0b10001,
        kModeIrq = 0b10010,
        kModeSvc = 0b10011,
        kModeAbt = 0b10111,
        kModeSys = 0b11111,
        kModeUnd = 0b11011
    };

    inline PSR& operator=(u32 value)
    {
        mode = bits< 0, 5>(value);
        t    = bits< 5, 1>(value);
        f    = bits< 6, 1>(value);
        i    = bits< 7, 1>(value);
        v    = bits<28, 1>(value);
        c    = bits<29, 1>(value);
        z    = bits<30, 1>(value);
        n    = bits<31, 1>(value);

        return *this;
    }

    inline operator u32() const
    {
        return mode
            | (t <<  5)
            | (f <<  6)
            | (i <<  7)
            | (v << 28)
            | (c << 29)
            | (z << 30)
            | (n << 31);
    }

    inline uint size() const
    {
        return 2 << (t ^ 0x1);
    }

    inline bool check(uint condition) const
    {
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
            EGG_UNREACHABLE;
            return false;
        }
    }

    uint mode;
    uint t;
    uint f;
    uint i;
    uint v;
    uint c;
    uint z;
    uint n;

private:
    enum Condition : uint
    {
        kConditionEQ = 0x0,
        kConditionNE = 0x1,
        kConditionCS = 0x2,
        kConditionCC = 0x3,
        kConditionMI = 0x4,
        kConditionPL = 0x5,
        kConditionVS = 0x6,
        kConditionVC = 0x7,
        kConditionHI = 0x8,
        kConditionLS = 0x9,
        kConditionGE = 0xA,
        kConditionLT = 0xB,
        kConditionGT = 0xC,
        kConditionLE = 0xD,
        kConditionAL = 0xE,
        kConditionNV = 0xF
    };
};
