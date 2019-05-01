#pragma once

#include "../register.h"

class Waitcnt : public Register
{
public:
    Waitcnt(u16& data);

    using Register::operator=;
    using Register::operator unsigned int;

    Field< 0, 2> sram;      // SRAM wait control (0..3 = 4,3,2,8 cycles)          
    Field< 2, 2> nonseq0;   // Wait state 0 first access (0..3 = 4,3,2,8 cycles)
    Field< 4, 1> seq0;      // Wait state 0 second access (0..1 = 2,1 cycles)
    Field< 5, 2> nonseq1;   // Wait state 1 first access (0..3 = 4,3,2,8 cycles)
    Field< 7, 1> seq1;      // Wait state 1 second access (0..1 = 4,1 cycles)
    Field< 8, 2> nonseq2;   // Wait state 2 first access (0..3 = 4,3,2,8 cycles)
    Field<10, 1> seq2;      // Wait state 2 second access (0..1 = 8,1 cycles)
    Field<11, 2> phi;       // PHI terminal output (0..3 = disable, 4.19MHz, 8.38MHz, 16.78MHz)
    Field<14, 1> prefetch;  // Game Pak prefetch buffer (1 = enable) 
    Field<15, 1> type;      // Game Pak type flag (0 = GBA, 1 = GBC)
};
