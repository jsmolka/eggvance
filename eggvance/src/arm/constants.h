#pragma once

enum Irq
{
    kIrqVBlank  = 1 << 0,
    kIrqHBlank  = 1 << 1,
    kIrqVMatch  = 1 << 2,
    kIrqTimer0  = 1 << 3,
    kIrqTimer1  = 1 << 4,
    kIrqTimer2  = 1 << 5,
    kIrqTimer3  = 1 << 6,
    kIrqSerial  = 1 << 7,
    kIrqDma0    = 1 << 8,
    kIrqDma1    = 1 << 9,
    kIrqDma2    = 1 << 10,
    kIrqDma3    = 1 << 11,
    kIrqGamepad = 1 << 12,
    kIrqGamePak = 1 << 13
};

enum State
{
    kStateThumb = 1 << 0,
    kStateHalt  = 1 << 1,
    kStateIrq   = 1 << 2,
    kStateDma   = 1 << 3,
    kStateTimer = 1 << 4
};

enum class Access
{
    NonSequential,
    Sequential
};
