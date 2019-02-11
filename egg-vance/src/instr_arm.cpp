#include "arm7.h"

#include <iostream>

// BX
void ARM7::branchExchange(u32 instr)
{
    u8 rn = instr & 0xF;

    // This operation is undefined for R15
    if (rn <= 14)
    {
        u32 operand = reg(rn);

        // Exchange instruction set
        if (operand & 0b1)
        {
            regs.cpsr |= CPSR_T;

            // Clear THUMB bit
            operand &= ~0b1;
        }

        regs.r15 = operand;
        needs_flush = true;
    }
    else
    {
        std::cout << __FUNCTION__ << " - Invalid register " << (int)rn << "\n";
    }
}

// B, BL
void ARM7::branchLink(u32 instr)
{
    // Link flag
    u8 l = instr >> 24 & 0x1;
    u32 offset = instr & 0xFFFFFF;

    // Shift left by two bits
    offset <<= 2;

    s32 signed_offset = offset;

    // Convert two's complement
    if (offset & 1 << 25)
    {
        offset = ~offset;
        offset++;

        // Sign extend
        offset |= 1 << 31;

        signed_offset = offset;
    }

    if (l)
    {
        // Save old PC in link register
        setReg(14, regs.r15 - 4);
    }

    regs.r15 += signed_offset;
    needs_flush = true;
}
