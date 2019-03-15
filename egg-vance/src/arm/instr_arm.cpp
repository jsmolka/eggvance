#include "arm.h"

/**
 * Todo
 * - ARM 1: special shift cases
 * - ARM 1: using PC as operan
 * - ARM 11: sign extension still needed with new code?
 */

#include "common/log.h"
#include "common/utility.h"

u32 ARM::dataProcessingShift(u16 value, bool& carry)
{
    // Shift data
    u8 shift = (value >> 4) & 0xFF;
    // Source register
    u8 rm = value & 0xF;

    u8 offset;
    if (shift & 0x1)
    {
        // Shift register
        u8 rs = (shift >> 4) & 0xF;
        // Offset is stored in the lower byte
        offset = regs[rs] & 0xFF;
    }
    else
    {
        // Offset is a 5-bit immediate value
        offset = (shift >> 3) & 0x1F;
    }

    switch ((shift >> 1) & 0x3)
    {
    case 0b00: return lsl(regs[rm], offset, carry);
    case 0b01: return lsr(regs[rm], offset, carry);
    case 0b10: return asr(regs[rm], offset, carry);
    case 0b11: return ror(regs[rm], offset, carry);
    }
}

// ARM 1
void ARM::dataProcessing(u32 instr)
{
    // Immediate operand flag
    u8 i = (instr >> 25) & 0x1;
    // Operation code
    u8 opcode = (instr >> 21) & 0xF;
    // Set conditions flag
    u8 s = (instr >> 20) & 0x1;
    // First operand register
    u8 rn = (instr >> 16) & 0xF;
    // Destination register
    u8 rd = (instr >> 12) & 0xF;
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
        u8 rotate = (op2 >> 8) & 0xF;

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
        // Apply shift to second operand
        op2 = dataProcessingShift(op2, carry);
    }

    // Writing to PC
    if (s && rd == 15)
    {
        // Move current SPSR into CPSR
        if (regs.spsr)
            regs.cpsr = *regs.spsr;

        // Do not set flags
        s = 0;
    }

    switch (opcode)
    {
    // AND
    case 0b0000:
        dst = op1 & op2;
        if (s) logical(dst, carry);
        break;

    // EOR
    case 0b0001:
        dst = op1 ^ op2;
        if (s) logical(dst, carry);
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
        logical(op1 & op2, carry);
        break;

    // TEQ
    case 0b1001:
        logical(op1 ^ op2, carry);
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
        if (s) logical(dst, carry);
        break;

    // MOV
    case 0b1101:
        dst = op2;
        if (s) logical(dst, carry);
        break;

    // BIC
    case 0b1110:
        dst = op1 & ~op2;
        if (s) logical(dst, carry);
        break;

    // MVN
    case 0b1111:
        dst = ~op2;
        if (s) logical(dst, carry);
        break;
    }
}

// ARM 2
void ARM::multiply(u32 instr)
{
    // Accumulate flag
    u8 a = (instr >> 21) & 0x1;
    // Set conditions flag
    u8 s = (instr >> 20) & 0x1;
    // Destination register
    u8 rd = (instr >> 16) & 0xF;
    // Operand registers
    u8 rn = (instr >> 12) & 0xF;
    u8 rs = (instr >> 8) & 0xF;
    u8 rm = instr & 0xF;

    if (rd == rm || rd == 15 || rm == 15)
        log() << "Handle me!";

    u32& dst = regs[rd];
    u32 op1 = regs[rm];
    u32 op2 = regs[rs];

    // Multiply (MUL)
    dst = op1 * op2;

    // Accumulate (MLA)
    if (a) 
        dst += regs[rn];

    // Todo: "meaningless carry value"
    if (s)
        logical(dst, false);
}

// ARM 3
void ARM::multiplyLong(u32 instr)
{
    // Signed / unsigned flag
    u8 u = (instr >> 22) & 0x1;
    // Accumulate flag
    u8 a = (instr >> 21) & 0x1;
    // Set conditions flag
    u8 s = (instr >> 20) & 0x1;
    // Source / destination registers
    u8 rd_hi = (instr >> 16) & 0xF;
    u8 rd_lo = (instr >> 12) & 0xF;
    // Operand registers
    u8 rs = (instr >> 8) & 0xF;
    u8 rm = instr & 0xF;

    if (rs == 15 || rm == 15 || rd_hi == 15 || rd_lo == 15)
        log() << "Handle me!";

    u32& dst_hi = regs[rd_hi];
    u32& dst_lo = regs[rd_lo];
    u32 op1 = regs[rm];
    u32 op2 = regs[rs];

    u64 result;
    if (u)
    {
        // Treat operands as two's complement
        s32 op1s = twos<32>(op1);
        s32 op2s = twos<32>(op2);

        // Multiply signed (SMULL)
        s64 result_signed = op1s * op2s;

        // Accumulate signed (SMLAL)
        if (a)
        {
            u64 acc = ((u64)dst_hi << 32) | dst_lo;
            s64 acc_signed = acc;

            // Todo: most likely false
            if (acc_signed & ((u64)1 << 63))
            {
                acc_signed = ~acc_signed;
                acc_signed++;
                acc_signed *= -1;
            }

            result_signed += acc_signed;
        }

        // Convert to two's complement
        result = ~result_signed + 1;
    }
    else
    {
        // Multiply unsigned (UMULL)
        result = op1 * op2;

        // Accumulate unsigned (UMLAL)
        if (a)
            result += ((u64)dst_hi << 32) | dst_lo;
    }

    if (s)
    {
        regs.setZ(result == 0);
        regs.setN(result >> 63);
    }

    dst_hi = result >> 32;
    dst_lo = result & 0xFFFFFFFF;
}

// ARM 4
void ARM::singleDataTransfer(u32 instr)
{
    // Register / immediate flag
    u8 i = (instr >> 25) & 0x1;
    // Pre / post indexing flag
    u8 p = (instr >> 24) & 0x1;
    // Up / down flag
    u8 u = (instr >> 23) & 0x1;
    // Byte / word flag
    u8 b = (instr >> 22) & 0x1;
    // Writeback flag
    u8 w = (instr >> 21) & 0x1;
    // Load / store flag
    u8 l = (instr >> 20) & 0x1;
    // Base register
    u8 rn = (instr >> 16) & 0xF;
    // Source / destination register
    u8 rd = (instr >> 12) & 0xF;
    // 12-bit immediate value / shifted register
    u32 offset = instr & 0x3FF;

    if (i)
    {
        bool carry;
        // Offset is a shifted register
        offset = dataProcessingShift(offset, carry);
    }

    u32 addr = u 
        ? regs[rn] + offset 
        : regs[rn] - offset;
}

// ARM 5
void ARM::branchExchange(u32 instr)
{
    // Operand register
    u8 rn = instr & 0xF;
    
    // Operation is undefined for R15
    if (rn > 14)
        log() << "Handle me!";

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
    u8 l = (instr >> 24) & 0x1;
    // 24-bit immediate value
    u32 offset = instr & 0xFFFFFF;

    s32 signed_offset = twos<24>(offset);

    // Shift left by two bits
    signed_offset <<= 2;

    //// Convert two's complement
    //if (offset & 1 << 25)
    //{
    //    offset = ~offset;
    //    offset++;

    //    // Sign extend
    //    offset |= 1 << 31;

    //    signed_offset = offset;
    //}

    if (l)
    {
        // Save address of next instruction
        regs.lr = regs.pc - 4;
    }

    regs.pc += signed_offset;
    needs_flush = true;
}
