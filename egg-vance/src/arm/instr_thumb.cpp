#include "arm.h"

/**
 * Todo
 * - LDR / STR with odd offset behave weirdly
 * - write test for BX
 * - process SWI in conditional branch
 */

#include "common/log.h"
#include "common/utility.h"

// THUMB 1
void ARM::moveShiftedRegister(u16 instr)
{
    u8 opcode = instr >> 11 & 0x3;
    u8 offset = instr >> 6 & 0x1F;
    u8 rs = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    switch (opcode)
    {
    case 0b00: 
        dst = LSL(src, offset); 
        break;

    case 0b01: 
        dst = LSR(src, offset); 
        break;

    case 0b10: 
        dst = ASR(src, offset); 
        break;

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

    u32 src = regs[rs];
    u32& dst = regs[rd];

    u32 operand = i ? offset : regs[offset];
    
    switch (opcode)
    {
    case 0b0: 
        dst = ADD(src, operand); 
        break;

    case 0b1: 
        dst = SUB(src, operand); 
        break;
    }
}

// THUMB 3
void ARM::moveCmpAddSubImmediate(u16 instr)
{
    u8 opcode = instr >> 11 & 0x3;
    u8 rd = instr >> 8 & 0x7;
    u8 offset = instr & 0xFF;

    u32& dst = regs[rd];

    switch (opcode)
    {
    case 0b00: 
        dst = MOV(offset); 
        break;

    case 0b01: 
        CMP(dst, offset); 
        break;

    case 0b10: 
        dst = ADD(dst, offset); 
        break;

    case 0b11: 
        dst = SUB(dst, offset); 
        break;
    }
}

// THUMB 4
void ARM::aluOperations(u16 instr)
{
    u8 opcode = instr >> 6 & 0xF;
    u8 rs = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    switch (opcode)
    {
    case 0b0000: 
        dst = AND(dst, src); 
        break;

    case 0b0001: 
        dst = EOR(dst, src); 
        break;

    case 0b0010: 
        dst = LSL(dst, src); 
        break;

    case 0b0011: 
        dst = LSR(dst, src); 
        break;

    case 0b0100: 
        dst = ASR(dst, src); 
        break;

    case 0b0101: 
        dst = ADC(dst, src); 
        break;

    case 0b0110: 
        dst = SBC(dst, src); 
        break;

    case 0b0111:
        if (src != 0)
        {
            dst = ROR(dst, src); break;
        }
        else
        {
            // Using ROR #0 only sets Z, N and does not change the dst
            updateZ(dst);
            updateN(dst);
        }
        break;

    case 0b1000:       
        TST(dst, src); 
        break;

    case 0b1001: 
        dst = NEG(src); 
        break;

    case 0b1010:
        CMP(dst, src); 
        break;

    case 0b1011:       
        CMN(dst, src); 
        break;

    case 0b1100: 
        dst = ORR(dst, src); 
        break;

    case 0b1101: 
        dst = MUL(dst, src);
        break;

    case 0b1110: 
        dst = BIC(dst, src); 
        break;

    case 0b1111: 
        dst = MVN(src); 
        break;
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

    u32 src = regs[rs];
    u32& dst = regs[rd];

    switch (opcode)
    {
    case 0b00: 
        dst = ADD(dst, src, false); 
        break;

    case 0b01:
        CMP(dst, src); 
        break;

    case 0b10: 
        dst = MOV(src, false); 
        break;

    case 0b11:
        BX(src); 
        break;
    }
}

// THUMB 6
void ARM::loadPcRelative(u16 instr)
{
    u8 rd = instr >> 8 & 0x7;
    u16 offset = instr & 0xFF;

    // Offset is a 10-bit address
    offset <<= 2;

    u32 addr = regs.pc;
    // Bit 1 is forced to 0
    addr &= ~0x2;

    regs[rd] = LDR(addr + offset);
}

// THUMB 7
void ARM::loadStoreRegisterOffset(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    // Byte / word flag
    u8 b = instr >> 10 & 0x1;
    u8 ro = instr >> 6 & 0x7;
    u8 rb = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 addr = regs[rb] + regs[ro];
    
    align16(addr);

    u32& dst = regs[rd];

    switch (l << 1 | b)
    {
    case 0b00:
        STR(addr, dst); 
        break;

    case 0b01: 
        STRB(addr, dst); 
        break;

    case 0b10: 
        dst = LDR(addr); 
        break;

    case 0b11: 
        dst = LDRB(addr); 
        break;
    }
}

// THUMB 8
void ARM::loadStoreHalfSignExtended(u16 instr)
{
    // Half / byte flag
    u8 h = instr >> 11 & 0x1;
    // Sign extend flag
    u8 s = instr >> 10 & 0x1;
    u8 ro = instr >> 6 & 0x7;
    u8 rb = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 addr = regs[rb] + regs[ro];

    align16(addr);

    u32& dst = regs[rd];

    switch (s << 1 | h)
    {
    case 0b00: 
        STRH(addr, dst); 
        break;

    case 0b01: 
        dst = LDRH(addr); 
        break;

    case 0b10: 
        dst = LDSB(addr); 
        break;

    case 0b11: 
        dst = LDSH(addr); 
        break;
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

    u32 addr = regs[rb] + offset;

    u32& dst = regs[rd];

    switch (l << 1 | b)
    {
    case 0b00: 
        STR(addr, dst); 
        break;

    case 0b01: 
        STRB(addr, dst); 
        break;

    case 0b10: 
        dst = LDR(addr); 
        break;

    case 0b11: 
        dst = LDRB(addr); 
        break;
    }
}

// THUMB 10
void ARM::loadStoreHalf(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    u8 offset = instr >> 6 & 0x1F;
    u8 rb = instr >> 3 & 0x7;
    u8 rd = instr & 0x7;

    u32 addr = regs[rb] + offset;

    u32& dst = regs[rd];

    switch (l)
    {
    case 0b0: 
        STRH(addr, dst); 
        break;

    case 0b1: 
        dst = LDRH(addr); 
        break;
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
    u32 addr = regs.sp + offset;

    u32& dst = regs[rd];

    switch (l)
    {
    case 0b0: 
        STR(addr, dst); 
        break;

    case 0b1: 
        dst = LDR(addr); 
        break;
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

    u32& dst = regs[rd];

    switch (sp)
    {
    case 0b0: 
        // Bit 1 is read as 0
        dst = offset + (regs.pc & ~0x2); 
        break;

    case 0b1: 
        dst = offset + regs.sp;
        break;
    }
}

// THUMB 13
void ARM::addOffsetSp(u16 instr)
{
    // Sign flag
    u8 s = instr >> 7 & 0x1;
    u16 offset = instr & 0x3F;

    // Offset is a 10 bit constant
    offset <<= 2;

    switch (s)
    {
    case 0b0: 
        regs.sp += offset; 
        break;

    case 0b1: 
        regs.sp -= offset; 
        break;
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
    case 0b0: 
        PUSH(rlist, r); 
        break;

    case 0b1: 
        POP(rlist, r); 
        break;
    }
}

// THUMB 15
void ARM::loadStoreMultiple(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    u8 rb = instr >> 8 & 0x7;
    u8 rlist = instr & 0xFF;

    u32& base = regs[rb];

    switch (l)
    {
    case 0b0: 
        base = STMIA(base, rlist); 
        break;

    case 0b1: 
        base = LDMIA(base, rlist); 
        break;
    }
}

// THUMB 16
void ARM::conditionalBranch(u16 instr)
{
    Condition cond = static_cast<Condition>(instr >> 8 & 0xF);
    u8 offset = instr & 0xFF;

    if (cond == COND_AL)
    {
        log() << "Undefined branch condition AL";
    }
    else if (cond == COND_NV)
    {
        // Todo: process SWI
    }
    else if (regs.checkCondition(cond))
    {
        s16 signed_offset = twos<8>(offset);

        // Offset needs to be 9-bit with bit 0 set to 0
        signed_offset <<= 1;

        regs.pc += signed_offset;
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

    s16 signed_offset = twos<11>(offset);

    // Offset needs to be 12-bit with bit 0 set to 0
    signed_offset <<= 1;

    regs.pc += signed_offset;
    needs_flush = true;
}

// THUMB 19
void ARM::longBranchLink(u16 instr)
{
    // Low / high flag
    u8 h = instr >> 11 & 0x1;
    u16 offset = instr & 0x7FF;

    // Instruction 1
    if (!h)
    {
        s32 signed_offset = twos<11>(offset);

        // Shift offset by 12 bits
        signed_offset <<= 12;

        regs.lr = regs.pc + signed_offset;
    }
    else  // Instruction 2
    {
        u32 next = regs.pc - 2 | 1;

        regs.pc = regs.lr + (offset << 1);
        
        align16(regs.pc);

        regs.lr = next;

        needs_flush = true;
    }
}
