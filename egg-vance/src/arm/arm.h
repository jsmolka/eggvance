#pragma once

#include "utility/integer.h"

class ARM
{
public:
    // Ref: Datasheet p. 17
    enum Mode
    {
        MODE_USR,  // Normal program execution state
        MODE_FIQ,  // Support data transfer or channel process
        MODE_IRQ,  // Generl purpose interrupt handling
        MODE_SVC,  // Protected mode for the operating system
        MODE_ABT,  // Entered after data or insruction prefetch abort
        MODE_SYS,  // Privileged user mode for the operating system
        MODE_UND   // Entered when an undefined instruction is executed
    } mode;

    // Ref: Datasheet p. 17
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

        // Subroutine Link Register (SLR)
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

        // Banked FIQ registers
        u32 r8_fiq;
        u32 r9_fiq;
        u32 r10_fiq;
        u32 r11_fiq;
        u32 r12_fiq;
        u32 r13_fiq;
        u32 r14_fiq;

        // Banked USR registers
        u32 r13_usr;
        u32 r14_usr;

        // Banked IRQ registers
        u32 r13_irq;
        u32 r14_irq;

        // Banked SVC registers
        u32 r13_svc;
        u32 r14_svc;

        // Banked ABT registers
        u32 r13_abt;
        u32 r14_abt;

        // Banked UND registers
        u32 r13_und;
        u32 r14_und;
    } regs;

    ARM();

    void reset();

    // Ref: Datasheet p. 18
    u32 reg(u8 number) const;
    void setReg(u8 number, u32 value);

    // Todo: State Program Status Registers
};

