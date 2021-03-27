#pragma once

#include "base/register.h"

class DmaSrcAddress : public RegisterW<u32>
{
public:
    DmaSrcAddress(uint id);

    operator u32() const;
};

class DmaDstAddress : public RegisterW<u32>
{
public:
    DmaDstAddress(uint id);

    operator u32() const;
};

class DmaCount : public RegisterW<u16>
{
public:
    DmaCount(uint id);

    operator uint() const;
};

class DmaControl : public Register<u16>
{
public:
    enum Control
    {
        kControlIncrement,
        kControlDecrement,
        kControlFixed,
        kControlReload
    };

    enum Timing
    {
        kTimingImmediate,
        kTimingVBlank,
        kTimingHBlank,
        kTimingSpecial
    };

    DmaControl(uint id);

    void write(uint index, u8 byte);
    void setEnabled(bool enabled);

    uint dadcnt  = 0;
    uint sadcnt  = 0;
    uint repeat  = 0;
    uint word    = 0;
    uint timing  = 0;
    uint irq     = 0;
    uint enabled = 0;

private:
    const uint id;
};
