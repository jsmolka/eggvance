#pragma once

#include "base/macros.h"
#include "interrupt/io.h"
#include "mmu/memmap.h"

enum Irq
{
    kIrqVBlank = 1 << 0,
    kIrqHBlank = 1 << 1,
    kIrqVMatch = 1 << 2,
    kIrqTimer  = 1 << 3,
    kIrqDma    = 1 << 8,
    kIrqKeypad = 1 << 12
};

class IrqHandler
{
public:
    friend class Io;

    void request(uint irq);

private:
    void update();

    template<uint addr>
    u8 read() const;

    template<uint addr>
    void write(u8 byte);

    IrqIo io;
};

extern IrqHandler irqh;

template<uint addr>
u8 IrqHandler::read() const
{
    switch (addr)
    {
    CASE2(kRegIrqEnable , return io.enable.read<kIndex>())
    CASE2(kRegIrqRequest, return io.request.read<kIndex>())
    CASE4(kRegIrqMaster , return io.master.read<kIndex>())

    default:
        UNREACHABLE;
        return 0;
    }
}

template<uint addr>
void IrqHandler::write(u8 byte)
{
    switch (addr)
    {
    CASE2(kRegIrqEnable , io.enable.write<kIndex>(byte))
    CASE2(kRegIrqRequest, io.request.write<kIndex>(byte))
    CASE4(kRegIrqMaster , io.master.write<kIndex>(byte))

    default:
        UNREACHABLE;
        break;
    }
    update();
}
