#pragma once

#include "registers/intmaster.h"
#include "registers/intenabled.h"
#include "registers/intrequest.h"

enum IRQ
{
    kIrqVBlank  = 1 <<  0,
    kIrqHBlank  = 1 <<  1,
    kIrqVMatch  = 1 <<  2,
    kIrqTimer0  = 1 <<  3,
    kIrqTimer1  = 1 <<  4,
    kIrqTimer2  = 1 <<  5,
    kIrqTimer3  = 1 <<  6,
    kIrqSerial  = 1 <<  7,
    kIrqDma0    = 1 <<  8,
    kIrqDma1    = 1 <<  9,
    kIrqDma2    = 1 << 10,
    kIrqDma3    = 1 << 11,
    kIrqKeypad  = 1 << 12,
    kIrqGamePak = 1 << 13
};

class IO;

class IRQHandler
{
public:
    friend class IO;

    void reset();
    void request(uint irq);

    bool requested = false;

private:
    void update();

    u8 read(u32 addr);
    void write(u32 addr, u8 byte);

    IntMaster intr_master;
    IntEnabled intr_enable;
    IntRequest intr_request;
};

extern IRQHandler irqh;
