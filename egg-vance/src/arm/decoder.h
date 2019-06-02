#pragma once

#include "common/integer.h"
#include "enums.h"

namespace decoder
{
    ArmInstr decodeArm(u32 instr);
    ThumbInstr decodeThumb(u16 instr);
}
