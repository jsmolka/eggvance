#pragma once

#include <memory>

#include "common/integer.h"

class InputDevice
{
public:
    virtual void init() = 0;
    virtual void deinit() = 0;
    virtual uint state() = 0;

protected:
    enum ButtonBit
    {
        kBitA      = 0,
        kBitB      = 1,
        kBitSelect = 2,
        kBitStart  = 3,
        kBitRight  = 4,
        kBitLeft   = 5,
        kBitUp     = 6,
        kBitDown   = 7,
        kBitR      = 8,
        kBitL      = 9
    };
};

extern std::shared_ptr<InputDevice> input_device;
