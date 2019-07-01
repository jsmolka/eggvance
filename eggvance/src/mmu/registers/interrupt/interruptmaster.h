#pragma once

#include "../register.h"

class InterruptMaster : public Register<u32>
{
public:
    InterruptMaster(u32& data);

    using Register<u32>::operator=;
    using Register<u32>::operator int;

    BitField<u32, 0, 1> enabled;  // Interrupts enabled
};
