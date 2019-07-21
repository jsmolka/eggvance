#pragma once

#include "common/integer.h"

enum Mode
{
    MODE_USR = 0b10000,  // User
    MODE_FIQ = 0b10001,  // Fast interrupt request
    MODE_IRQ = 0b10010,  // Interrupt request
    MODE_SVC = 0b10011,  // Supervisor
    MODE_ABT = 0b10111,  // Abort
    MODE_SYS = 0b11111,  // System
    MODE_UND = 0b11011   // Undefined
};

enum Condition
{
    COND_EQ = 0x0,  // Equal
    COND_NE = 0x1,  // Not equal
    COND_CS = 0x2,  // Carry set
    COND_CC = 0x3,  // Carry clear
    COND_MI = 0x4,  // Minus
    COND_PL = 0x5,  // Plus
    COND_VS = 0x6,  // V set
    COND_VC = 0x7,  // V clear
    COND_HI = 0x8,  // Unsigned higher
    COND_LS = 0x9,  // Unsigned lower or same
    COND_GE = 0xA,  // Greater or equal
    COND_LT = 0xB,  // Less than
    COND_GT = 0xC,  // Greater than
    COND_LE = 0xD,  // Less or equal
    COND_AL = 0xE,  // Always
    COND_NV = 0xF,  // Never
};

enum CPSR
{
    CPSR_M = 0x1F,     // Mode
    CPSR_T = 1 << 5,   // THUMB enable
    CPSR_F = 1 << 6,   // FIQ disable
    CPSR_I = 1 << 7,   // IRQ disable
    CPSR_V = 1 << 28,  // Overflow
    CPSR_C = 1 << 29,  // Carry
    CPSR_Z = 1 << 30,  // Zero
    CPSR_N = 1 << 31   // Negative
};

enum ExceptionVector
{
    EXV_RESET    = 0x00,  // 1 Reset
    EXV_UNDEF    = 0x04,  // 7 Undefined instruction
    EXV_SWI      = 0x08,  // 6 Software interrupt
    EXV_PREFETCH = 0x0C,  // 5 Prefetch abort
    EXV_DATA     = 0x10,  // 2 Data abort
    EXV_ADDR     = 0x14,  // ? Address exceeds 26bit
    EXV_IRQ      = 0x18,  // 4 Normal interrupt
    EXV_FIQ      = 0x1C   // 3 Fast interrupt
};
