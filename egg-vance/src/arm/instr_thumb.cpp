#include "arm.h"

#include "common/log.h"

// THUMB 1
void ARM::moveShiftedRegister(u16 instr)
{
    u8 opcode = instr >> 11 & 0x3;
    u8 offset = instr >> 6 & 0x1F;
    u8 rs = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32& src = reg(rs);
    u32& dst = reg(rd);

    switch (opcode)
    {
    case 0b00: dst = LSL(src, offset); break;
    case 0b01: dst = LSR(src, offset); break;
    case 0b10: dst = ASR(src, offset); break;

    default:
        log() << "Invalid operation " << (int)opcode;
    }
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

    u32& src = reg(rs);
    u32& dst = reg(rd);

    u32 operand = i ? offset : reg(offset);
    
    switch (opcode)
    {
    case 0b0: dst = ADD(src, operand); break;
    case 0b1: dst = SUB(src, operand); break;
    }
}

// THUMB 3
void ARM::moveCmpAddSubImmediate(u16 instr)
{
    u8 opcode = instr >> 11 & 0x3;
    u8 rd = instr >> 8 & 0x7;
    u8 offset = instr & 0xFF;

    u32& dst = reg(rd);

    switch (opcode)
    {
    case 0b00: dst = MOV(     offset); break;
    case 0b01:       CMP(dst, offset); break;
    case 0b10: dst = ADD(dst, offset); break;
    case 0b11: dst = SUB(dst, offset); break;
    }
}

// THUMB 4
void ARM::aluOperations(u16 instr)
{
    u8 opcode = instr >> 6 & 0xF;
    u8 rs = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32& src = reg(rs);
    u32& dst = reg(rd);

    switch (opcode)
    {
    case 0b0000: dst = AND(dst, src); break;
    case 0b0001: dst = EOR(dst, src); break;
    case 0b0010: dst = LSL(dst, src); break;
    case 0b0011: dst = LSR(dst, src); break;
    case 0b0100: dst = ASR(dst, src); break;
    case 0b0101: dst = ADC(dst, src); break;
    case 0b0110: dst = SBC(dst, src); break;
    case 0b0111: dst = ROR(dst, src); break;
    case 0b1000:       TST(dst, src); break;
    case 0b1001: dst = NEG(     src); break;
    case 0b1010:       CMP(dst, src); break;
    case 0b1011:       CMN(dst, src); break;
    case 0b1100: dst = ORR(dst, src); break;
    case 0b1101: dst = MUL(dst, src); break;
    case 0b1110: dst = BIC(dst, src); break;
    case 0b1111: dst = MVN(     src); break;
    }
}

// THUMB 5
void ARM::highRegisterBranchExchange(u16 instr)
{
    u8 opcode = instr >> 8 & 0x3;
    // High operand flag for rd
    u8 hd = instr >> 7 & 0x1;
    // High operand flag for rs
    u8 hs = instr >> 6 & 0x1;
    u8 rs = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    // Use high registers
    rs |= (hs << 3);
    rd |= (hd << 3);

    u32 value = reg(rd);
    u32 operand = reg(rs);

    switch (opcode)
    {
    case 0b00: value = ADD(value, operand, false); break;
    case 0b01: CMP(value, operand); return;
    case 0b10: value = MOV(operand, false); break;
    case 0b11: BX(operand); break;
    }

    reg(rd) = value;
}

// THUMB 6
void ARM::loadPcRelative(u16 instr)
{
    u8 rd = instr >> 8 & 0x7;
    u16 offset = instr & 0xFF;

    // Offset is a 10-bit address
    offset <<= 2;

    u32 pc = regs.r15;
    // Bit 1 is forced to 0
    pc &= ~(1 << 1);

    reg(rd) = LDRW(pc + offset);
}

// THUMB 7
void ARM::loadStoreRegisterOffset(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    // Byte / word flag
    u8 b = instr >> 11 & 0x1;
    u8 ro = instr >> 6 & 0x7;
    u8 rb = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 addr = reg(rb) + reg(ro);

    switch (l << 1 | b)
    {
    case 0b00: STRW(addr, reg(rd)); break;
    case 0b01: STRB(addr, reg(rd)); break;
    case 0b10: reg(rd) = LDRW(addr); break;
    case 0b11: reg(rd) = LDRB(addr); break;
    }
}

// THUMB 8
void ARM::loadStoreSignExtended(u16 instr)
{
    // Half / byte flag
    u8 h = instr >> 11 & 0x1;
    // Sign extend flag
    u8 s = instr >> 10 & 0x1;
    u8 ro = instr >> 6 & 0x7;
    u8 rb = instr >> 3 & 0x7;
    u8 rd = instr >> 0x7;

    u32 addr = reg(rb) + reg(ro);

    switch (s << 1 | h)
    {
    case 0b00: STRH(addr, reg(rd)); break;
    case 0b01: reg(rd) = LDRH(addr); break;
    case 0b10: reg(rd) = LDSB(addr); break;
    case 0b11: reg(rd) = LDSB(addr); break;
    }
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

// THUMB 10
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

// THUMB 11
void ARM::loadStoreSpRelative(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    u8 rd = instr >> 8 & 0x7;
    u16 offset = instr & 0xFF;

    // Offset is a 10 bit constant
    offset <<= 2;

    // Add unsigned offset to SP
    u32 addr = reg(13) + offset;

    switch (l)
    {
    case 0b0: STRW(addr, reg(rd)); break;
    case 0b1: reg(rd) = LDRW(addr); break;
    }
}

// THUMB 12
void ARM::loadAddress(u16 instr)
{
    // SP / PC flag
    u8 sp = instr >> 11 & 0x1;
    u8 rd = instr >> 8 & 0x7;
    u16 offset = instr & 0xFF;

    // Offset is a 10 bit constant
    offset <<= 2;

    u32 value = 0;

    switch (sp)
    {
    // Bit 1 of the PC is read as 0
    case 0b0: value += regs.r15 & ~(1 << 1); break;
    case 0b1: value += reg(13); break;
    }

    reg(rd) = value + offset;
}

// THUMB 13
void ARM::addOffsetSp(u16 instr)
{
    // Sign flag
    u8 s = instr >> 7;
    u16 offset = instr & 0xFF;

    // Offset is a 10 bit constant
    offset <<= 2;
    
    switch (s)
    {
    case 0b0: reg(13) += offset; break;
    case 0b1: reg(13) -= offset; break;
    }
}

// THUMB 14
void ARM::pushPopRegisters(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    // Store LR / load PC flag
    u8 r = instr >> 8 & 0x1;
    u8 rlist = instr & 0xFF;

    switch (l)
    {
    case 0b0: PUSH(rlist, r); break;
    case 0b1: POP(rlist, r); break;
    }
}

// THUMB 15
void ARM::multipleLoadStore(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    u8 rb = instr >> 8 & 0x7;
    u8 rlist = instr & 0xFF;

    switch (l)
    {
    case 0b0: reg(rb) = STMIA(reg(rb), rlist); break;
    case 0b1: reg(rb) = LDMIA(reg(rb), rlist); break;
    }
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

        regs.r15 += signed_offset;
        needs_flush = true;
    }
}

// THUMB 17
void ARM::softwareInterruptThumb(u16 instr)
{

}

// THUMB 18
void ARM::unconditionalBranch(u16 instr)
{
    u16 offset = instr & 0x7FF;

    s16 signed_offset = offset;

    // Convert two's complement
    if (offset & 1 << 10)
    {
        offset = ~offset;
        // Work with the 11-bit offset
        offset &= 0x7FF;
        offset++;

        signed_offset = -1 * offset;
    }

    // Offset needs to be 9-bit with bit 0 set to 0
    signed_offset <<= 1;

    regs.r15 += signed_offset;
    needs_flush = true;
}

// THUMB 19
void ARM::longBranchLink(u16 instr)
{

}
