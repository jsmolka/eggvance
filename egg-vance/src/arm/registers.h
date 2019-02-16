#pragma once

#include "common/integer.h"

class Registers
{
public:
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
};
