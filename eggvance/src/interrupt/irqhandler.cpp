#include "irqhandler.h"

#include "arm/arm.h"
#include "base/macros.h"
#include "mmu/memmap.h"

IRQHandler irqh;

void IRQHandler::request(IRQ irq)
{
    io.request |= static_cast<uint>(irq);

    update();
}

void IRQHandler::update()
{
    bool interrupt = io.enable & io.request;

    if (interrupt)
        arm.state &= ~ARM::STATE_HALT;

    if (interrupt && io.master)
        arm.state |= ARM::STATE_IRQ;
    else
        arm.state &= ~ARM::STATE_IRQ;
}

u8 IRQHandler::read(u32 addr) const
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

void IRQHandler::write(u32 addr, u8 byte)
{
    switch (addr)
    {
    WRITE_HALF_REG(REG_IE , io.enable , 0x0000'3FFF);
    WRITE_HALF_REG(REG_IF , io.request, 0x0000'3FFF);
    WRITE_HALF_REG(REG_IME, io.master , 0x0000'0001);

    default:
        UNREACHABLE;
        break;
    }
    update();
}
