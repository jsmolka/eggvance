#pragma once

#include "../register.h"

class Ie : public Register<u16>
{
public:
    Ie(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16,  0, 1> vblank;           // Enable V-Blank interrupt
    BitField<u16,  1, 1> hblank;           // Enable H-Blank interrupt
    BitField<u16,  2, 1> vcount_match;     // Enable V-Counter match interrupt
    BitField<u16,  3, 1> timer0_overflow;  // Enable timer0 overflow interrupt
    BitField<u16,  4, 1> timer1_overflow;  // Enable timer1 overflow interrupt
    BitField<u16,  5, 1> timer2_overflow;  // Enable timer2 overflow interrupt
    BitField<u16,  6, 1> timer3_overflow;  // Enable timer3 overflow interrupt
    BitField<u16,  7, 1> serial;           // Enable serial communication interrupt
    BitField<u16,  8, 1> dma0;             // Enable dma0 interrupt 
    BitField<u16,  9, 1> dma1;             // Enable dma1 interrupt 
    BitField<u16, 10, 1> dma2;             // Enable dma2 interrupt 
    BitField<u16, 11, 1> dma3;             // Enable dma3 interrupt 
    BitField<u16, 12, 1> keypad;           // Enable keypad interrupt 
    BitField<u16, 13, 1> gamepak;          // Enable GamePak removed interrupt 
};
