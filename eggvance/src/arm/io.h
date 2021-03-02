#pragma once

#include <functional>

#include "constants.h"
#include "base/register.h"

class HaltControl : public XRegisterW<u8>
{
public:
    void write(uint index, u8 byte);
};

class WaitControl : public XRegister<u16>
{
public:
    WaitControl();

    void write(uint index, u8 byte);

    int cyclesHalf(u32 addr, Access access) const { return cycles_half[(addr >> 25) & 0x3][static_cast<uint>(access)]; }
    int cyclesWord(u32 addr, Access access) const { return cycles_word[(addr >> 25) & 0x3][static_cast<uint>(access)]; }

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

    int cycles_half[4][2];
    int cycles_word[4][2];
};

class IrqMaster : public XRegister<u32, 0x0001>
{
public:
    operator bool() const;

    void write(uint index, u8 byte);

    std::function<void(void)> on_write;
};

class IrqEnable : public XRegister<u16, 0x3FFF>
{
public:
    operator u16() const;

    void write(uint index, u8 byte);

    std::function<void(void)> on_write;
};

class IrqRequest : public XRegister<u16, 0x3FFF>
{
public:
    operator u16() const;

    void write(uint index, u8 byte);

    std::function<void(void)> on_write;
};

