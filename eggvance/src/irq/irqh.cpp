#include "irqh.h"

#include "arm/arm.h"

void IrqHandler::request(uint irq)
{
    io.request.value |= irq;

    update();
}

void IrqHandler::update()
{
    bool interrupt = io.enable.value & io.request.value;

    if (interrupt)
        arm.state &= ~Arm::kStateHalt;

    if (interrupt && io.master.value)
        arm.state |= Arm::kStateIrq;
    else
        arm.state &= ~Arm::kStateIrq;
}
