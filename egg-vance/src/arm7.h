#pragma once

#include "integer.h"
#include "mmu.h"

class ARM7
{
public:
    #pragma region Enums
    enum State
    {
        STATE_ARM   = 0,
        STATE_THUMB = 1
    } state;

    enum Mode
    {
        MODE_USR = 0b10000,  // Normal program execution state
        MODE_FIQ = 0b10001,  // Support data transfer or channel process
        MODE_IRQ = 0b10010,  // Generl purpose interrupt handling
        MODE_SVC = 0b10011,  // Protected mode for the operating system
        MODE_ABT = 0b10111,  // Entered after data or insruction prefetch abort
        MODE_SYS = 0b11111,  // Privileged user mode for the operating system
        MODE_UND = 0b11011   // Entered when an undefined instruction is executed
    } mode;

    enum Control
    {
        CTRL_MODE  = 0b11111,   // 5 Mode bits
        CTRL_STATE = (1 << 5),  // State bit (1 = THUMB mode)
        CTRL_FIQ   = (1 << 6),  // FIQ disable
        CTRL_IRQ   = (1 << 7)   // IRQ disable
    };

    enum Flags
    {
        FLAG_Q = (1 << 27),  // Sticky overflow
        FLAG_V = (1 << 28),  // Overflow
        FLAG_C = (1 << 29),  // Underflow
        FLAG_Z = (1 << 30),  // Zero
        FLAG_N = (1 << 31)   // Negative / less than
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
    #pragma endregion

    #pragma region Registers
    struct Registers
    {
        // General purpose registers
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

        // Stack Pointer (SP)
        u32 r13;

        // Link Register (LR)
        // Receives a copy of R15 when a Branch and Link instruction is 
        // executed
        u32 r14;

        // Program Counter (PC)
        // In ARM state: bits [0:1] are 0 and bits [2:31] contain the PC
        // in THUMB state: bit [0] is 0 and bits [1:31] contain PC
        u32 r15;

        // Current Program Status Register (CPSR)
        // Contains condition code flags and current mode bits
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

    void step();

    MMU* mmu;
    
    u32 pipeline[3];

    void aluOperations(u16 instruction);
    void highRegisterBranchExchange(u16 instruction);
    void pcRelativeLoad(u16 instruction);
    void loadStoreWithRegisterOffset(u16 instruction);
    void loadStoreSignExtendedByteHalfword(u16 instruction);
    void loadStoreWithImmediateOffset(u16 instruction);
    void loadStoreHalfword(u16 instruction);
    void spRelativeLoadStore(u16 instruction);
    void loadAddress(u16 instruction);
    void addOffsetToSp(u16 instruction);
    void pushPopRegisters(u16 instruction);
    void multipleLoadStore(u16 instruction);
    void conditionalBranch(u16 instruction);
    void softwareInterrupt(u16 instruction);
    void unconditionalBranch(u16 instruction);
    void longBranchWithLink(u16 instrution);
};
