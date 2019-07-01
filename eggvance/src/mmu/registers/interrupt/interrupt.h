#pragma once

#include "../register.h"

enum InterruptFlag
{
    IF_VBLANK            = 1 <<  0,
    IF_HBLANK            = 1 <<  1,
    IF_VCOUNT_MATCH      = 1 <<  2,
    IF_TIMER0_OVERFLOW   = 1 <<  3,
    IF_TIMER1_OVERFLOW   = 1 <<  4,
    IF_TIMER2_OVERFLOW   = 1 <<  5,
    IF_TIMER3_OVERFLOW   = 1 <<  6,
    IF_SERIAL            = 1 <<  7,
    IF_DMA0              = 1 <<  8,
    IF_DMA1              = 1 <<  9,
    IF_DMA2              = 1 << 10,
    IF_DMA3              = 1 << 11,
    IF_KEYPAD            = 1 << 12,
    IF_GAMEPAK           = 1 << 13
};

class Interrupt : public Register<u16>
{
public:
    Interrupt(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16,  0, 1> vblank;           // V-Blank interrupt
    BitField<u16,  1, 1> hblank;           // H-Blank interrupt
    BitField<u16,  2, 1> vcount_match;     // V-Counter match interrupt
    BitField<u16,  3, 1> timer0_overflow;  // Timer0 overflow interrupt
    BitField<u16,  4, 1> timer1_overflow;  // Timer1 overflow interrupt
    BitField<u16,  5, 1> timer2_overflow;  // Timer2 overflow interrupt
    BitField<u16,  6, 1> timer3_overflow;  // Timer3 overflow interrupt
    BitField<u16,  7, 1> serial;           // Terial communication interrupt
    BitField<u16,  8, 1> dma0;             // DMA0 interrupt 
    BitField<u16,  9, 1> dma1;             // DMA1 interrupt 
    BitField<u16, 10, 1> dma2;             // DMA2 interrupt 
    BitField<u16, 11, 1> dma3;             // DMA3 interrupt 
    BitField<u16, 12, 1> keypad;           // Keypad interrupt 
    BitField<u16, 13, 1> gamepak;          // GamePak removed interrupt 
};

using InterruptEnabled = Interrupt;
using InterruptRequest = Interrupt;
