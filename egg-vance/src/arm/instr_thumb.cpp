#include "arm.h"

#include "common/log.h"

// THUMB 1
void ARM::moveShiftedRegister(u16 instr)
{
    u8 opcode = instr >> 11 & 0x3;
    u8 offset = instr >> 6 & 0x1F;
    u8 rs = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 value = reg(rs);

    switch (opcode)
    {
    case 0b00: value = LSL(value, offset); break;
    case 0b01: value = LSR(value, offset); break;
    case 0b10: value = ASR(value, offset); break;

    default:
        log() << "Invalid operation " << (int)opcode;
    }

    reg(rd) = value;
}

// THUMB 2
void ARM::addSubImmediate(u16 instr)
{
    // Immediate / register flag
    u8 i = instr >> 10 & 0x1;
    u8 opcode = instr >> 9 & 0x1;
    u8 offset = instr >> 6 & 0x7;
    u8 rs = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 value = reg(rs);
    u32 operand = i ? offset : reg(offset);
    
    switch (opcode)
    {
    case 0b0: value = ADD(value, operand); break;
    case 0b1: value = SUB(value, operand); break;
    }

    reg(rd) = value;
}

// THUMB 3
void ARM::moveCmpAddSubImmediate(u16 instr)
{
    u8 opcode = instr >> 11 & 0x3;
    u8 rd = instr >> 8 & 0x7;
    u8 offset = instr & 0xFF;

    u32 value = reg(rd);
    u32 operand = offset;

    switch (opcode)
    {
    case 0b00: value = MOV(value); break;
    case 0b01: CMP(value, operand); return;
    case 0b10: value = ADD(value, operand); break;
    case 0b11: value = SUB(value, operand); break;
    }

    reg(rd) = value;
}

// THUMB 4
void ARM::aluOperations(u16 instr)
{
    u8 opcode = instr >> 6 & 0xF;
    u8 rs = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 value = reg(rd);
    u32 operand = reg(rs);

    switch (opcode)
    {
    case 0b0000: value = AND(value, operand); break;
    case 0b0001: value = EOR(value, operand); break;
    case 0b0010: value = LSL(value, operand); break;
    case 0b0011: value = LSR(value, operand); break;
    case 0b0100: value = ASR(value, operand); break;
    case 0b0101: value = ADC(value, operand); break;
    case 0b0110: value = SBC(value, operand); break;
    case 0b0111: value = ROR(value, operand); break;
    case 0b1000: TST(value, operand); return;
    case 0b1001: value = NEG(operand); break;
    case 0b1010: CMP(value, operand); return;
    case 0b1011: CMN(value, operand); return;
    case 0b1100: value = ORR(value, operand); break;
    case 0b1101: value = MUL(value, operand); break;
    case 0b1110: value = BIC(value, operand); break;
    case 0b1111: value = MVN(operand); break;
    }

    reg(rd) = value;
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

// THUMB 9
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

    u32 addr = reg(rb) + offset;

    switch (l << 1 | b)
    {
    case 0b00: STRW(addr, reg(rd)); break;
    case 0b01: STRB(addr, reg(rd)); break;
    case 0b10: reg(rd) = LDRW(addr); break;
    case 0b11: reg(rd) = LDRB(addr); break;
    }
}

void ARM::loadStoreHalfword(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    u8 offset = instr >> 6 & 0x1F;
    u8 rb = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 addr = reg(rb) + offset;

    switch (l)
    {
    case 0b0: STRH(addr, reg(rd)); break;
    case 0b1: reg(rd) = LDRH(addr); break;
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

// THUMB 16
void ARM::conditionalBranch(u16 instr)
{
    Condition cond = static_cast<Condition>(instr >> 8 & 0xF);
    u8 offset = instr & 0xFF;

    if (checkBranchCondition(cond))
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

        regs.pc() += signed_offset;
        needs_flush = true;
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