#include "arm.h"

/**
 * Todo
 * - should BX align?
 * - PC in ARM 1
 */

#include "common/log.h"
#include "common/utility.h"

// ARM 1
void ARM::dataProcessing(u32 instr)
{
    // Immediate flag
    u8 i = instr >> 25 & 0x1;
    u8 opcode = instr >> 21 & 0xF;
    // Set conditions flag
    u8 s = instr >> 20 & 0x1;
    u8 rn = instr >> 16 & 0xF;
    u8 rd = instr >> 12 & 0xF;
    u32 op2 = instr & 0xFFF;

    u32 op1 = regs[rn];
    u32& dst = regs[rd];

    if (i)
    {
        // Operand 2 is an immediate value with applied rotate
        u8 imm = op2 & 0xFF;
        u8 rot = op2 >> 8 & 0xF;

        // No RRX special case here
        if (rot != 0)
            op2 = ROR(imm, 2 * rot, false);
        else
            op2 = imm;
    }
    else
    {
        // Operand 2 is a register with applied shift
        u8 rm = op2 & 0xF;
        u8 shift = op2 >> 4 & 0xFF;

        u8 offset;
        // Process different shift formats
        if (shift & 0x1)
        {
            // Offset is stored in the lower byte of rs
            u8 rs = shift >> 4 & 0xF;
            offset = regs[rs] & 0xFF;
        }
        else
        {
            // Offset is a 5-bit immediate value
            offset = shift >> 3 & 0x1FF;
        }

        switch (shift >> 1 & 0x3)
        {
        case 0b00: op2 = LSL(regs[rm], offset, false); break;
        case 0b01: op2 = LSR(regs[rm], offset, false); break;
        case 0b10: op2 = ASR(regs[rm], offset, false); break;
        case 0b11: op2 = ROR(regs[rm], offset, false); break;
        }
    }

    switch (opcode)
    {
    case 0b0000:
        dst = AND(op1, op2, s);
        break;

    case 0b0001:
        dst = EOR(op1, op2, s);
        break;

    case 0b0010:
        dst = SUB(op1, op2, s);
        break;

    case 0b0011:
        dst = SUB(op2, op1, s);
        break;

    case 0b0100:
        dst = ADD(op1, op2, s);
        break;

    case 0b0101:
        dst = ADC(op1, op2, s);
        break;

    case 0b0110:
        dst = SBC(op1, op2, s);
        break;

    case 0b0111:
        dst = SBC(op2, op1, s);
        break;

    case 0b1000:
        TST(op1, op2);
        break;

    case 0b1001:
        TEQ(op1, op2);
        break;

    case 0b1010:
        CMP(op1, op2);
        break;

    case 0b1011:
        CMN(op1, op2);
        break;

    case 0b1100:
        dst =ORR(op1, op2, s);
        break;

    case 0b1101:
        dst = MOV(op2, s);
        break;

    case 0b1110:
        dst = BIC(op1, op2, s);
        break;

    case 0b1111:
        dst = MVN(op2, s);
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
