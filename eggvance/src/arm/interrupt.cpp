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
    interrupt.request |= irq;

    interruptHandle(late);
}

void Arm::interruptHw()
{
    Psr saved = cpsr;
    switchMode(Psr::Mode::Irq);
    spsr = saved;

    lr = pc - 2 * cpsr.size() + 4;
    pc = uint(ExceptionVector::Irq);

    cpsr.t = 0;
    cpsr.i = 1;

    flushWord();
    state &= ~State::Thumb;
}

void Arm::interruptSw()
{
    Psr saved = cpsr;
    switchMode(Psr::Mode::Svc);
    spsr = saved;

    lr = pc - cpsr.size();
    pc = uint(ExceptionVector::Swi);

    cpsr.t = 0;
    cpsr.i = 1;

    flushWord();
    state &= ~State::Thumb;
}

void Arm::interruptHandle(u64 late)
{
    if (interrupt.isServable())
        state &= ~State::Halt;

    if (interrupt.isServable() && interrupt.master)
    {
        if (!interrupt.delay.scheduled() && !(state & State::Irq))
        {
            if (late < 3)
                scheduler.insert(interrupt.delay, 3 - late);
            else
                state |= State::Irq;
        }
    }
    else
    {
        state &= ~State::Irq;
        scheduler.remove(interrupt.delay);
    }
}

bool Arm::Interrupt::isServable() const
{
    return enable & request;
}
