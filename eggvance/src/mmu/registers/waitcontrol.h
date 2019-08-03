#pragma once

struct WaitControl
{
    struct WaitState
    {
        int n;  // Nonsequential cycles (0..3 = 4,3,2,8 cycles)
        int s;  // Sequential cycles (dependent on waitstate)
    };

    int sram;       // SRAM wait control (0..3 = 4,3,2,8 cycles)      
    WaitState ws0;  // Sequential cycles (0..1 = 2,1 cycles)
    WaitState ws1;  // Sequential cycles (0..1 = 4,1 cycles)
    WaitState ws2;  // Sequential cycles (0..1 = 8,1 cycles)
    int phi;        // PHI terminal output (0..3 = disable, 4.19MHz, 8.38MHz, 16.78MHz)
    int prefetch;   // GamePak prefetch buffer (1 = enable) 
    int type;       // GamePak type flag (0 = GBA, 1 = GBC)
};
