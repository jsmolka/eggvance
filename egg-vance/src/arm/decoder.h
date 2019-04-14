#pragma once

#include "common/integer.h"
#include "enums.h"

class Decoder
{
public:
    static Format decode(u32 instr, bool arm);

private:
    Decoder() = default;
    
    static Format decodeArm(u32 instr);
    static Format decodeThumb(u16 instr);
};
