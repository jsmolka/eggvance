#include "arm.h"

/**
 * Todo
 * - implement SWI
 * - process SWI in conditional branch
 */

#include "common/log.h"
#include "common/utility.h"

// THUMB 1
void ARM::moveShiftedRegister(u16 instr)
{
    // Operation code
    int opcode = instr >> 11 & 0x3;
    // 5-bit immediate value
    int offset = instr >> 6 & 0x1F;
    // Source register
    int rs = instr >> 3 & 0x7;
    // Destination register
    int rd = instr & 0x7;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    bool carry;
    switch (opcode)
    {
    case 0b00: dst = lsl(src, offset, carry); break;
    case 0b01: dst = lsr(src, offset, carry); break;
    case 0b10: dst = asr(src, offset, carry); break;

    default:
        log() << "Invalid opcode";
    }
    logical(dst, carry);
}

// THUMB 2
void ARM::addSubImmediate(u16 instr)
{
    // Immediate / register flag
    bool use_imm = instr >> 10 & 0x1;
    // Subtract / add flag
    bool subtract = instr >> 9 & 0x1;
    // 3-bit immediate value / register
    int offset = instr >> 6 & 0x7;
    // Source register
    int rs = instr >> 3 & 0x7;
    // Destination register
    int rd = instr & 0x7;

    u32 src = regs[rs];
    u32& dst = regs[rd];
    
    u32 op = use_imm ? offset : regs[offset];

    if (subtract)
        dst = src - op;
    else
        dst = src + op;

    arithmetic(src, op, !subtract);
}

// THUMB 3
void ARM::moveCmpAddSubImmediate(u16 instr)
{
    // Operation code
    int opcode = instr >> 11 & 0x3;
    // Source / destination register
    int rd = instr >> 8 & 0x7;
    // 8-bit immediate value
    int offset = instr & 0xFF;

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
        arithmetic(dst, offset, false);
        break;

    // ADD
    case 0b10: 
        arithmetic(dst, offset, true);
        dst += offset;
        break;

    // SUB
    case 0b11: 
        arithmetic(dst, offset, false);
        dst -= offset;
        break;
    }
}

// THUMB 4
void ARM::aluOperations(u16 instr)
{
    // Operation code
    int opcode = instr >> 6 & 0xF;
    // Source register
    int rs = instr >> 3 & 0x7;
    // Source / destination register
    int rd = instr & 0x7;

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
    int opcode = instr >> 8 & 0x3;
    // High operand flag rd
    int hd = instr >> 7 & 0x1;
    // High operand flag rs
    int hs = instr >> 6 & 0x1;
    // Source register
    int rs = instr >> 3 & 0x7;
    // Destination register
    int rd = instr & 0x7;

    // Apply high operand flags
    rs |= hs << 3;
    rd |= hd << 3;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    // Only compare modifies flags
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
        if (rd == 15)
        {
            src = alignHalf(src);
            needs_flush = true;
        }
        dst = src;
        break;

    // BX
    case 0b11:
        if (src & 0x1)
        {
            src = alignHalf(src);
        }
        else
        {
            src = alignWord(src);
            // Change instruction set
            regs.setThumb(false);
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
    int rd = instr >> 8 & 0x7;
    // 8-bit immediate value
    int offset = instr & 0xFF;

    // Offset is a 10-bit immediate
    offset <<= 2;

    // Bit 1 is forced to 0
    u32 addr = regs.pc & ~0x2;

    regs[rd] = mmu->readWord(addr + offset);
}

// THUMB 7
void ARM::loadStoreRegisterOffset(u16 instr)
{
    // Load / store flag
    int load = instr >> 11 & 0x1;
    // Byte / word flag
    int byte = instr >> 10 & 0x1;
    // Offset register
    int ro = instr >> 6 & 0x7;
    // Base register
    int rb = instr >> 3 & 0x7;
    // Source / destination register
    int rd = instr & 0x7;

    u32& dst = regs[rd];

    u32 addr = regs[rb] + regs[ro];

    // Byte operations are not aligned
    switch (load << 1 | byte)
    {
    // STR
    case 0b00:
        mmu->writeWord(alignWord(addr), dst); 
        break;

    // STRB
    case 0b01: 
        mmu->writeByte(addr, dst & 0xFF); 
        break;

    // LDR
    case 0b10: 
        dst = ldr(addr);
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
    int half = instr >> 11 & 0x1;
    // Sign extend flag
    int sign = instr >> 10 & 0x1;
    // Offset register
    int ro = instr >> 6 & 0x7;
    // Base register
    int rb = instr >> 3 & 0x7;
    // Destination register
    int rd = instr & 0x7;
    
    u32& dst = regs[rd];

    u32 addr = regs[rb] + regs[ro];

    switch (sign << 1 | half)
    {
    // STRH
    case 0b00:
        mmu->writeHalf(alignHalf(addr), dst & 0xFFFF); 
        break;

    // LDRH
    case 0b01: 
        dst = ldrh(addr);
        break;

    // LDSB
    case 0b10: 
        dst = mmu->readByte(addr);
        if (dst & 1 << 7)
            dst |= 0xFFFFFF00;
        break;

    // LDSH
    case 0b11: 
        dst = ldrsh(addr);
        break;
    }
}

// THUMB 9
void ARM::loadStoreImmediateOffset(u16 instr)
{
    // Byte / word flag
    int byte = instr >> 12 & 0x1;
    // Load / store flag
    int load = instr >> 11 & 0x1;
    // 5-bit immediate value
    int offset = instr >> 6 & 0x1F;
    // Base register
    int rb = instr >> 3 & 0x7;
    // Destination register
    int rd = instr & 0x7;

    u32& dst = regs[rd];

    // Word access uses a 7-bit offset
    if (!byte)
        offset <<= 2;

    u32 addr = regs[rb] + offset;

    switch (load << 1 | byte)
    {
    // STR
    case 0b00: 
        mmu->writeWord(alignWord(addr) , dst);
        break;

    // STRB
    case 0b01: 
        mmu->writeByte(addr, dst & 0xFF); 
        break;

    // LDR
    case 0b10: 
        dst = ldr(addr);
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
    bool load = instr >> 11 & 0x1;
    // 5-bit immediate value
    int offset = instr >> 6 & 0x1F;
    // Base register
    int rb = instr >> 3 & 0x7;
    // Destination register
    int rd = instr & 0x7;

    u32& dst = regs[rd];

    // Offset is a 6-bit constant
    offset <<= 1;

    u32 addr = regs[rb] + offset;

    if (load)
        dst = ldrh(addr);
    else
        mmu->writeHalf(alignHalf(addr), dst & 0xFFFF);
}

// THUMB 11
void ARM::loadStoreSpRelative(u16 instr)
{
    // Load / store flag
    bool load = instr >> 11 & 0x1;
    // Destination register
    int rd = instr >> 8 & 0x7;
    // 8-bit immediate value
    int offset = instr & 0xFF;

    u32& dst = regs[rd];
    
    // Offset is a 10-bit constant
    offset <<= 2;

    u32 addr = regs.sp + offset;

    if (load)
        dst = ldr(addr);
    else
        mmu->writeWord(alignWord(addr), dst);
}

// THUMB 12
void ARM::loadAddress(u16 instr)
{
    // SP / PC flag
    bool sp = instr >> 11 & 0x1;
    // Destination register
    int rd = instr >> 8 & 0x7;
    // 8-bit immediate value
    int offset = instr & 0xFF;

    u32& dst = regs[rd];
    
    // Offset is a 10 bit constant
    offset <<= 2;

    if (sp)
        dst = regs.sp + offset;
    else
        // Bit 1 is forced to 0
        dst = (regs.pc & ~0x2) + offset;
}

// THUMB 13
void ARM::addOffsetSp(u16 instr)
{
    // Sign flag
    bool sign = instr >> 7 & 0x1;
    // 7-bit immediate value
    int offset = instr & 0x3F;

    // Offset is a 10-bit value
    offset <<= 2;

    if (sign)
        offset *= -1;

    regs.sp += offset; 
}

// THUMB 14
void ARM::pushPopRegisters(u16 instr)
{
    // Pop / push flag
    bool pop = instr >> 11 & 0x1;
    // Load PC / store LR flag
    bool pc_lr = instr >> 8 & 0x1;
    // Register list
    int rlist = instr & 0xFF;

    // Assume full descending stack
    if (pop)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (rlist & 1 << x)
            {
                regs[x] = mmu->readWord(regs.sp);
                regs.sp += 4;
            }
        }

        if (pc_lr)
        {
            regs.pc = alignHalf(mmu->readWord(regs.sp));
            regs.sp += 4;

            needs_flush = true;
        }
    }
    else
    {
        if (pc_lr)
        {
            regs.sp -= 4;
            mmu->writeWord(regs.sp, regs.lr);
        }

        for (int x = 7; x >= 0; --x)
        {
            if (rlist & 1 << x)
            {
                regs.sp -= 4;
                mmu->writeWord(regs.sp, regs[x]);
            }
        }
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
