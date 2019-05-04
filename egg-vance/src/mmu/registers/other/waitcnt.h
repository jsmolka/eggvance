#pragma once

#include "../register.h"

class Waitcnt : public Register<u16>
{
public:
    Waitcnt(u16& data);

    using Register<u16>::operator=;
    using Register<u16>::operator unsigned int;

    BitField<u16,  0, 2> sram;      // SRAM wait control (0..3 = 4,3,2,8 cycles)          
    BitField<u16,  2, 2> nonseq0;   // Wait state 0 first access (0..3 = 4,3,2,8 cycles)
    BitField<u16,  4, 1> seq0;      // Wait state 0 second access (0..1 = 2,1 cycles)
    BitField<u16,  5, 2> nonseq1;   // Wait state 1 first access (0..3 = 4,3,2,8 cycles)
    BitField<u16,  7, 1> seq1;      // Wait state 1 second access (0..1 = 4,1 cycles)
    BitField<u16,  8, 2> nonseq2;   // Wait state 2 first access (0..3 = 4,3,2,8 cycles)
    BitField<u16, 10, 1> seq2;      // Wait state 2 second access (0..1 = 8,1 cycles)
    BitField<u16, 11, 2> phi;       // PHI terminal output (0..3 = disable, 4.19MHz, 8.38MHz, 16.78MHz)
    BitField<u16, 14, 1> prefetch;  // Game Pak prefetch buffer (1 = enable) 
    BitField<u16, 15, 1> type;      // Game Pak type flag (0 = GBA, 1 = GBC)
};
