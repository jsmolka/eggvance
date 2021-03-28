#pragma once

#include "base/register.h"

class DmaSrcAddress : public RegisterW<u32>
{
public:
    DmaSrcAddress(uint id);

    operator uint() const;
};

class DmaDstAddress : public RegisterW<u32>
{
public:
    DmaDstAddress(uint id);

    operator uint() const;
};

class DmaCount : public RegisterW<u16>
{
public:
    DmaCount(uint id);

    operator uint() const;
};

class DmaChannel;

class DmaControl : public Register<u16>
{
public:
    enum class Control
    {
        Increment,
        Decrement,
        Fixed,
        Reload
    };

    enum class Timing
    {
        Immediate,
        VBlank,
        HBlank,
        Special
    };

    DmaControl(DmaChannel& channel);

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
    DmaChannel& channel;
};
