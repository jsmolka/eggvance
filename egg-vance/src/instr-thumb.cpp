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
    // LSL
    case 0b00:
        carry = logicalShiftLeft(result, offset); 
        break;

    // LSR
    case 0b01:
        carry = logicalShiftRight(result, offset); 
        break;

    // ASR
    case 0b10:
        carry = arithmeticShiftRight(result, offset); 
        break;

    default:
        std::cout << __FUNCTION__ << " - Invalid operation\n";
    }

    updateFlagZ(result);
    updateFlagN(result);
    updateFlagC(carry);

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

    u32 input = reg(src);
    u32 result = input;
    
    switch (opcode)
    {
    // ADD
    case 0b0:
        result += operand;
        break;

    // SUB
    case 0b1:
        result -= operand;
        break;
    }

    updateFlagZ(result);
    updateFlagN(result);
    updateFlagC(input, operand, opcode == 0b0);
    updateFlagV(input, operand, opcode == 0b0);

    setReg(dst, result);
}

void ARM7::moveCmpAddSubImmediate(u16 instr)
{
    u8 opcode = (instr >> 11) & 0b11;
    u8 dst = (instr >> 8) & 0b111;
    u8 offset = instr & 0xFF;

    u32 input = reg(dst);
    u32 result = input;

    bool writeback = true;

    switch (opcode)
    {
    // MOV
    case 0b00:
        result = offset;
        break;

    // CMP
    case 0b01:  
        // CMP is the same as SUB but without writeback
        writeback = false;

    // SUB
    case 0b11:
        result -= offset;
        updateFlagC(input, offset, false);
        updateFlagV(input, offset, false);
        break;
    
    // ADD
    case 0b10:
        result += offset;
        updateFlagC(input, offset, true);
        updateFlagV(input, offset, true);
        break;
    }

    updateFlagZ(result);
    updateFlagN(result);

    if (writeback)
        setReg(dst, result);
}

void ARM7::aluOperations(u16 instr)
{
    u8 opcode = (instr >> 6) & 0xF;
    u8 src = (instr >> 3) & 0b111;
    u8 dst = instr & 0b111;

    u32 input = reg(dst);
    u32 operand = reg(src);
    u32 result = input;

    bool writeback = true;

    switch (opcode)
    {
    // TST
    case 0b1000:
        // TST is same as AND but without writeback
        writeback = false;

    // AND
    case 0b0000:
        result &= operand;
        break;

    // EOR
    case 0b0001:
        result ^= operand;
        break;

    // LSL
    case 0b0010:
    {
        u8 carry = logicalShiftLeft(result, operand);
        updateFlagC(carry);
        break;
    }

    // LSR
    case 0b0011:
    {
        u8 carry = logicalShiftLeft(result, operand);
        updateFlagC(carry);
        break;
    }

    // ASR
    case 0b0100:
    {
        u8 carry = arithmeticShiftRight(result, operand);
        updateFlagC(carry);
        break;
    }

    // ADC
    case 0b0101:
        operand += flagC();
        result += operand;
        updateFlagC(input, operand, true);
        updateFlagV(input, operand, true);
        break;

    // SBC
    case 0b0110:
        // NOT carry
        operand += !flagC();
        result -= operand;
        updateFlagC(input, operand, false);
        updateFlagV(input, operand, false);
        break;

    // ROR
    case 0b0111:
    {
        u8 carry = rotateRight(result, operand);
        updateFlagC(carry);
        break;
    }

    // CMP
    case 0b1010:
        result -= operand;
        updateFlagC(input, operand, false);
        updateFlagV(input, operand, false);
        writeback = false;
        break;

    // CMN
    case 0b1011:
        result += operand;
        updateFlagC(input, operand, true);
        updateFlagV(input, operand, true);
        writeback = false;
        break;

    // ORR
    case 0b1100:
        result |= operand;
        break;

    // MUL
    case 0b1101:
        // Todo: check if MUL sets the C / V flags
        result *= operand;
        break;

    // BIC
    case 0b1110:
        result &= ~operand;
        break;

    // MVN
    case 0b1111:
        result = ~operand;
        break;
    }

    updateFlagZ(result);
    updateFlagN(result);

    if (writeback)
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