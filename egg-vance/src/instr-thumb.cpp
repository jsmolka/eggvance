#include "arm7.h"

#include <iostream>

void ARM7::moveShiftedRegister(u16 instr)
{
    u8 opcode = (instr >> 11) & 0b11;
    u8 offset = (instr >> 6) & 0b11111;
    u8 src = (instr >> 3) & 0b111;
    u8 dst = instr & 0b111;

    u8 carry = 0;
    u32 result = reg(src);

    switch (opcode)
    {
    case 0b00:  // LSL
        carry = logicalShiftLeft(result, offset); 
        break;

    case 0b01:  // LSR
        carry = logicalShiftRight(result, offset); 
        break;

    case 0b10:  // ASR
        carry = arithmeticShiftRight(result, offset); 
        break;

    default:
        std::cout << __FUNCTION__ << " - Invalid operation\n";
    }

    updateZero(result);
    updateSign(result);
    updateCarry(carry);

    setReg(dst, result);
}

void ARM7::addSubtract(u16 instr)
{
    u8 opcode = (instr >> 9) & 0b1;
    u32 src = (instr >> 3) & 0b111;
    u32 dst = instr & 0b111;

    // Get the immediate operand value
    u32 operand = (instr >> 6) & 0b111;
    // Check if a register should be used instead
    if ((instr >> 10) & 0b0)
        // Get the register for the immediate value
        operand = reg(operand);

    u32 src_reg = reg(src);
    u32 result = src_reg;
    
    switch (opcode)
    {
    case 0b0:  // ADD
        result += operand;
        updateCarry(result != (src_reg + operand));
        break;

    case 0b1:  // SUB
        result -= operand;
        updateCarry(result != (src_reg - operand));
        break;
    }

    updateZero(result);
    updateSign(result);
    updateOverflow(src_reg, operand, result, opcode == 0);

    setReg(dst, result);
}

void ARM7::moveCmpAddSubImmediate(u16 instr)
{
    u8 opcode = (instr >> 11) & 0b11;
    u8 dst = (instr >> 8) & 0b111;
    u8 offset = instr & 0xFF;

    u32 dst_reg = reg(dst);
    u32 result = dst_reg;

    switch (opcode)
    {
    case 0b00:  // MOV
        result = offset;
        break;

    case 0b01:  // CMP
    case 0b11:  // SUB
        result -= offset;
        updateCarry(result != (dst_reg - offset));
        updateOverflow(dst_reg, offset, result, false);
        break;

    case 0b10:  // ADD
        result += offset;
        updateCarry(result != (dst_reg - offset));
        updateOverflow(dst_reg, offset, result, true);
        break;
    }

    updateZero(result);
    updateSign(result);

    // No writeback when comparing
    if (opcode != 0b01)
        setReg(dst, result);
}

void ARM7::aluOperations(u16 instr)
{
    u8 opcode = (instr >> 6) & 0xF;
    u8 src = (instr >> 3) & 0b111;
    u8 dst = instr & 0b111;

    u32 src_reg = reg(src);
    u32 dst_reg = reg(dst);
    u32 result = dst_reg;

    switch (opcode)
    {
    case 0b0000:  // AND
    case 0b1000:  // TST
        result &= src_reg;
        break;

    case 0b0001:  // EOR
        result ^= src_reg;
        break;

    case 0b0010:  // LSL
    {
        u8 carry = logicalShiftLeft(result, src_reg);
        updateCarry(carry);
        break;
    }

    case 0b0011:  // LSR
    {
        u8 carry = logicalShiftLeft(result, src_reg);
        updateCarry(carry);
        break;
    }

    case 0b0100:  // ASR
    {
        u8 carry = arithmeticShiftRight(result, src_reg);
        updateCarry(carry);
        break;
    }

    case 0b0101:  // ADC
    {
        u8 carry = (regs.cpsr & CPSR_C) ? 1 : 0;
        result += (src_reg + carry);
        updateCarry(result != (dst_reg + src_reg + carry));
        updateOverflow(dst_reg, src_reg + carry, result, true);
        break;
    }

    case 0b0110:  // SBC
    {
        // NOT carry
        u8 carry = (regs.cpsr & CPSR_C) ? 0 : 1;
        result -= (src_reg + carry);
        updateCarry(result != (dst_reg - src_reg - carry));
        updateOverflow(dst_reg, src_reg + carry, result, false);
        break;
    }

    case 0b0111:  // ROR
    {
        u8 carry = rotateRight(result, src_reg);
        updateCarry(carry);
        break;
    }

    case 0b1010:  // CMP
        result -= src_reg;
        updateCarry(result != (dst_reg - src_reg));
        updateOverflow(dst_reg, src_reg, result, true);
        break;

    case 0b1011:  // CMN
        result += src_reg;
        updateCarry(result != (dst_reg - src_reg));
        break;

    case 0b1100:  // ORR
        result |= src_reg;
        break;

    case 0b1101:  // MUL
        result *= src_reg;
        updateCarry(result != (dst_reg * src_reg));
        // Todo: overflow?
        break;

    case 0b1110:  // BIC
        result &= ~src_reg;
        break;

    case 0b1111:  // MVN
        result = ~src_reg;
        break;
    }

    updateZero(result);
    updateSign(result);

    // Writeback for almost all opcodes
    if (opcode != 0b1000
            && opcode != 0b1010
            && opcode != 0b1011)
        setReg(dst, result);
}

void ARM7::highRegisterBranchExchange(u16 instr)
{
    // Todo:
    // updateArithmetic (C, O)
    // updateLogical (Z, S)
}

void ARM7::pcRelativeLoad(u16 instr)
{
}

void ARM7::loadStoreWithRegisterOffset(u16 instr)
{
}

void ARM7::loadStoreSignExtendedByteHalfword(u16 instr)
{
}

void ARM7::loadStoreWithImmediateOffset(u16 instr)
{
}

void ARM7::loadStoreHalfword(u16 instr)
{
}

void ARM7::spRelativeLoadStore(u16 instr)
{
}

void ARM7::loadAddress(u16 instr)
{
}

void ARM7::addOffsetToSp(u16 instr)
{
}

void ARM7::pushPopRegisters(u16 instr)
{
}

void ARM7::multipleLoadStore(u16 instr)
{
}

void ARM7::conditionalBranch(u16 instr)
{
}

void ARM7::softwareInterrupt(u16 instr)
{
}

void ARM7::unconditionalBranch(u16 instr)
{
}

void ARM7::longBranchWithLink(u16 instr)
{
}