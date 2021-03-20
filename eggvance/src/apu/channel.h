#pragma once

#include "envelope.h"
#include "length.h"
#include "sweep.h"
#include "base/register.h"

class Channel : public Register<u64>
{
public:
    Channel(u64 mask, uint length);

    virtual void tick() = 0;

    void tickSweep();
    void tickLength();
    void tickEnvelope();

    uint sample    = 0;
    uint enabled   = 0;
    uint frequency = 0;

protected:
    virtual uint period() const = 0;

    void init(bool enabled);
    void initSweep();
    void initEnvelope();

    uint run();
    void write(uint index, u8 byte);

    Sweep sweep;
    Length length;
    Envelope envelope;

private:
    void doSweep(bool writeback);

    u64 timer = 0;
    u64 since = 0;
};
