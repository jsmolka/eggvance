#include "arm.h"

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

    processIrq();
}

void Arm::interrupt(u32 pc, u32 lr, PSR::Mode mode)
{
    PSR cpsr = this->cpsr;
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

    interrupt(kVectorIrq, lr, PSR::kModeIrq);
}

void Arm::interruptSw()
{
    u32 lr = pc - cpsr.size();

    interrupt(kVectorSwi, lr, PSR::kModeSvc);
}

void Arm::processIrq()
{
    bool interrupt = irq.enable.value & irq.request.value;

    if (interrupt)
        state &= ~kStateHalt;

    if (interrupt && irq.master.value)
        state |= kStateIrq;
    else
        state &= ~kStateIrq;
}
