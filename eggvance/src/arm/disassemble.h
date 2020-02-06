#pragma once

#include <string>

#include "common/integer.h"

std::string disassemble(u32 instr, u32 pc, u32 lr, bool thumb);
