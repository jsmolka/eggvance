#include "irqhandler.h"

#include "arm/arm.h"
#include "common/macros.h"
#include "mmu/memmap.h"
#include "registers/macros.h"

IRQHandler irqh;

void IRQHandler::reset()
{
    *this = IRQHandler();
}

void IRQHandler::request(IRQ irq)
{
    io.intr_request |= static_cast<uint>(irq);

    update();
}

void IRQHandler::update()
{
    requested = io.intr_enable & io.intr_request;

    if (requested)
        arm.io.haltcnt = false;

    requested &= io.intr_master;

    arm.updateDispatch();
}

u8 IRQHandler::read(u32 addr)
{
    switch (addr)
    {
    READ_HALF_REG(REG_IE , io.intr_enable);
    READ_HALF_REG(REG_IF , io.intr_request);
    READ_HALF_REG(REG_IME, io.intr_master);

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

void IRQHandler::write(u32 addr, u8 byte)
{
    switch (addr)
    {
    WRITE_HALF_REG(REG_IE , io.intr_enable);
    WRITE_HALF_REG(REG_IF , io.intr_request);
    WRITE_HALF_REG(REG_IME, io.intr_master);

    default:
        EGG_UNREACHABLE;
        break;
    }
    update();
}
