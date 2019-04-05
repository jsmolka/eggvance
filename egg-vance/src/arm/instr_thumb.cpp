#include "arm.h"

/**
 * Todo
 * - LDR / STR with odd offset behave weirdly
 * - write test for BX
 * - process SWI in conditional branch
 * - properly name operands
 * - test ROR in Thumb 4
 * - check if RORs special cases are ok
 */

#include "common/log.h"
#include "common/utility.h"

// THUMB 1
void ARM::moveShiftedRegister(u16 instr)
{
    // Operation code
    u8 opcode = instr >> 11 & 0x3;
    // 5-bit immediate value
    u8 offset = instr >> 6 & 0x1F;
    // Source register
    u8 rs = instr >> 3 & 0x7;
    // Destination register
    u8 rd = instr & 0x7;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    bool carry;
    switch (opcode)
    {
    case 0b00: dst = lsl(src, offset, carry); break;
    case 0b01: dst = lsr(src, offset, carry); break;
    case 0b10: dst = asr(src, offset, carry); break;
    }
    logical(dst, carry);
}

// THUMB 2
void ARM::addSubImmediate(u16 instr)
{
    // Immediate / register flag
    u8 i = instr >> 10 & 0x1;
    // Operation code
    u8 opcode = instr >> 9 & 0x1;
    // 3-bit immediate value / register
    u8 offset = instr >> 6 & 0x7;
    // Source register
    u8 rs = instr >> 3 & 0x7;
    // Destination register
    u8 rd = instr & 0x7;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    // Use immediate value or register as operand
    u32 op = i ? offset : regs[offset];
    
    switch (opcode)
    {
    // ADD
    case 0b0: 
        dst = src + op;
        arithmetic(src, op, true);
        break;

    // SUB
    case 0b1: 
        dst = src - op; 
        arithmetic(src, op, false);
        break;
    }
}

// THUMB 3
void ARM::moveCmpAddSubImmediate(u16 instr)
{
    // Operation code
    u8 opcode = instr >> 11 & 0x3;
    // Source / destination register
    u8 rd = instr >> 8 & 0x7;
    // 8-bit immediate value
    u8 offset = instr & 0xFF;

    u32 src = regs[rd];
    u32& dst = regs[rd];

    switch (opcode)
    {
    // MOV
    case 0b00: 
        dst = offset;
        logical(dst);
        break;

    // CMP
    case 0b01: 
        arithmetic(src, offset, false);
        break;

    // ADD
    case 0b10: 
        dst = src + offset;
        arithmetic(src, offset, true);
        break;

    // SUB
    case 0b11: 
        dst = src - offset;
        arithmetic(src, offset, false);
        break;
    }
}

// THUMB 4
void ARM::aluOperations(u16 instr)
{
    // Operation code
    u8 opcode = instr >> 6 & 0xF;
    // Source register
    u8 rs = instr >> 3 & 0x7;
    // Source / destination register
    u8 rd = instr & 0x7;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    bool carry;
    switch (opcode)
    {
    // AND
    case 0b0000: 
        dst &= src;
        logical(dst);
        break;

    // EOR
    case 0b0001: 
        dst ^= src;
        logical(dst);
        break;

    // LSL
    case 0b0010: 
        if (src == 0)
            carry = regs.c();
        else
            dst = lsl(dst, src, carry); 
        logical(dst, carry);
        break;

    // LSR
    case 0b0011: 
        if (src == 0)
            carry = regs.c();
        else
            dst = lsr(dst, src, carry); 
        logical(dst, carry);
        break;

    // ASR
    case 0b0100:
        if (src == 0)
            carry = regs.c();
        else
            dst = asr(dst, src, carry); 
        logical(dst, carry);
        break;

    // ADC
    case 0b0101: 
        src += regs.c();
        arithmetic(dst, src, true);
        dst += src;
        break;

    // SBC
    case 0b0110: 
        src += regs.c() ? 0 : 1;
        arithmetic(dst, src, false);
        dst -= src;
        break;

    // ROR
    case 0b0111:
        if (src == 0)
            carry = regs.c();
        else
            dst = ror(dst, src, carry);
        logical(dst, carry);
        break;

    // TST
    case 0b1000:
        logical(dst & src);
        break;

    // NEG
    case 0b1001:
        dst = 0 - src;
        arithmetic(0, src, false);
        break;

    // CMP
    case 0b1010:
        arithmetic(dst, src, false);
        break;

    // CMN
    case 0b1011:       
        arithmetic(dst, src, true);
        break;

    // ORR
    case 0b1100: 
        dst |= src;
        logical(dst);
        break;

    // MUL
    case 0b1101: 
        dst *= src;
        logical(dst);
        break;

    // BIC
    case 0b1110:
        dst &= ~src;
        logical(dst);
        break;

    // MVN
    case 0b1111: 
        dst = ~src;
        logical(dst);
        break;
    }
}

// THUMB 5
void ARM::highRegisterBranchExchange(u16 instr)
{
    // Operation code
    u8 opcode = instr >> 8 & 0x3;
    // High operand flag for rd
    u8 hd = instr >> 7 & 0x1;
    // High operand flag for rs
    u8 hs = instr >> 6 & 0x1;
    // Source register
    u8 rs = instr >> 3 & 0x7;
    // Destination register
    u8 rd = instr & 0x7;

    // Use high registers
    rs |= hs << 3;
    rd |= hd << 3;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    // Only set flags in CMP
    switch (opcode)
    {
    // ADD
    case 0b00: 
        dst += src;
        break;

    // CMP
    case 0b01:
        arithmetic(dst, src, false);
        break;

    // MOV
    case 0b10: 
        dst = src;
        break;

    // BX
    case 0b11:
        if ((src & 0x1) == 0)
        {
            // Switch to ARM mode
            regs.setThumb(false);

            align_word(src);
        }
        else
        {
            align_half(src);
        }

        regs.pc = src;
        needs_flush = true;        
        break;
    }
}

// THUMB 6
void ARM::loadPcRelative(u16 instr)
{
    // Destination register
    u8 rd = instr >> 8 & 0x7;
    // 8-bit immediate value
    u16 offset = instr & 0xFF;

    // Offset is a 10-bit address
    offset <<= 2;

    u32 addr = regs.pc;
    // Bit 1 is forced to 0
    addr &= ~0x2;

    regs[rd] = mmu->readWord(addr + offset);
}

// THUMB 7
void ARM::loadStoreRegisterOffset(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    // Byte / word flag
    u8 b = instr >> 10 & 0x1;
    // Offset register
    u8 ro = instr >> 6 & 0x7;
    // Base register
    u8 rb = instr >> 3 & 0x7;
    // Source / destination register
    u8 rd = instr & 0x7;

    u32 addr = regs[rb] + regs[ro];
    align_half(addr);

    u32& dst = regs[rd];

    switch (l << 1 | b)
    {
    // STR
    case 0b00:
        mmu->writeWord(addr, dst); 
        break;

    // STRB
    case 0b01: 
        mmu->writeByte(addr, dst & 0xFF); 
        break;

    // LDR
    case 0b10: 
        dst = mmu->readWord(addr); 
        break;

    // LDRB
    case 0b11: 
        dst = mmu->readByte(addr); 
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
    // Offset register
    u8 ro = instr >> 6 & 0x7;
    // Base register
    u8 rb = instr >> 3 & 0x7;
    // Destination register
    u8 rd = instr & 0x7;

    u32 addr = regs[rb] + regs[ro];
    align_half(addr);

    u32& dst = regs[rd];

    switch (s << 1 | h)
    {
    // STRH
    case 0b00: 
        mmu->writeHalf(addr, dst & 0xFFFF); 
        break;

    // LDRH
    case 0b01: 
        dst = mmu->readHalf(addr); 
        break;

    // LDSB
    case 0b10: 
        dst = mmu->readByte(addr);
        // Extend with bit 8
        if (dst & (1 << 7))
            dst |= 0xFFFFFF00;
        break;

    // LDSH
    case 0b11: 
        dst = mmu->readHalf(addr);
        // Extend with bit 16
        if (dst & (1 << 15))
            dst |= 0xFFFF0000;
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
    // 5-bit immediate value
    u8 offset = instr >> 6 & 0x1F;
    // Base register
    u8 rb = instr >> 3 & 0x7;
    // Destination register
    u8 rd = instr & 0x7;

    if (!b)
        // Word access uses 7-bit offset
        offset <<= 2;

    u32 addr = regs[rb] + offset;
    align_half(addr);

    u32& dst = regs[rd];

    switch (l << 1 | b)
    {
    // STR
    case 0b00: 
        mmu->writeWord(addr, dst); 
        break;

    // STRB
    case 0b01: 
        mmu->writeByte(addr, dst & 0xFF); 
        break;

    // LDR
    case 0b10: 
        dst = mmu->readWord(addr); 
        break;

    // LDRB
    case 0b11: 
        dst = mmu->readByte(addr); 
        break;
    }
}

// THUMB 10
void ARM::loadStoreHalf(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    // 5-bit offset
    u8 offset = instr >> 6 & 0x1F;
    // Base register
    u8 rb = instr >> 3 & 0x7;
    // Destination register
    u8 rd = instr & 0x7;

    u32 addr = regs[rb] + offset;
    align_half(addr);

    u32& dst = regs[rd];

    switch (l)
    {
    // STRH
    case 0b0: 
        mmu->writeHalf(addr, dst & 0xFFFF); 
        break;

    // LDRH
    case 0b1: 
        dst = mmu->readHalf(addr); 
        break;
    }
}

// THUMB 11
void ARM::loadStoreSpRelative(u16 instr)
{
    // Load / store flag
    u8 l = instr >> 11 & 0x1;
    // Destination register
    u8 rd = instr >> 8 & 0x7;
    // 8-bit immediate value
    u16 offset = instr & 0xFF;

    // Offset is a 10 bit constant
    offset <<= 2;

    // Add unsigned offset to SP
    u32 addr = regs.sp + offset;
    align_half(addr);

    u32& dst = regs[rd];

    switch (l)
    {
    // STR
    case 0b0: 
        mmu->writeWord(addr, dst); 
        break;

    // LDR
    case 0b1: 
        dst = mmu->readWord(addr); 
        break;
    }
}

// THUMB 12
void ARM::loadAddress(u16 instr)
{
    // SP / PC flag
    u8 sp = instr >> 11 & 0x1;
    // Destination register
    u8 rd = instr >> 8 & 0x7;
    // 8-bit immediate value
    u16 offset = instr & 0xFF;

    // Offset is a 10 bit constant
    offset <<= 2;

    u32& dst = regs[rd];

    switch (sp)
    {
    case 0b0: 
        // Bit 1 is read as 0
        dst = (regs.pc & ~0x2) + offset;
        break;

    case 0b1: 
        dst = regs.sp + offset;
        break;
    }
}

// THUMB 13
void ARM::addOffsetSp(u16 instr)
{
    // Sign flag
    u8 s = instr >> 7 & 0x1;
    // 7-bit immediate value
    u16 offset = instr & 0x3F;

    // Offset is a 10-bit value
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
    // Register list
    u8 rlist = instr & 0xFF;

    switch (l)
    {
    // PUSH
    case 0b0: 
        // Store LR
        if (r)
        {
            regs.sp -= 4;
            mmu->writeWord(regs.sp, regs.lr);
        }

        // Iterate over specified registers
        for (int x = 7; x >= 0; --x)
        {
            if (rlist & (1 << x))
            {
                regs.sp -= 4;
                mmu->writeWord(regs.sp, regs[x]);
            }
        }
        break;

    // POP
    case 0b1: 
        // Iterate over specified registers
        for (int x = 0; x < 8; ++x)
        {
            if (rlist & 0x1)
            {
                regs[x] = mmu->readWord(regs.sp);
                regs.sp += 4;
            }
            rlist >>= 1;
        }

        // Load PC
        if (r)
        {
            regs.pc = mmu->readWord(regs.sp);

            align_half(regs.pc);

            regs.sp += 4;

            needs_flush = true;
        }
        break;
    }
}

// THUMB 15
void ARM::loadStoreMultiple(u16 instr)
{
    // Load / store flag
    bool load = instr >> 11 & 0x1;
    // Base register
    int rb = instr >> 8 & 0x7;
    // Register list
    int rlist = instr & 0xFF;

    u32& addr = regs[rb];

    for (int x = 0; x < 8; ++x)
    {
        if (rlist & 1 << x)
        {
            if (load)
                regs[x] = mmu->readWord(addr);
            else
                mmu->writeWord(addr, regs[x]);

            addr += 4;
        }
    }
}

// THUMB 16
void ARM::conditionalBranch(u16 instr)
{
    // Condition
    Condition cond = static_cast<Condition>(instr >> 8 & 0xF);
    // 8-bit immediate value
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
        align_half(regs.pc);

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
    // 11-bit immediate value
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
    // 11-bit immediate value
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
        
        align_half(regs.pc);

        regs.lr = next;

        needs_flush = true;
    }
}
