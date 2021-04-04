#include "arm.h"

enum class ExceptionVector
{
    Reset         = 0x00,
    Undefined     = 0x04,
    Swi           = 0x08,
    AbortPrefetch = 0x0C,
    AbortData     = 0x10,
    Reserved      = 0x14,
    Irq           = 0x18,
    Fiq           = 0x1C
};

void Arm::raise(Irq irq, u64 late)
{
    this->irq.request |= irq;

    interruptHandle(late);
}

void Arm::interrupt(u32 pc, u32 lr, Psr::Mode mode)
{
    Psr cpsr = this->cpsr;
    switchMode(mode);
    spsr = cpsr;

    this->lr = lr;
    this->pc = pc;

    this->cpsr.t = 0;
    this->cpsr.i = 1;

    flushWord();
    state &= ~State::Thumb;
}

void Arm::interruptHw()
{
    u32 lr = pc - 2 * cpsr.size() + 4;

    interrupt(uint(ExceptionVector::Irq), lr, Psr::Mode::Irq);
}

void Arm::interruptSw()
{
    u32 lr = pc - cpsr.size();

    interrupt(uint(ExceptionVector::Swi), lr, Psr::Mode::Svc);
}

void Arm::interruptHandle(u64 late)
{
    if (irq.enable & irq.request)
        state &= ~State::Halt;

    if (irq.enable & irq.request && irq.master)
    {
        if (!irq.delay.scheduled() && !(state & State::Irq))
        {
            if (late < 3)
                scheduler.insert(irq.delay, 3 - late);
            else
                state |= State::Irq;
        }
    }
    else
    {
        state &= ~State::Irq;
        scheduler.remove(irq.delay);
    }
}
