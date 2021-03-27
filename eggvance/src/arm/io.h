#pragma once

#include "enums.h"
#include "base/array.h"
#include "base/register.h"

class PostFlag : public Register<u8, 0x01>
{
public:
    PostFlag();
};

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

    u64 waitHalf(u32 addr, Access access) const;
    u64 waitWord(u32 addr, Access access) const;

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

    struct
    {
        array<u64, 4, 2> half = {};
        array<u64, 4, 2> word = {};
    } wait;
};

class IrqMaster : public Register<u32, 0x0001>
{
public:
    operator bool() const;

    void write(uint index, u8 byte);
};

class IrqEnable : public Register<u16, 0x3FFF>
{
public:
    operator u16() const;

    void write(uint index, u8 byte);
};

class IrqRequest : public Register<u16, 0x3FFF>
{
public:
    IrqRequest& operator|=(Irq irq);
    operator u16() const;

    void write(uint index, u8 byte);
};

