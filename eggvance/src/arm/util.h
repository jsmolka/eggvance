#pragma once

#include "arm/psr.h"

namespace util
{
    inline bool zFlag(u32 value)
    {
        return value == 0;
    }

    inline bool nFlag(u32 value)
    {
        return value >> 31;
    }

    inline bool cFlagAdd(u64 op1, u64 op2)
    {
        return (op1 + op2) > 0xFFFF'FFFF;
    }

    inline bool cFlagSub(u64 op1, u64 op2)
    {
        return op2 <= op1;
    }

    inline bool vFlagAdd(u32 op1, u32 op2, u32 res)
    {
        return ((op1 ^ res) & (~op1 ^ op2)) >> 31;
    }

    inline bool vFlagSub(u32 op1, u32 op2, u32 res)
    {
        return ((op1 ^ op2) & (~op2 ^ res)) >> 31;
    }

    inline u32 lslArm(u32 value, u32 amount, bool flags, PSR& psr)
    {
        if (amount != 0)
        {
            if (amount < 32)
            {
                if (flags) psr.c = (value << (amount - 1)) >> 31;
                value <<= amount;
            }
            else
            {
                if (flags)
                {
                    if (amount == 32)
                        psr.c = value & 0x1;
                    else
                        psr.c = 0;
                }
                value = 0;
            }
        }
        return value;
    }

    template<uint amount>
    inline u32 lslThumb(u32 value, PSR& psr)
    {
        if constexpr (amount != 0)
        {
            if (amount < 32)
            {
                psr.c = (value << (amount - 1)) >> 31;
                value <<= amount;
            }
            else
            {
                if (amount == 32)
                    psr.c = value & 0x1;
                else
                    psr.c = 0;

                value = 0;
            }
        }
        return value;
    }

    inline u32 lslThumb(u32 value, u32 amount, PSR& psr)
    {
        if (amount != 0)
        {
            if (amount < 32)
            {
                psr.c = (value << (amount - 1)) >> 31;
                value <<= amount;
            }
            else
            {
                if (amount == 32)
                    psr.c = value & 0x1;
                else
                    psr.c = 0;

                value = 0;
            }
        }
        return value;
    }

    template<bool immediate>
    inline u32 lsrArm(u32 value, u32 amount, bool flags, PSR& psr)
    {
        if (amount != 0)
        {
            if (amount < 32)
            {
                if (flags) psr.c = (value >> (amount - 1)) & 0x1;
                value >>= amount;
            }
            else
            {
                if (flags)
                {
                    if (amount == 32)
                        psr.c = value >> 31;
                    else
                        psr.c = 0;
                }
                value = 0;
            }
        }
        else if (immediate)
        {
            if (flags) psr.c = value >> 31;
            value = 0;
        }
        return value;
    }

    template<uint amount>
    inline u32 lsrThumb(u32 value, PSR& psr)
    {
        if constexpr (amount != 0)
        {
            if (amount < 32)
            {
                psr.c = (value >> (amount - 1)) & 0x1;
                value >>= amount;
            }
            else
            {
                if (amount == 32)
                    psr.c = value >> 31;
                else
                    psr.c = 0;

                value = 0;
            }
        }
        else
        {
            psr.c = value >> 31;
            value = 0;
        }
        return value;
    }

    inline u32 lsrThumb(u32 value, u32 amount, PSR& psr)
    {
        if (amount != 0)
        {
            if (amount < 32)
            {
                psr.c = (value >> (amount - 1)) & 0x1;
                value >>= amount;
            }
            else
            {
                if (amount == 32)
                    psr.c = value >> 31;
                else
                    psr.c = 0;

                value = 0;
            }
        }
        return value;
    }

    template<bool immediate>
    inline u32 asrArm(u32 value, u32 amount, bool flags, PSR& psr)
    {
        if (amount != 0)
        {
            if (amount < 32)
            {
                if (flags) psr.c = (value >> (amount - 1)) & 0x1;
                value = bits::sar(value, amount);
            }
            else
            {
                value = bits::sar(value, 31);
                if (flags) psr.c = value & 0x1;
            }
        }
        else if (immediate)
        {
            value = bits::sar(value, 31);
            if (flags) psr.c = value & 0x1;
        }
        return value;
    }

    template<u32 amount>
    inline u32 asrThumb(u32 value, PSR& psr)
    {
        if constexpr (amount != 0 && amount < 32)
        {
            psr.c = (value >> (amount - 1)) & 0x1;
            value = bits::sar(value, amount);
        }
        else
        {
            value = bits::sar(value, 31);
            psr.c = value & 0x1;
        }
        return value;
    }

    inline u32 asrThumb(u32 value, u32 amount, PSR& psr)
    {
        if (amount != 0)
        {
            if (amount < 32)
            {
                psr.c = (value >> (amount - 1)) & 0x1;
                value = bits::sar(value, amount);
            }
            else
            {
                value = bits::sar(value, 31);
                psr.c = value & 0x1;
            }
        }
        return value;
    }

    template<bool immediate>
    inline u32 rorArm(u32 value, u32 amount, bool flags, PSR& psr)
    {
        if (amount != 0)
        {
            value = bits::ror(value, amount);
            if (flags) psr.c = value >> 31;
        }
        else if (immediate)
        {
            uint c = psr.c;
            if (flags) psr.c = value & 0x1;
            value = (c << 31) | (value >> 1);
        }
        return value;
    }

    inline u32 rorThumb(u32 value, u32 amount, PSR& psr)
    {
        if (amount != 0)
        {
            value = bits::ror(value, amount);
            psr.c = value >> 31;
        }
        return value;
    }

    inline u32 log(u32 value, PSR& psr)
    {
        psr.z = zFlag(value);
        psr.n = nFlag(value);

        return value;
    }

    inline u32 log(u32 value, bool flags, PSR& psr)
    {
        if (flags)
        {
            psr.z = zFlag(value);
            psr.n = nFlag(value);
        }
        return value;
    }

    inline u32 add(u32 op1, u32 op2, PSR& psr)
    {
        u32 res = op1 + op2;

        psr.z = zFlag(res);
        psr.n = nFlag(res);
        psr.c = cFlagAdd(op1, op2);
        psr.v = vFlagAdd(op1, op2, res);

        return res;
    }

    inline u32 add(u32 op1, u32 op2, bool flags, PSR& psr)
    {
        u32 res = op1 + op2;

        if (flags)
        {
            psr.z = zFlag(res);
            psr.n = nFlag(res);
            psr.c = cFlagAdd(op1, op2);
            psr.v = vFlagAdd(op1, op2, res);
        }
        return res;
    }

    inline u32 sub(u32 op1, u32 op2, PSR& psr)
    {
        u32 res = op1 - op2;

        psr.z = zFlag(res);
        psr.n = nFlag(res);
        psr.c = cFlagSub(op1, op2);
        psr.v = vFlagSub(op1, op2, res);

        return res;
    }

    inline u32 sub(u32 op1, u32 op2, bool flags, PSR& psr)
    {
        u32 res = op1 - op2;

        if (flags)
        {
            psr.z = zFlag(res);
            psr.n = nFlag(res);
            psr.c = cFlagSub(op1, op2);
            psr.v = vFlagSub(op1, op2, res);
        }
        return res;
    }

    inline u32 adc(u32 op1, u32 op2, PSR& psr)
    {
        u64 opc = static_cast<u64>(op2) + psr.c;
        u32 res = static_cast<u32>(op1 + opc);

        psr.z = zFlag(res);
        psr.n = nFlag(res);
        psr.c = cFlagAdd(op1, opc);
        psr.v = vFlagAdd(op1, op2, res);

        return res;
    }

    inline u32 adc(u32 op1, u32 op2, bool flags, PSR& psr)
    {
        u64 opc = static_cast<u64>(op2) + psr.c;
        u32 res = static_cast<u32>(op1 + opc);

        if (flags)
        {
            psr.z = zFlag(res);
            psr.n = nFlag(res);
            psr.c = cFlagAdd(op1, opc);
            psr.v = vFlagAdd(op1, op2, res);
        }
        return res;
    }

    inline u32 sbc(u32 op1, u32 op2, PSR& psr)
    {
        u64 opc = static_cast<u64>(op2) - psr.c + 1;
        u32 res = static_cast<u32>(op1 - opc);

        psr.z = zFlag(res);
        psr.n = nFlag(res);
        psr.c = cFlagSub(op1, opc);
        psr.v = vFlagSub(op1, op2, res);

        return res;
    }

    inline u32 sbc(u32 op1, u32 op2, bool flags, PSR& psr)
    {
        u64 opc = static_cast<u64>(op2) - psr.c + 1;
        u32 res = static_cast<u32>(op1 - opc);

        if (flags)
        {
            psr.z = zFlag(res);
            psr.n = nFlag(res);
            psr.c = cFlagSub(op1, opc);
            psr.v = vFlagSub(op1, op2, res);
        }
        return res;
    }
}
