#pragma once

#include "register.h"
#include "common/config.h"

template<s16 post_bios>
class BGParameter : public TRegister<BGParameter<post_bios>, 2>
{
public:
    BGParameter()
    {
        if (config.bios_skip)
            *reinterpret_cast<s16*>(this->data) = post_bios;
    }

    inline operator s16()
    {
        return *reinterpret_cast<s16*>(this->data);
    }

    template<uint index>
    inline u8 read() const = delete;
};

using BGParameterA = BGParameter<0x0100>;
using BGParameterB = BGParameter<0x0000>;
using BGParameterC = BGParameter<0x0000>;
using BGParameterD = BGParameter<0x0100>;
