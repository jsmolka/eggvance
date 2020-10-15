#pragma once

constexpr auto kScreenW     = 240;
constexpr auto kScreenH     = 160;
constexpr auto kColorMask   = 0x7FFF;
constexpr auto kTransparent = 0x8000;
constexpr auto kRefreshRate = 59.737;

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

enum class ColorMode
{
    C16x16 = 0,
    C256x1 = 1
};

enum class ObjectMapping
{
    TwoDim = 0,
    OneDim = 1
};
