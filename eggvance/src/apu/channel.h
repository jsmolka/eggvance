#pragma once

#include "envelope.h"
#include "length.h"
#include "sweep.h"
#include "base/register.h"

class Channel : public XRegister<u64>
{
public:
    Channel(u64 mask, uint base);

    void tickSweep();
    void tickLength();
    void tickEnvelope();

    uint timer     = 0;
    uint sample    = 0;
    uint enabled   = 0;
    uint frequency = 0;

protected:
    void doSweep(bool writeback);

    virtual uint period() const = 0;

    Sweep sweep;
    Length length;
    Envelope envelope;
};
