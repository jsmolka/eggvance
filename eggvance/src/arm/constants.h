#pragma once

enum class Irq
{
    VBlank  = 1 << 0,
    HBlank  = 1 << 1,
    VMatch  = 1 << 2,
    Timer   = 1 << 3,
    Serial  = 1 << 7,
    Dma     = 1 << 8,
    Keypad  = 1 << 12,
    GamePak = 1 << 13
};

enum class Access
{
    NonSequential,
    Sequential
};
