#pragma once

#include <functional>

#include "constants.h"
#include "base/register.h"

class HaltControl : public RegisterW<u8>
{
public:
    void write(uint index, u8 byte);
};

class WaitControl : public Register<u16>
{
public:
    WaitControl();

    void write(uint index, u8 byte);

    u64 waitHalf(u32 addr, Access access) const { return wait.half[(addr >> 25) & 0x3][static_cast<uint>(access)]; }
    u64 waitWord(u32 addr, Access access) const { return wait.word[(addr >> 25) & 0x3][static_cast<uint>(access)]; }

    uint sram     = 0;
    uint ws0_n    = 0;
    uint ws0_s    = 0;
    uint ws1_n    = 0;
    uint ws1_s    = 0;
    uint ws2_n    = 0;
    uint ws2_s    = 0;
    uint prefetch = 0;

private:
    void update();

    struct WaitStates
    {
        u64 half[4][2];
        u64 word[4][2];
    } wait;
};

class IrqMaster : public Register<u32, 0x0001>
{
public:
    operator bool() const;

    void write(uint index, u8 byte);

    std::function<void(void)> on_write;
};

class IrqEnable : public Register<u16, 0x3FFF>
{
public:
    operator u16() const;

    void write(uint index, u8 byte);

    std::function<void(void)> on_write;
};

class IrqRequest : public Register<u16, 0x3FFF>
{
public:
    operator u16() const;

    void write(uint index, u8 byte);

    std::function<void(void)> on_write;
};

