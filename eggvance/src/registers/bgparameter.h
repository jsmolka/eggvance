#pragma once

#include "register.h"
#include "common/config.h"

template<uint init>
class BGParameter : public TRegister<BGParameter<init>, 2>
{
public:
    BGParameter()
    {
        if (config.bios_skip)
            *reinterpret_cast<s16*>(data) = init;
    }

    inline operator s16()
    {
        return *reinterpret_cast<s16*>(data);
    }

    template<uint index>
    inline u8 read() const = delete;
};

using BGParameterA = BGParameter<0x0100>;
using BGParameterB = BGParameter<0x0000>;
using BGParameterC = BGParameter<0x0000>;
using BGParameterD = BGParameter<0x0100>;
