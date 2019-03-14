#include "arm.h"

/**
 * Todo
 * - should BX align?
 * - PC in ARM 1
 * - process shift carry in data processing
 */

#include "common/log.h"
#include "common/utility.h"

// ARM 1
void ARM::dataProcessing(u32 instr)
{
    // Immediate operand flag
    u8 i = instr >> 25 & 0x1;
    // Operation code
    u8 opcode = instr >> 21 & 0xF;
    // Set conditions flag
    u8 s = instr >> 20 & 0x1;
    // First operand register
    u8 rn = instr >> 16 & 0xF;
    // Destination register
    u8 rd = instr >> 12 & 0xF;
    // Second operand
    u32 op2 = instr & 0xFFF;

    u32 op1 = regs[rn];
    u32& dst = regs[rd];

    bool carry;
    if (i)
    {
        // Immediate 8-bit value
        u8 imm = op2 & 0xFF;
        // Rotation applied to the immediate value
        u8 rotate = op2 >> 8 & 0xF;

        // Twice the rotation is applied
        rotate *= 2;

        // RRX does not exist in this case
        if (rotate == 0)
        {
            op2 = imm;
        }
        else
        {
            bool carry;
            op2 = ror(imm, rotate, carry);
        }
    }
    else
    {
        // Second operator register
        u8 rm = op2 & 0xF;
        // Shift applied to register
        u8 shift = op2 >> 4 & 0xFF;

        // Shift offset
        u8 offset;

        if (shift & 0x1)
        {
            // Shift register
            u8 rs = shift >> 4 & 0xF;
            // Offset is stored in the lower byte
            offset = regs[rs] & 0xFF;
        }
        else
        {
            // Offset is a 5-bit immediate value
            offset = shift >> 3 & 0x1FF;
        }

        switch (shift >> 1 & 0x3)
        {
        case 0b00: op2 = lsl(regs[rm], offset, carry); break;
        case 0b01: op2 = lsr(regs[rm], offset, carry); break;
        case 0b10: op2 = asr(regs[rm], offset, carry); break;
        case 0b11: op2 = ror(regs[rm], offset, carry); break;
        }
    }

    // Writing to PC
    if (s && rd == 15)
    {
        // Move current SPSR into CPSR
        if (regs.spsr)
            regs.cpsr = *regs.spsr;
        else
            log() << "SPSR in null in mode " << (int)regs.mode();
    }

    switch (opcode)
    {
    // AND
    case 0b0000:
        dst = op1 & op2;
        if (s) logical(dst);
        break;

    // EOR
    case 0b0001:
        dst = op1 ^ op2;
        if (s) logical(dst);
        break;

    // SUB
    case 0b0010:
        dst = op1 - op2;
        if (s) arithmetic(op1, op2, false);
        break;

    // RSB
    case 0b0011:
        dst = op2 - op1;
        if (s) arithmetic(op2, op1, false);
        break;

    // ADD
    case 0b0100:
        dst = op1 + op2;
        if (s) arithmetic(op1, op2, true);
        break;

    // ADC
    case 0b0101:
        op2 += regs.c();
        dst = op1 + op2;
        if (s) arithmetic(op1, op2, true);
        break;

    // SBC
    case 0b0110:
        op2 += regs.c() ? 0 : 1;
        dst = op1 - op2;
        if (s) arithmetic(op1, op2, false);
        break;

    // RBC
    case 0b0111:
        op1 += regs.c() ? 0 : 1;
        dst = op2 - op1;
        if (s) arithmetic(op2, op1, false);
        break;

    // TST
    case 0b1000:
        logical(op1 & op2);
        break;

    // TEQ
    case 0b1001:
        logical(op1 ^ op2);
        break;

    // CMP
    case 0b1010:
        arithmetic(op1, op2, false);
        break;

    // CMN
    case 0b1011:
        arithmetic(op1, op2, true);
        break;

    // ORR
    case 0b1100:
        dst = op1 | op2;
        if (s) logical(dst);
        break;

    // MOV
    case 0b1101:
        dst = op2;
        if (s) logical(dst);
        break;

    // BIC
    case 0b1110:
        dst = op1 & ~op2;
        if (s) logical(dst);
        break;

    // MVN
    case 0b1111:
        dst = ~op2;
        if (s) logical(dst);
        break;
    }
}

// ARM 5
void ARM::branchExchange(u32 instr)
{
    u8 rn = instr & 0xF;
    
    // Operation is undefined for R15
    if (rn > 14)
    {
        log() << "Invalid register " << (int)rn;
        return;
    }

    u32 addr = regs[rn];

    // Exchange instruction set
    if (addr & 0x1)
    {
        regs.setThumb(true);

        // Align and clear thumb bit
        align16(addr);
    }
    else
    {
        align32(addr);
    }

    regs.pc = addr;
    needs_flush = true;
}

// ARM 11
void ARM::branchLink(u32 instr)
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
        regs.lr = regs.pc - 4;
    }

    regs.pc += signed_offset;
    needs_flush = true;
}
