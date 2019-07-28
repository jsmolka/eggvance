#pragma once

#include "mmu.h"

enum InterruptFlag
{
    IF_VBLANK          = 1 <<  0,  // V-Blank interrupt
    IF_HBLANK          = 1 <<  1,  // H-Blank interrupt
    IF_VCOUNT_MATCH    = 1 <<  2,  // V-Counter match interrupt
    IF_TIMER0_OVERFLOW = 1 <<  3,  // Timer0 overflow interrupt
    IF_TIMER1_OVERFLOW = 1 <<  4,  // Timer1 overflow interrupt
    IF_TIMER2_OVERFLOW = 1 <<  5,  // Timer2 overflow interrupt
    IF_TIMER3_OVERFLOW = 1 <<  6,  // Timer3 overflow interrupt
    IF_SERIAL          = 1 <<  7,  // Serial communication interrupt
    IF_DMA0            = 1 <<  8,  // DMA0 interrupt
    IF_DMA1            = 1 <<  9,  // DMA1 interrupt
    IF_DMA2            = 1 << 10,  // DMA2 interrupt
    IF_DMA3            = 1 << 11,  // DMA3 interrupt
    IF_KEYPAD          = 1 << 12,  // Keypad interrupt
    IF_GAMEPAK         = 1 << 13   // GamePak removed interrupt
};

class Interrupt
{
public:
    Interrupt() = delete;

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

    static inline MMU* mmu = nullptr;
};
