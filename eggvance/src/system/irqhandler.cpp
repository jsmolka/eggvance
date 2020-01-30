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

void IRQHandler::request(uint irq)
{
    intr_request |= irq;

    update();
}

void IRQHandler::update()
{
    requested = intr_enable & intr_request && intr_master;

    if (intr_enable & intr_request)
        arm.io.haltcnt = false;
}

u8 IRQHandler::read(u32 addr)
{
    switch (addr)
    {
    READ2(REG_IE , intr_enable);
    READ2(REG_IF , intr_request);
    READ2(REG_IME, intr_master);

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

void IRQHandler::write(u32 addr, u8 byte)
{
    switch (addr)
    {
    WRITE2(REG_IE , intr_enable);
    WRITE2(REG_IF , intr_request);
    WRITE2(REG_IME, intr_master);

    default:
        EGG_UNREACHABLE;
        break;
    }
    update();
}
