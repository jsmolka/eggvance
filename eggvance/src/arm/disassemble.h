#pragma once

#include <string>

#include "common/integer.h"

struct DisasmData
{
    u32 lr;
    u32 pc;
    u32 instr;
    bool thumb;
};

const std::string disassemble(const DisasmData& data);
