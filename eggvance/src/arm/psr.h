#pragma once

#include "base/bits.h"
#include "base/macros.h"

class PSR
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

    PSR& operator=(u32 value)
    {
        m = bits::seq< 0, 5>(value);
        t = bits::seq< 5, 1>(value);
        f = bits::seq< 6, 1>(value);
        i = bits::seq< 7, 1>(value);
        v = bits::seq<28, 1>(value);
        c = bits::seq<29, 1>(value);
        z = bits::seq<30, 1>(value);
        n = bits::seq<31, 1>(value);

        return *this;
    }

    operator u32() const
    {
        return (m << 0) | (t << 5) | (f << 6) | (i << 7) | (v << 28) | (c << 29) | (z << 30) | (n << 31);
    }

    inline uint size() const
    {
        return 2 << (static_cast<uint>(t) ^ 0x1);
    }

    bool check(uint condition) const
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
            UNREACHABLE;
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

private:
    enum Condition
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
