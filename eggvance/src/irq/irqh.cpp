#include "irqh.h"

#include "arm/arm.h"

IrqHandler irqh;

void IrqHandler::request(uint irq)
{
    io.request.value |= irq;

    update();
}

void IrqHandler::update()
{
    bool interrupt = io.enable.value & io.request.value;

    if (interrupt)
        arm.state &= ~ARM::kStateHalt;

    if (interrupt && io.master.value)
        arm.state |= ARM::kStateIrq;
    else
        arm.state &= ~ARM::kStateIrq;
}
