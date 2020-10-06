#pragma once

#include <string>

#include "base/int.h"

std::string disassemble(u32 instr, u32 pc);
std::string disassemble(u16 instr, u32 pc, u32 lr);
