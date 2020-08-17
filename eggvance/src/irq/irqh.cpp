#include "irqh.h"

#include "core/core.h"

IrqHandler::IrqHandler(Core& core)
    : core(core)
{

}

void IrqHandler::request(uint irq)
{
    io.request.value |= irq;

    update();
}

void IrqHandler::update()
{
    bool interrupt = io.enable.value & io.request.value;

    if (interrupt)
        core.arm.state &= ~ARM::kStateHalt;

    if (interrupt && io.master.value)
        core.arm.state |= ARM::kStateIrq;
    else
        core.arm.state &= ~ARM::kStateIrq;
}
