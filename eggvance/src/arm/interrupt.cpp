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

void Arm::raise(uint irq)
{
    this->irq.request.value |= irq;

    interruptProcess();
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

void Arm::interruptProcess()
{
    bool servable = irq.enable & irq.request;

    if (servable)
        state &= ~kStateHalt;

    if (servable && irq.master)
    {
        if (!irq.delayed && events.interrupt.when == 0)
        {
            irq.delayed = false;
            scheduler.queueIn(events.interrupt, 3);
        }
        state |= kStateIrq;
    }
    else
    {
        scheduler.dequeue(events.interrupt);
        irq.delayed = false;
        state &= ~kStateIrq;
    }
}

void Arm::Events::doInterrupt(void* data, u64 late)
{
    Arm& arm = *reinterpret_cast<Arm*>(data);

    arm.irq.delayed = true;
}
