#pragma once

#include "integer.h"
#include "mmu.h"

class ARM7
{
public:
    #pragma region Enums
    enum Mode
    {
        MODE_USR = 0b10000,  // Normal program execution state
        MODE_FIQ = 0b10001,  // Support data transfer or channel process
        MODE_IRQ = 0b10010,  // Generl purpose interrupt handling
        MODE_SVC = 0b10011,  // Protected mode for the operating system
        MODE_ABT = 0b10111,  // Entered after data or insruction prefetch abort
        MODE_SYS = 0b11111,  // Privileged user mode for the operating system
        MODE_UND = 0b11011   // Entered when an undefined instruction is executed
    };

    enum CPSR
    {
        CPSR_M = 0b11111,    // Mode
        CPSR_T = (1 << 5),   // THUMB enable
        CPSR_F = (1 << 6),   // FIQ disable
        CPSR_I = (1 << 7),   // IRQ disable
        CPSR_V = (1 << 28),  // Overflow
        CPSR_C = (1 << 29),  // Underflow
        CPSR_Z = (1 << 30),  // Zero
        CPSR_N = (1 << 31)   // Negative / sign
    };

    enum Condition
    {
        COND_EQ = 0x0,  // Z = 1, equal (zero)
        COND_NE = 0x1,  // Z = 0, not equal (nonzero)
        COND_CS = 0x2,  // C = 1, carry set (unsigned higher or same)
        COND_CC = 0x3,  // C = 0, carry cleared (carry cleared)
        COND_MI = 0x4,  // N = 1, minus (negative)
        COND_PL = 0x5,  // N = 0, plus (positive)
        COND_VS = 0x6,  // V = 1, V set (overflow)
        COND_VC = 0x7,  // V = 0, V cleared (no overflow)
        COND_HI = 0x8,  // C = 1 and Z = 0, unsigned higher
        COND_LS = 0x9,  // C = 0 or Z = 1, unsigned lower or same
        COND_GE = 0xA,  // N = V, greater or equal
        COND_LT = 0xB,  // N <> V, less than
        COND_GT = 0xC,  // Z = 1 and N = V, greater than
        COND_LE = 0xD,  // Z = 1 or N <> V, less or equal
        COND_AL = 0xE,  // Always
        COND_NV = 0xF,  // Never
    };

    enum Instruction
    {
        UNDEFINED,    // Undefined instruction
        REFILL_PIPE,  // Refill the pipe
        THUMB_1,      // Move shifted register
        THUMB_2,      // Add / subtract
        THUMB_3,      // Move / compare / add / subtract immediate
        THUMB_4,      // ALU operations
        THUMB_5,      // Hi register operations / branch exchange
        THUMB_6,      // PC-relative load
        THUMB_7,      // Load / store with register offset
        THUMB_8,      // Load / store sign-extended byte / halfword
        THUMB_9,      // Load / store with immediate offset
        THUMB_10,     // Load / store halfword
        THUMB_11,     // SP-relative load / store
        THUMB_12,     // Load address
        THUMB_13,     // Add offset to stack pointer
        THUMB_14,     // Push / pop registers
        THUMB_15,     // Mutiple load / store
        THUMB_16,     // Conditional branch
        THUMB_17,     // Software interrupt
        THUMB_18,     // Unconditional branch
        THUMB_19      // Long branch with link
    };
    #pragma endregion

    #pragma region Registers
    struct Registers
    {
        // General Purpose Registers
        u32 r0;
        u32 r1;
        u32 r2;
        u32 r3;
        u32 r4;
        u32 r5;
        u32 r6;
        u32 r7;
        u32 r8;
        u32 r9;
        u32 r10;
        u32 r11;
        u32 r12;

        // Stack Pointer
        u32 r13;

        // Link Register
        u32 r14;

        // Program Counter
        u32 r15;

        // Current Program Status Register
        u32 cpsr; 

        // Banked USR registers
        u32 r13_usr;
        u32 r14_usr;

        // Banked FIQ registers
        u32 r8_fiq;
        u32 r9_fiq;
        u32 r10_fiq;
        u32 r11_fiq;
        u32 r12_fiq;
        u32 r13_fiq;
        u32 r14_fiq;
        u32 spsr_fiq;

        // Banked SVC registers
        u32 r13_svc;
        u32 r14_svc;
        u32 spsr_svc;

        // Banked ABT registers
        u32 r13_abt;
        u32 r14_abt;
        u32 spsr_abt;

        // Banked IRQ registers
        u32 r13_irq;
        u32 r14_irq;
        u32 spsr_irq;

        // Banked UND registers
        u32 r13_und;
        u32 r14_und;
        u32 spsr_und;
    } regs;
    #pragma endregion

    ARM7();

    void reset();

    u32 reg(u8 number) const;
    void setReg(u8 number, u32 value);

    u32 spsr(u8 number) const;
    void setSpsr(u8 number, u32 value);

    void fetch();
    void decode();
    void execute();
    void advance();

    void step();

    MMU* mmu;
    
    // Contains memory values
    u32 pipe[3];
    // Contains decoded instructions
    Instruction pipe_instr[3];

    // Functions for updating flags
    void updateZero(u32 result);
    void updateSign(u32 result);
    void updateCarry(bool carry);
    void updateOverflow(u32 value, u32 operand, u32 result, bool addition);

    static u8 logicalShiftLeft(u32& value, u8 offset);
    static u8 logicalShiftRight(u32& value, u8 offset);
    static u8 arithmeticShiftRight(u32& value, u8 offset);

    void moveShiftedRegister(u16 instr);
    void addSubtract(u16 instr);
    void moveCompareAddSubtractAddImmediate(u16 instr);
    void aluOperations(u16 instr);
    void highRegisterBranchExchange(u16 instr);
    void pcRelativeLoad(u16 instr);
    void loadStoreWithRegisterOffset(u16 instr);
    void loadStoreSignExtendedByteHalfword(u16 instr);
    void loadStoreWithImmediateOffset(u16 instr);
    void loadStoreHalfword(u16 instr);
    void spRelativeLoadStore(u16 instr);
    void loadAddress(u16 instr);
    void addOffsetToSp(u16 instr);
    void pushPopRegisters(u16 instr);
    void multipleLoadStore(u16 instr);
    void conditionalBranch(u16 instr);
    void softwareInterrupt(u16 instr);
    void unconditionalBranch(u16 instr);
    void longBranchWithLink(u16 instr);
};
