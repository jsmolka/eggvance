#pragma once

#include "base/constants.h"
#include "base/macros.h"
#include "interrupt/io.h"

enum Irq
{
    kIrqVBlank  = 1 << 0,
    kIrqHBlank  = 1 << 1,
    kIrqVMatch  = 1 << 2,
    kIrqTimer0  = 1 << 3,
    kIrqTimer1  = 1 << 4,
    kIrqTimer2  = 1 << 5,
    kIrqTimer3  = 1 << 6,
    kIrqSerial  = 1 << 7,
    kIrqDma0    = 1 << 8,
    kIrqDma1    = 1 << 9,
    kIrqDma2    = 1 << 10,
    kIrqDma3    = 1 << 11,
    kIrqKeypad  = 1 << 12,
    kIrqGamePak = 1 << 13
};

class IrqHandler
{
public:
    void request(uint irq);

    template<uint Addr>
    u8 read() const;

    template<uint Addr>
    void write(u8 byte);

private:
    void update();

    IrqIo io;
};

extern IrqHandler irqh;

template<uint Addr>
u8 IrqHandler::read() const
{
    INDEXED_IF2(Addr, kRegIrqEnable , return io.enable.read<kIndex>())
    INDEXED_IF2(Addr, kRegIrqRequest, return io.request.read<kIndex>())
    INDEXED_IF4(Addr, kRegIrqMaster , return io.master.read<kIndex>())

    UNREACHABLE;
    return 0;
}

template<uint Addr>
void IrqHandler::write(u8 byte)
{
    INDEXED_IF2(kRegIrqEnable , io.enable.write<kIndex>(byte))
    INDEXED_IF2(kRegIrqRequest, io.request.write<kIndex>(byte))
    INDEXED_IF4(kRegIrqMaster , io.master.write<kIndex>(byte))

    update();
}
