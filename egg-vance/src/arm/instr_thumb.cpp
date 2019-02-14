#include "arm.h"

#include "common/log.h"

// THUMB 1
void ARM::moveShiftedRegister(u16 instr)
{
    u8 opcode = instr >> 11 & 0x3;
    u8 offset = instr >> 6 & 0x1F;
    u8 rs = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 result = reg(rs);
    u8 carry = 0;

    switch (opcode)
    {
    // LSL
    case 0b00:
        carry = LSL(result, offset); 
        break;

    // LSR
    case 0b01:
        carry = LSR(result, offset); 
        break;

    // ASR
    case 0b10:
        carry = ASR(result, offset); 
        break;

    default:
        log() << "Invalid operation " << (int)opcode;
    }

    updateFlagZ(result);
    updateFlagN(result);
    updateFlagC(carry);

    setReg(rd, result);
}

// THUMB 2
void ARM::addSubImmediate(u16 instr)
{
    // Immediate flag
    u8 i = instr >> 10 & 0x1;
    u8 opcode = instr >> 9 & 0x1;
    u8 offset = instr >> 6 & 0x7;
    u32 rs = instr >> 3 & 0x7;
    u32 rd = instr & 0x7;

    u32 operand;
    if (i)
        // Use immediate value as operand
        operand = offset;
    else
        // Use register as operand
        operand = reg(offset);

    u32 input = reg(rs);
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

    setReg(rd, result);
}

// THUMB 3
void ARM::moveCmpAddSubImmediate(u16 instr)
{
    u8 opcode = instr >> 11 & 0x3;
    u8 rd = instr >> 8 & 0x7;
    u8 offset = instr & 0xFF;

    u32 input = reg(rd);
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
        setReg(rd, result);
}

// THUMB 4
void ARM::aluOperations(u16 instr)
{
    u8 opcode = (instr >> 6) & 0xF;
    u8 src = (instr >> 3) & 0x7;
    u8 dst = instr & 0x7;

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
        u8 carry = LSL(result, operand);
        updateFlagC(carry);
        break;
    }

    // LSR
    case 0b0011:
    {
        u8 carry = LSL(result, operand);
        updateFlagC(carry);
        break;
    }

    // ASR
    case 0b0100:
    {
        u8 carry = ASR(result, operand);
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
        u8 carry = ROR(result, operand);
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

void ARM::highRegisterBranchExchange(u16 instr)
{

}

void ARM::loadPcRelative(u16 instr)
{

}

void ARM::loadStoreRegisterOffset(u16 instr)
{

}

void ARM::loadStoreSignExtended(u16 instr)
{

}

// THUMB 9 - STR, LDR, STRB, LDRB
void ARM::loadStoreImmediateOffset(u16 instr)
{
    // Byte / word flag
    u8 b = instr >> 12 & 0x1;
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    u8 offset = instr >> 6 & 0x1F;
    u8 rb = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    if (!b)
        // Word access uses a 7-bit offset
        offset <<= 2;

    // Calculate address
    u32 addr = reg(rb) + offset;

    switch (l << 1 | b)
    {
    // STR - store word
    case 0b00:
        mmu->writeWord(addr, reg(rd));
        break;

    // LDR - load word
    case 0b10:
        setReg(rd, mmu->readWord(addr));
        break;

    // STRB - store byte
    case 0b01:
        mmu->writeByte(addr, reg(rd) & 0xFF);
        break;

    // LDRB - load byte
    case 0b11:
        setReg(rd, mmu->readByte(addr));
        break;
    }
}

void ARM::loadStoreHalfword(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    u8 offset = instr >> 6 & 0x1F;
    u8 rb = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    // Calculate address
    u32 addr = reg(rb) + offset;

    switch (l)
    {
    // Store
    case 0b0:
        mmu->writeHalf(addr, reg(rd) & 0xFFFF);
        break;

    // Load
    case 0b1:
        setReg(rd, mmu->readHalf(addr));
        break;
    }
}

void ARM::loadStoreSpRelative(u16 instr)
{

}

void ARM::loadAddress(u16 instr)
{

}

void ARM::addOffsetSp(u16 instr)
{

}

void ARM::pushPopRegisters(u16 instr)
{

}

void ARM::multipleLoadStore(u16 instr)
{

}

// THUMB 16 - B(cond)
void ARM::conditionalBranch(u16 instr)
{
    Condition cond = static_cast<Condition>(instr >> 8 & 0xF);
    u8 offset = instr & 0xFF;

    if (checkCondition(cond))
    {
        if (cond == COND_AL)
        {
            log() << "Undefined condition";
        }
        else if (cond == COND_NV)
        {
            // Todo: process SWI
        }
        else
        {
            s16 signed_offset = offset;

            // Convert two's complement
            if (offset & 1 << 7)
            {
                offset = ~offset;
                offset++;

                signed_offset = -1 * offset;
            }

            // Offset needs to be 9-bit with bit 0 set to 0
            signed_offset <<= 1;

            regs.r15 += signed_offset;

            needs_flush = true;
        }
    }
}

void ARM::softwareInterruptThumb(u16 instr)
{

}

void ARM::unconditionalBranch(u16 instr)
{

}

void ARM::longBranchLink(u16 instr)
{

}