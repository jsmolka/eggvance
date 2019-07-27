#pragma once

struct WaitControl
{
    int sram;      // SRAM wait control (0..3 = 4,3,2,8 cycles)          
    int ws0_n;   // Wait state 0 first access (0..3 = 4,3,2,8 cycles)
    int ws0_s;      // Wait state 0 second access (0..1 = 2,1 cycles)
    int ws1_n;   // Wait state 1 first access (0..3 = 4,3,2,8 cycles)
    int ws1_s;      // Wait state 1 second access (0..1 = 4,1 cycles)
    int ws2_n;   // Wait state 2 first access (0..3 = 4,3,2,8 cycles)
    int ws2_s;      // Wait state 2 second access (0..1 = 8,1 cycles)
    int phi;       // PHI terminal output (0..3 = disable, 4.19MHz, 8.38MHz, 16.78MHz)
    int prefetch;  // Game Pak prefetch buffer (1 = enable) 
    int type;      // Game Pak type flag (0 = GBA, 1 = GBC)
};
