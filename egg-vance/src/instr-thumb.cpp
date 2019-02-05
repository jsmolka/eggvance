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

    updateFlagsZNC(result, carry);

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
        break;

    case 0b1:  // SUB
        result -= operand;
        break;
    }

    updateFlagsZNCV(src_reg, operand, result, opcode == 0b0);

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
        updateFlagsZN(result);
        break;

    case 0b01:  // CMP
    case 0b11:  // SUB
        result -= offset;
        updateFlagsZNCV(dst_reg, offset, result, false);
        break;

    case 0b10:  // ADD
        result += offset;
        updateFlagsZNCV(dst_reg, offset, result, true);
        break;
    }

    // No writeback for CMP
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
        updateFlagsZN(result);
        break;

    case 0b0001:  // EOR
        result ^= src_reg;
        updateFlagsZN(result);
        break;

    case 0b0010:  // LSL
    {
        u8 carry = logicalShiftLeft(result, src_reg);
        updateFlagsZNC(result, carry);
        break;
    }

    case 0b0011:  // LSR
    {
        u8 carry = logicalShiftLeft(result, src_reg);
        updateFlagsZNC(result, carry);
        break;
    }

    case 0b0100:  // ASR
    {
        u8 carry = arithmeticShiftRight(result, src_reg);
        updateFlagsZNC(result, carry);
        break;
    }

    case 0b0101:  // ADC
        result += (src_reg + flagC());
        updateFlagsZNCV(dst_reg, src_reg + flagC(), result, true);
        break;

    case 0b0110:  // SBC
        // NOT carry
        result -= (src_reg + !flagC());
        updateFlagsZNCV(dst_reg, src_reg + !flagC(), result, false);
        break;

    case 0b0111:  // ROR
    {
        u8 carry = rotateRight(result, src_reg);
        updateFlagsZNC(result, carry);
        break;
    }

    case 0b1010:  // CMP
        result -= src_reg;
        updateFlagsZNCV(dst_reg, src_reg, result, false);
        break;

    case 0b1011:  // CMN
        result += src_reg;
        updateFlagsZNCV(dst_reg, src_reg, result, true);
        break;

    case 0b1100:  // ORR
        result |= src_reg;
        updateFlagsZN(result);
        break;

    case 0b1101:  // MUL
        result *= src_reg;
        updateFlagsZN(result);
        setFlagC(result != (dst_reg * src_reg));
        // Todo: overflow?
        break;

    case 0b1110:  // BIC
        result &= ~src_reg;
        updateFlagsZN(result);
        break;

    case 0b1111:  // MVN
        result = ~src_reg;
        updateFlagsZN(result);
        break;
    }

    // Writeback for almost all opcodes
    if (opcode != 0b1000
            && opcode != 0b1010
            && opcode != 0b1011)
        setReg(dst, result);
}

void ARM7::highRegisterBranchExchange(u16 instr)
{
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