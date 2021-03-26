#include "arm.h"

#include "constants.h"

enum ExceptionVector
{
    kVectorReset         = 0x00,
    kVectorUndefined     = 0x04,
    kVectorSwi           = 0x08,
    kVectorAbortPrefetch = 0x0C,
    kVectorAbortData     = 0x10,
    kVectorReserved      = 0x14,
    kVectorIrq           = 0x18,
    kVectorFiq           = 0x1C
};

void Arm::raise(uint irq, u64 late)
{
    this->irq.request |= irq;

    interruptProcess(late);
}

void Arm::interrupt(u32 pc, u32 lr, Psr::Mode mode)
{
    Psr cpsr = this->cpsr;
    switchMode(mode);
    spsr = cpsr;

    this->cpsr.t = 0;
    this->cpsr.i = 1;

    this->lr = lr;
    this->pc = pc;

    flushWord();
    state &= ~kStateThumb;
}

void Arm::interruptHw()
{
    u32 lr = pc - 2 * cpsr.size() + 4;

    interrupt(kVectorIrq, lr, Psr::kModeIrq);
}

void Arm::interruptSw()
{
    u32 lr = pc - cpsr.size();

    interrupt(kVectorSwi, lr, Psr::kModeSvc);
}

void Arm::interruptProcess(u64 late)
{
    if (irq.servable())
        state &= ~kStateHalt;

    if (irq.interrupted())
    {
        if (!events.interrupt.scheduled() && !(state & kStateIrq))
        {
            if (late < 3)
                scheduler.add(events.interrupt, 3 - late);
            else
                state |= kStateIrq;
        }
    }
    else
    {
        state &= ~kStateIrq;
        scheduler.remove(events.interrupt);
    }
}

bool Arm::Irq::servable() const
{
    return enable & request;
}

bool Arm::Irq::interrupted() const
{
    return servable() && master;
}
