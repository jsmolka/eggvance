#include "irqhandler.h"

#include "arm/arm.h"
#include "base/macros.h"
#include "mmu/memmap.h"

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

u8 IrqHandler::read(u32 addr) const
{
    switch (addr)
    {
    READ_HALF_REG(REG_IE , io.enable );
    READ_HALF_REG(REG_IF , io.request);
    READ_HALF_REG(REG_IME, io.master );

    default:
        UNREACHABLE;
        return 0;
    }
}

void IrqHandler::write(u32 addr, u8 byte)
{
    switch (addr)
    {
    WRITE_HALF_REGX(REG_IE , io.enable );
    WRITE_HALF_REGX(REG_IF , io.request);
    WRITE_HALF_REGX(REG_IME, io.master );

    default:
        UNREACHABLE;
        break;
    }
    update();
}
