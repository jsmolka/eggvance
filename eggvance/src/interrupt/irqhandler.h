#pragma once

#include "io/intr_io.h"

enum class Irq
{
    VBlank = 1 << 0x0,
    HBlank = 1 << 0x1,
    VMatch = 1 << 0x2,
    Timer  = 1 << 0x3,
    Dma    = 1 << 0x8,
    Keypad = 1 << 0xC
};

class IrqHandler
{
public:
    friend class IO;

    void request(Irq irq);

private:
    void update();

    u8 read(u32 addr) const;
    void write(u32 addr, u8 byte);

    IntrIO io;
};

extern IrqHandler irqh;
