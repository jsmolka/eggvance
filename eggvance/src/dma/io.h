#pragma once

#include <functional>

#include "base/register.h"

class DmaSource : public XRegisterW<u32>
{
public:
    DmaSource(uint id)
        : XRegisterW(id == 0 ? 0x07FF'FFFF : 0x0FFF'FFFF)
    {

    }

    operator u32() const
    {
        return value;
    }

    void write(uint index, u8 byte)
    {
        XRegisterW::write(index, byte);

        value &= mask;
    }
};

class DmaDestination : public XRegisterW<u32>
{
public:
    DmaDestination(uint id)
        : XRegisterW(id == 3 ? 0x0FFF'FFFF : 0x07FF'FFFF)
    {

    }

    operator u32() const
    {
        return value;
    }

    void write(uint index, u8 byte)
    {
        XRegisterW::write(index, byte);

        value &= mask;
    }
};

class DmaCount : public XRegisterW<u16>
{
public:
    DmaCount(uint id)
        : XRegisterW(id == 3 ? 0xFFFF : 0x3FFF)
    {

    }

    void write(uint index, u8 byte)
    {
        XRegisterW::write(index, byte);

        value &= mask;
    }

    operator uint() const
    {
        if (value == 0)
            return mask + 1;
        else
            return value;
    }
};

class DmaControl : public XRegister<u16>
{
public:
    enum Control
    {
        kIncrement,
        kDecrement,
        kFixed,
        kReload
    };

    enum Timing
    {
        kImmediate,
        kVBlank,
        kHBlank,
        kSpecial
    };

    DmaControl(uint id)
        : XRegister(id == 3 ? 0xFFE0 : 0xF7E0)
    {

    }

    void write(uint index, u8 byte)
    {
        // Todo: return here?, use something better than value?
        XRegister::write(index, byte);

        value &= mask;

        sadcnt = bit::seq<7, 2>(value);

        if (index == 0)
        {
            dadcnt = bit::seq<5, 2>(byte);
        }
        else
        {
            uint was_enable = enable;

            repeat = bit::seq< 9, 1>(value);
            word   = bit::seq<10, 1>(value);
            drq    = bit::seq<11, 1>(value);
            timing = bit::seq<12, 2>(value);
            irq    = bit::seq<14, 1>(value);
            enable = bit::seq<15, 1>(value);

            on_write(!was_enable && enable);
        }
    }

    uint dadcnt = 0;
    uint sadcnt = 0;
    uint repeat = 0;
    uint word   = 0;
    uint drq    = 0;
    uint timing = 0;
    uint irq    = 0;
    uint enable = 0;

    std::function<void(bool)> on_write;
};
