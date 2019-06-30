#pragma once

enum InterruptFlag
{
    IF_VBLANK           = 1 <<  0,
    IF_HBLANK           = 1 <<  1,
    IF_VCOUNT_MATCH     = 1 <<  2,
    IF_TIMER0_OVERFLOW  = 1 <<  3,
    IF_TIMER1_OVERFLOW  = 1 <<  4,
    IF_TIMER2_OVERFLOW  = 1 <<  5,
    IF_TIMER3_OVERFLOW  = 1 <<  6,
    IF_SERIAL           = 1 <<  7,
    IF_DMA0             = 1 <<  8,
    IF_DMA1             = 1 <<  9,
    IF_DMA2             = 1 << 10,
    IF_DMA3             = 1 << 11,
    IF_KEYPAD           = 1 << 12,
    IF_GAMEPAK          = 1 << 13
};
