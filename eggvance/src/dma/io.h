#pragma once

#include "base/register.h"

class DmaAddress : public RegisterW<u32>
{
public:
    using RegisterW::RegisterW;

    operator u32() const;

    bool isFifo() const;
    bool isGamePak() const;
};

class DmaSource : public DmaAddress
{
public:
    DmaSource(uint id);
};

class DmaDestination : public DmaAddress
{
public:
    DmaDestination(uint id);
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
