#pragma once

#include "io/intrio.h"

enum class IRQ
{
    VBlank = 1 << 0x0,
    HBlank = 1 << 0x1,
    VMatch = 1 << 0x2,
    Timer  = 1 << 0x3,
    Dma    = 1 << 0x8,
    Keypad = 1 << 0xC
};

class IRQHandler
{
public:
    friend class IO;

    void request(IRQ irq);

private:
    void update();

    u8 read(u32 addr) const;
    void write(u32 addr, u8 byte);

    IntrIO io;
};

extern IRQHandler irqh;
