#pragma once

#include "../register.h"

class Ir : public Register<u16>
{
public:
    Ir(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator int;

    BitField<u16,  0, 1> vblank;           // Request V-Blank interrupt
    BitField<u16,  1, 1> hblank;           // Request H-Blank interrupt
    BitField<u16,  2, 1> vcount_match;     // Request V-Counter match interrupt
    BitField<u16,  3, 1> timer0_overflow;  // Request timer0 overflow interrupt
    BitField<u16,  4, 1> timer1_overflow;  // Request timer1 overflow interrupt
    BitField<u16,  5, 1> timer2_overflow;  // Request timer2 overflow interrupt
    BitField<u16,  6, 1> timer3_overflow;  // Request timer3 overflow interrupt
    BitField<u16,  7, 1> serial;           // Request serial communication interrupt
    BitField<u16,  8, 1> dma0;             // Request dma0 interrupt 
    BitField<u16,  9, 1> dma1;             // Request dma1 interrupt 
    BitField<u16, 10, 1> dma2;             // Request dma2 interrupt 
    BitField<u16, 11, 1> dma3;             // Request dma3 interrupt 
    BitField<u16, 12, 1> keypad;           // Request keypad interrupt 
    BitField<u16, 13, 1> gamepak;          // Request GamePak removed interrupt 
};
