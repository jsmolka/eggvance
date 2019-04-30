#pragma once

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

enum Format
{
    FMT_NONE,    // Undefined instruction
    FMT_REFILL,  // Refill the pipeline
    ARM_1,       // Branch and exchange
    ARM_2,       // Branch and branch with link
    ARM_3,       // Data processing
    ARM_4,       // PSR transfer
    ARM_5,       // Multiply and multiply-accumulate
    ARM_6,       // Multiply long and multiply-accumulate long
    ARM_7,       // Single data transfer
    ARM_8,       // Halfword data transfer
    ARM_9,       // Block data transfer
    ARM_10,      // Single data swap
    ARM_11,      // Software interrupt
    ARM_12,      // Coprocessor data operations
    ARM_13,      // Coprocessor data transfers
    ARM_14,      // Coprocessor register transfers
    ARM_15,      // Undefined
    THUMB_1,     // Move shifted register
    THUMB_2,     // Add / subtract
    THUMB_3,     // Move / compare / add / subtract immediate
    THUMB_4,     // ALU operations
    THUMB_5,     // High register operations / branch exchange
    THUMB_6,     // Load PC-relative
    THUMB_7,     // Load / store with register offset
    THUMB_8,     // Load / store sign-extended byte / halfword
    THUMB_9,     // Load / store with immediate offset
    THUMB_10,    // Load / store halfword
    THUMB_11,    // Load / store SP-relative
    THUMB_12,    // Load address
    THUMB_13,    // Add offset to stack pointer
    THUMB_14,    // Push / pop registers
    THUMB_15,    // Mutiple load / store
    THUMB_16,    // Conditional branch
    THUMB_17,    // Software interrupt
    THUMB_18,    // Unconditional branch
    THUMB_19     // Long branch with link
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
