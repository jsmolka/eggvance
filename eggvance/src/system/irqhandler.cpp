#include "irqhandler.h"

#include "arm/arm.h"
#include "mmu/memmap.h"
#include "registers/macros.h"

IrqHandler irqh;

void IrqHandler::reset()
{
    *this = IrqHandler();
}

void IrqHandler::request(Irq irq)
{
    io.intr_request |= static_cast<uint>(irq);

    update();
}

void IrqHandler::update()
{
    bool interrupt = io.intr_enable & io.intr_request;

    if (interrupt)
        arm.state &= ~ARM::STATE_HALT;

    if (interrupt && io.intr_master)
        arm.state |= ARM::STATE_IRQ;
    else
        arm.state &= ~ARM::STATE_IRQ;
}

u8 IrqHandler::read(u32 addr) const
{
    switch (addr)
    {
    READ_HALF_REG(REG_IE , io.intr_enable );
    READ_HALF_REG(REG_IF , io.intr_request);
    READ_HALF_REG(REG_IME, io.intr_master );

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

void IrqHandler::write(u32 addr, u8 byte)
{
    switch (addr)
    {
    WRITE_HALF_REG(REG_IE , io.intr_enable , 0x0000'3FFF);
    WRITE_HALF_REG(REG_IF , io.intr_request, 0x0000'3FFF);
    WRITE_HALF_REG(REG_IME, io.intr_master , 0x0000'0001);

    default:
        EGG_UNREACHABLE;
        break;
    }
    update();
}
