#pragma once

#include "mmu.h"

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

    static inline void init(MMU* mmu)
    {
        Interrupt::mmu = mmu;
    }

    static inline void request(InterruptFlag flag)
    {
        if (mmu->intr_master)
        {
            if (mmu->intr_enabled & flag)
                mmu->halt = false;

            mmu->intr_request |= flag;
        }
    }

    static inline bool requested()
    {
        return mmu->intr_master && (mmu->intr_enabled & mmu->intr_request);
    }

private:
    static inline MMU* mmu = nullptr;
};
