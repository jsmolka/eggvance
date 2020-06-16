#pragma once

#include "interrupt/io.h"

enum Irq
{
    kIrqVBlank = 1 << 0x0,
    kIrqHBlank = 1 << 0x1,
    kIrqVMatch = 1 << 0x2,
    kIrqTimer  = 1 << 0x3,
    kIrqDma    = 1 << 0x8,
    kIrqKeypad = 1 << 0xC
};

class IrqHandler
{
public:
    friend class IO;

    void request(uint irq);

private:
    void update();

    u8 read(u32 addr) const;
    void write(u32 addr, u8 byte);

    IrqIo io;
};

extern IrqHandler irqh;
