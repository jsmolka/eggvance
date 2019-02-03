#include "arm7.h"

#include <iostream>

void ARM7::moveShiftedRegister(u16 instr)
{
    u8 opcode = (instr >> 11) & 0b11;
    u8 offset = (instr >> 6) & 0b11111;
    u8 src_reg = (instr >> 3) & 0b111;
    u8 dst_reg = instr & 0b111;

    u8 carry = 0;
    u32 result = reg(src_reg);

    switch (opcode)
    {
    case 0b00: carry = logicalShiftLeft(result, offset); break;
    case 0b01: carry = logicalShiftRight(result, offset); break;
    case 0b10: carry = arithmeticShiftRight(result, offset); break;

    default:
        std::cout << __FUNCTION__ << " - Invalid operation\n";
    }

    updateZero(result);
    updateSign(result);
    updateCarry(carry == 1);

    setReg(dst_reg, result);
}

void ARM7::addSubtract(u16 instr)
{
    u32 src_reg = reg((instr >> 3) & 0b111);
    u32 dst_reg = reg(instr & 0b111);

    // Get the immediate operand value
    u32 operand = (instr >> 6) & 0b111;
    // Check if a register should be used instead
    if ((instr >> 10) & 0)
        // Get the register for the immediate value
        operand = reg(operand);

    u32 result = 0;

    u8 opcode = (instr >> 9) & 1;
    switch (opcode)
    {
    case 0: 
        result = src_reg + operand;
        updateCarry(result < (src_reg + operand));
        break;

    case 1: 
        result -= operand;
        updateCarry(result > (src_reg - operand));
        break;
    }

    updateZero(result);
    updateSign(result);
    updateOverflow(src_reg, operand, result, opcode == 0);

    setReg(dst_reg, result);
}

void ARM7::moveCompareAddSubtractAddImmediate(u16 instr)
{
}

void ARM7::aluOperations(u16 instr)
{
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