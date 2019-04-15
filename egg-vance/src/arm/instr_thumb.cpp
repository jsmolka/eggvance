#include "arm.h"

/**
 * Todo
 * - implement SWI
 * - process SWI in conditional branch
 */

#include <iostream>

#include "utility.h"

// THUMB 1
void ARM::moveShiftedRegister(u16 instr)
{
    int opcode = (instr >> 11) & 0x3;
    int offset = (instr >> 6) & 0x1F;
    int rs = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

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
    bool immediate = (instr >> 10) & 0x1;
    bool subtract = (instr >> 9) & 0x1;
    int rn = (instr >> 6) & 0x7;
    int rs = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    u32 op = immediate ? rn : regs[rn];

    if (subtract)
        dst = src - op;
    else
        dst = src + op;

    arithmetic(src, op, !subtract);
}

// THUMB 3
void ARM::addSubCmpMovImmediate(u16 instr)
{
    int opcode = (instr >> 11) & 0x3;
    int rd = (instr >> 8) & 0x7;
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
    int opcode = (instr >> 6) & 0xF;
    int rs = (instr >> 3) & 0x7;
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
        dst = lsl(dst, src, carry); 
        logical(dst, carry);
        break;

    // LSR
    case 0b0011: 
        dst = lsr(dst, src, carry, false); 
        logical(dst, carry);
        break;

    // ASR
    case 0b0100:
        dst = asr(dst, src, carry, false); 
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
        dst = ror(dst, src, carry, false);
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
    int opcode = (instr >> 8) & 0x3;
    int hd = (instr >> 7) & 0x1;
    int hs = (instr >> 6) & 0x1;
    int rs = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    rs |= hs << 3;
    rd |= hd << 3;

    u32 src = regs[rs];
    u32& dst = regs[rd];

    switch (opcode)
    {
    // ADD
    case 0b00: 
        dst += src;
        if (rd == 15)
        {
            dst = alignHalf(dst);
            needs_flush = true;
        }
        break;

    // CMP
    case 0b01:
        arithmetic(dst, src, false);
        break;

    // MOV
    case 0b10: 
        dst = src;
        if (rd == 15)
        {
            dst = alignHalf(dst);
            needs_flush = true;
        }
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
            // Switch instruction set
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
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;

    offset <<= 2;

    regs[rd] = mmu.readWord((regs.pc & ~0x2) + offset);
}

// THUMB 7
void ARM::loadStoreRegisterOffset(u16 instr)
{
    int load = (instr >> 11) & 0x1;
    int byte = (instr >> 10) & 0x1;
    int ro = (instr >> 6) & 0x7;
    int rb = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    u32& dst = regs[rd];

    u32 addr = regs[rb] + regs[ro];

    switch (load << 1 | byte)
    {
    // STR
    case 0b00:
        mmu.writeWord(alignWord(addr), dst); 
        break;

    // STRB
    case 0b01: 
        mmu.writeByte(addr, dst & 0xFF); 
        break;

    // LDR
    case 0b10: 
        dst = ldr(addr);
        break;

    // LDRB
    case 0b11: 
        dst = mmu.readByte(addr); 
        break;
    }
}

// THUMB 8
void ARM::loadStoreHalfSigned(u16 instr)
{
    int half = (instr >> 11) & 0x1;
    int sign = (instr >> 10) & 0x1;
    int ro = (instr >> 6) & 0x7;
    int rb = (instr >> 3) & 0x7;
    int rd = instr & 0x7;
    
    u32& dst = regs[rd];

    u32 addr = regs[rb] + regs[ro];

    switch (sign << 1 | half)
    {
    // STRH
    case 0b00:
        mmu.writeHalf(alignHalf(addr), dst & 0xFFFF); 
        break;

    // LDRH
    case 0b01: 
        dst = ldrh(addr);
        break;

    // LDSB
    case 0b10: 
        dst = mmu.readByte(addr);
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
    int byte = (instr >> 12) & 0x1;
    int load = (instr >> 11) & 0x1;
    int offset = (instr >> 6) & 0x1F;
    int rb = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    u32& dst = regs[rd];

    if (!byte)
        offset <<= 2;

    u32 addr = regs[rb] + offset;

    switch (load << 1 | byte)
    {
    // STR
    case 0b00: 
        mmu.writeWord(alignWord(addr) , dst);
        break;

    // STRB
    case 0b01: 
        mmu.writeByte(addr, dst & 0xFF); 
        break;

    // LDR
    case 0b10: 
        dst = ldr(addr);
        break;

    // LDRB
    case 0b11: 
        dst = mmu.readByte(addr); 
        break;
    }
}

// THUMB 10
void ARM::loadStoreHalf(u16 instr)
{
    bool load = (instr >> 11) & 0x1;
    int offset = (instr >> 6) & 0x1F;
    int rb = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    u32& dst = regs[rd];

    offset <<= 1;

    u32 addr = regs[rb] + offset;

    if (load)
        dst = ldrh(addr);
    else
        mmu.writeHalf(alignHalf(addr), dst & 0xFFFF);
}

// THUMB 11
void ARM::loadStoreSpRelative(u16 instr)
{
    bool load = (instr >> 11) & 0x1;
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;

    u32& dst = regs[rd];
    
    offset <<= 2;

    u32 addr = regs.sp + offset;

    if (load)
        dst = ldr(addr);
    else
        mmu.writeWord(alignWord(addr), dst);
}

// THUMB 12
void ARM::loadAddress(u16 instr)
{
    bool sp = (instr >> 11) & 0x1;
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;

    u32& dst = regs[rd];
    
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
    bool sign = (instr >> 7) & 0x1;
    int offset = instr & 0x3F;

    offset <<= 2;

    if (sign)
        regs.sp -= offset;
    else
        regs.sp += offset; 
}

// THUMB 14
void ARM::pushPopRegisters(u16 instr)
{
    bool pop = (instr >> 11) & 0x1;
    bool pc_lr = (instr >> 8) & 0x1;
    int rlist = instr & 0xFF;

    // Full descending stack
    if (pop)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (rlist & 1 << x)
            {
                regs[x] = mmu.readWord(alignWord(regs.sp));
                regs.sp += 4;
            }
        }

        if (pc_lr)
        {
            regs.pc = mmu.readWord(alignWord(regs.sp));
            regs.pc = alignHalf(regs.pc);
            regs.sp += 4;

            needs_flush = true;
        }
    }
    else
    {
        if (pc_lr)
        {
            regs.sp -= 4;
            mmu.writeWord(alignWord(regs.sp), regs.lr);
        }

        for (int x = 7; x >= 0; --x)
        {
            if (rlist & 1 << x)
            {
                regs.sp -= 4;
                mmu.writeWord(alignWord(regs.sp), regs[x]);
            }
        }
    }
}

// THUMB 15
void ARM::loadStoreMultiple(u16 instr)
{
    bool load = (instr >> 11) & 0x1;
    int rb = (instr >> 8) & 0x7;
    int rlist = instr & 0xFF;

    u32& addr = regs[rb];

    addr = alignWord(addr);

    for (int x = 0; x < 8; ++x)
    {
        if (rlist & 1 << x)
        {
            if (load)
                regs[x] = mmu.readWord(addr);
            else
                mmu.writeWord(addr, regs[x]);

            addr += 4;
        }
    }
}

// THUMB 16
void ARM::conditionalBranch(u16 instr)
{
    int condition = (instr >> 8) & 0xF;
    int offset = instr & 0xFF;

    if (regs.check(static_cast<Condition>(condition)))
    {
        offset = twos<8>(offset);
        offset <<= 1;

        regs.pc += offset;
        needs_flush = true;
    }
}

// THUMB 17
void ARM::softwareInterruptThumb(u16 instr)
{
    std::cout << "Unimplemented THUMB SWI\n";
}

// THUMB 18
void ARM::unconditionalBranch(u16 instr)
{
    int offset = instr & 0x7FF;
    
    offset = twos<11>(offset);
    offset <<= 1;

    regs.pc += offset;
    needs_flush = true;
}

// THUMB 19
void ARM::longBranchLink(u16 instr)
{
    bool high = (instr >> 11) & 0x1;
    int offset = instr & 0x7FF;

    if (!high)
    {
        offset = twos<11>(offset);
        offset <<= 12;

        regs.lr = regs.pc + offset;
    }
    else
    {
        u32 next = regs.pc - 2 | 1;

        regs.pc = regs.lr + (offset << 1);

        regs.lr = next;

        needs_flush = true;
    }
}
