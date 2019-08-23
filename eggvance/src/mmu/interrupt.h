#pragma once

#include "mmio.h"

enum InterruptFlag
{
    IF_VBLANK  = 1 <<  0,
    IF_HBLANK  = 1 <<  1,
    IF_VMATCH  = 1 <<  2,
    IF_TIMER0  = 1 <<  3,
    IF_TIMER1  = 1 <<  4,
    IF_TIMER2  = 1 <<  5,
    IF_TIMER3  = 1 <<  6,
    IF_SERIAL  = 1 <<  7,
    IF_DMA0    = 1 <<  8,
    IF_DMA1    = 1 <<  9,
    IF_DMA2    = 1 << 10,
    IF_DMA3    = 1 << 11,
    IF_KEYPAD  = 1 << 12,
    IF_GAMEPAK = 1 << 13
};

class Interrupt
{
public:
    Interrupt() = delete;

    static inline void init(MMIO* mmio)
    {
        Interrupt::mmio = mmio;
    }

    static inline void request(InterruptFlag flag)
    {
        if (mmio->intr_master)
        {
            if (mmio->intr_enabled & flag)
                mmio->halt = false;

            mmio->intr_request |= flag;
        }
    }

    static inline bool requested()
    {
        return mmio->intr_master && (mmio->intr_enabled & mmio->intr_request);
    }

private:
    static inline MMIO* mmio = nullptr;
};
