#include "arm.h"

#include "utility.h"

// THUMB 1
void ARM::moveShiftedRegister(u16 instr)
{
    int opcode = (instr >> 11) & 0x03;
    int offset = (instr >>  6) & 0x1F;
    int rs     = (instr >>  3) & 0x07;
    int rd     = (instr >>  0) & 0x07;

    u32& dst = regs[rd];
    u32  src = regs[rs];

    bool carry;
    switch (opcode)
    {
    case 0b00: dst = lsl(src, offset, carry); break;
    case 0b01: dst = lsr(src, offset, carry); break;
    case 0b10: dst = asr(src, offset, carry); break;
    }
    logical(dst, carry);

    cycle(regs.pc + 2, SEQ);
}

// THUMB 2
void ARM::addSubtractImmediate(u16 instr)
{
    int use_imm  = (instr >> 10) & 0x1;
    int subtract = (instr >>  9) & 0x1;
    int rn       = (instr >>  6) & 0x7;
    int rs       = (instr >>  3) & 0x7;
    int rd       = (instr >>  0) & 0x7;

    u32& dst = regs[rd];
    u32  src = regs[rs];
    u32  op1 = use_imm ? rn : regs[rn];

    if (subtract)
        dst = src - op1;
    else
        dst = src + op1;

    arithmetic(src, op1, !subtract);

    cycle(regs.pc + 2, SEQ);
}

// THUMB 3
void ARM::addSubtractMoveCompareImmediate(u16 instr)
{
    int opcode = (instr >> 11) & 0x03;
    int rd     = (instr >>  8) & 0x07;
    int offset = (instr >>  0) & 0xFF;

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

    cycle(regs.pc + 2, SEQ);
}

// THUMB 4
void ARM::aluOperations(u16 instr)
{
    int opcode = (instr >> 6) & 0xF;
    int rs     = (instr >> 3) & 0x7;
    int rd     = (instr >> 0) & 0x7;

    u32& dst = regs[rd];
    u32  src = regs[rs];

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
        cycle();
        break;

    // LSR
    case 0b0011: 
        dst = lsr(dst, src, carry, false); 
        logical(dst, carry);
        cycle();
        break;

    // ASR
    case 0b0100:
        dst = asr(dst, src, carry, false); 
        logical(dst, carry);
        cycle();
        break;

    // ADC
    case 0b0101: 
        src += regs.c;
        arithmetic(dst, src, true);
        dst += src;
        break;

    // SBC
    case 0b0110: 
        src += 1 - regs.c;
        arithmetic(dst, src, false);
        dst -= src;
        break;

    // ROR
    case 0b0111:
        dst = ror(dst, src, carry, false);
        logical(dst, carry);
        cycle();
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
        cycleBooth(dst, true);
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

    cycle(regs.pc + 2, SEQ);
}

// THUMB 5
void ARM::highRegisterBranchExchange(u16 instr)
{
    int opcode = (instr >> 8) & 0x3;
    int hd     = (instr >> 7) & 0x1;
    int hs     = (instr >> 6) & 0x1;
    int rs     = (instr >> 3) & 0x7;
    int rd     = (instr >> 0) & 0x7;

    rs |= hs << 3;
    rd |= hd << 3;

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (opcode)
    {
    // ADD
    case 0b00: 
        if (rd == 15)
        {
            cycle(regs.pc, NSEQ);

            dst += src;
            dst = alignHalf(dst);
            advance();

            cycle(regs.pc, SEQ);
        }
        else
        {
            dst += src;
        }
        break;

    // CMP
    case 0b01:
        arithmetic(dst, src, false);
        break;

    // MOV
    case 0b10: 
        if (rd == 15)
        {
            cycle(regs.pc, NSEQ);

            dst = alignHalf(src);
            advance();

            cycle(regs.pc, SEQ);
        }
        else
        {
            dst = src;
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
            // Change instruction set
            regs.thumb = false;
        }

        cycle(regs.pc, NSEQ);

        regs.pc = src;
        advance();

        cycle(regs.pc, SEQ);
        break;
    }
    cycle(regs.pc + (regs.thumb ? 2 : 4), SEQ);
}

// THUMB 6
void ARM::loadPCRelative(u16 instr)
{
    int rd     = (instr >> 8) & 0x07;
    int offset = (instr >> 0) & 0xFF;

    offset <<= 2;

    cycle(regs.pc, NSEQ);
    cycle();

    regs[rd] = mmu.readWord(alignWord(regs.pc) + offset);

    cycle(regs.pc + 2, SEQ);
}

// THUMB 7
void ARM::loadStoreRegisterOffset(u16 instr)
{
    int load = (instr >> 11) & 0x1;
    int byte = (instr >> 10) & 0x1;
    int ro   = (instr >>  6) & 0x7;
    int rb   = (instr >>  3) & 0x7;
    int rd   = (instr >>  0) & 0x7;

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[ro];

    cycle(regs.pc, NSEQ);

    if (load)
    {
        cycle();

        if (byte)
            dst = mmu.readByte(addr);
        else
            dst = ldr(addr);

        cycle(regs.pc + 2, SEQ);
    }
    else
    {
        if (byte)
            mmu.writeByte(addr, static_cast<u8>(dst));
        else
            mmu.writeWord(alignWord(addr), dst);

        cycle(addr, NSEQ);
    }
}

// THUMB 8
void ARM::loadStoreHalfwordSigned(u16 instr)
{
    int opcode = (instr >> 10) & 0x3;
    int ro     = (instr >>  6) & 0x7;
    int rb     = (instr >>  3) & 0x7;
    int rd     = (instr >>  0) & 0x7;
    
    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[ro];

    cycle(regs.pc, NSEQ);

    if (opcode == 0b00)
    {
        mmu.writeHalf(alignHalf(addr), static_cast<u16>(dst));

        cycle(addr, NSEQ);
    }
    else
    {
        cycle();

        switch (opcode)
        {
        // LDRSB
        case 0b01:
            dst = mmu.readByte(addr);
            if (dst & (1 << 7))
                dst |= 0xFFFFFF00;
            break;

        // LDRH
        case 0b10:
            dst = ldrh(addr);
            break;

        // LDRSH
        case 0b11:
            dst = ldrsh(addr);
            break;
        }

        cycle(regs.pc + 2, SEQ);
    }
}

// THUMB 9
void ARM::loadStoreImmediateOffset(u16 instr)
{
    int byte   = (instr >> 12) & 0x01;
    int load   = (instr >> 11) & 0x01;
    int offset = (instr >>  6) & 0x1F;
    int rb     = (instr >>  3) & 0x07;
    int rd     = (instr >>  0) & 0x07;

    u32& dst = regs[rd];

    if (!byte)
        offset <<= 2;

    u32 addr = regs[rb] + offset;

    cycle(regs.pc, NSEQ);

    if (load)
    {
        cycle();

        if (byte)
            dst = mmu.readByte(addr);
        else
            dst = ldr(addr);

        cycle(regs.pc + 2, SEQ);
    }
    else
    {
        if (byte)
            mmu.writeByte(addr, static_cast<u8>(dst));
        else
            mmu.writeWord(alignWord(addr), dst);

        cycle(addr, NSEQ);
    }
}

// THUMB 10
void ARM::loadStoreHalfword(u16 instr)
{
    int load   = (instr >> 11) & 0x01;
    int offset = (instr >>  6) & 0x1F;
    int rb     = (instr >>  3) & 0x07;
    int rd     = (instr >>  0) & 0x07;

    u32& dst = regs[rd];

    offset <<= 1;

    u32 addr = regs[rb] + offset;

    cycle(regs.pc, NSEQ);

    if (load)
    {
        cycle();
        dst = ldrh(addr);
        cycle(regs.pc + 2, SEQ);
    }
    else
    {
        mmu.writeHalf(alignHalf(addr), static_cast<u16>(dst));
        cycle(addr, NSEQ);
    }
}

// THUMB 11
void ARM::loadStoreSPRelative(u16 instr)
{
    int load   = (instr >> 11) & 0x01;
    int rd     = (instr >>  8) & 0x07;
    int offset = (instr >>  0) & 0xFF;

    u32& dst = regs[rd];
    
    offset <<= 2;

    u32 addr = regs.sp + offset;

    cycle(regs.pc, NSEQ);

    if (load)
    {
        cycle();
        dst = ldr(addr);
        cycle(regs.pc + 2, SEQ);
    }
    else
    {
        mmu.writeWord(alignWord(addr), dst);
        cycle(addr, NSEQ);
    }
}

// THUMB 12
void ARM::loadAddress(u16 instr)
{
    int use_sp = (instr >> 11) & 0x01;
    int rd     = (instr >>  8) & 0x07;
    int offset = (instr >>  0) & 0xFF;

    u32& dst = regs[rd];
    
    offset <<= 2;

    if (use_sp)
        dst = regs.sp + offset;
    else
        dst = alignWord(regs.pc) + offset;

    cycle(regs.pc + 2, SEQ);
}

// THUMB 13
void ARM::addOffsetSP(u16 instr)
{
    int sign   = (instr >> 7) & 0x01;
    int offset = (instr >> 0) & 0x3F;

    offset <<= 2;

    if (sign)
        regs.sp -= offset;
    else
        regs.sp += offset; 

    cycle(regs.pc + 2, SEQ);
}

// THUMB 14
void ARM::pushPopRegisters(u16 instr)
{
    int pop   = (instr >> 11) & 0x01;
    int pc_lr = (instr >>  8) & 0x01;
    int rlist = (instr >>  0) & 0xFF;

    // Register count needed for cycles
    int rcount = count_bits(rlist);

    cycle(regs.pc, NSEQ);

    // Full descending stack
    if (pop)
    {
        for (int x = 0; rcount > 0; ++x)
        {
            if (rlist & (1 << x))
            {
                if (--rcount > 0)
                    cycle(regs.sp, SEQ);
                else
                    cycle();

                regs[x] = mmu.readWord(alignWord(regs.sp));
                regs.sp += 4;
            }
        }

        if (pc_lr)
        {
            cycle(regs.pc + 2, NSEQ);

            regs.pc = mmu.readWord(alignWord(regs.sp));
            regs.pc = alignHalf(regs.pc);
            advance();
            regs.sp += 4;

            cycle(regs.pc, SEQ);
        }
        cycle(regs.pc + 2, SEQ);
    }
    else
    {
        if (pc_lr)
        {
            regs.sp -= 4;
            mmu.writeWord(alignWord(regs.sp), regs.lr);
        }

        for (int x = 7; rcount > 0; --x)
        {
            if (rlist & (1 << x))
            {
                if (--rcount > 0)
                    cycle(regs.sp, SEQ);

                regs.sp -= 4;
                mmu.writeWord(alignWord(regs.sp), regs[x]);
            }
        }
        cycle(regs.sp, NSEQ);
    }
}

// THUMB 15
void ARM::loadStoreMultiple(u16 instr)
{
    int load  = (instr >> 11) & 0x01;
    int rb    = (instr >>  8) & 0x07;
    int rlist = (instr >>  0) & 0xFF;

    u32& addr = regs[rb];
    addr = alignWord(addr);

    if (rlist != 0)
    {
        cycle(regs.pc, NSEQ);
    
        // Register count needed for cycles
        int rcount = count_bits(rlist);

        if (load)
        {
            for (int x = 0; rcount > 0; ++x)
            {
                if (rlist & (1 << x))
                {
                    if (--rcount > 0)
                        cycle(addr, SEQ);
                    else
                        cycle();

                    regs[x] = mmu.readWord(addr);

                    addr += 4;
                }
            }
            cycle(regs.pc + 2, SEQ);
        }
        else
        {
            for (int x = 0; rcount > 0; ++x)
            {
                if (rlist & (1 << x))
                {
                    if (--rcount > 0)
                        cycle(addr, SEQ);

                    mmu.writeWord(addr, regs[x]);

                    addr += 4;
                }
            }
            cycle(addr, NSEQ);
        }
    }
    else  // Special case empty rlist
    {
        if (load)
        {
            regs.pc = mmu.readWord(alignWord(addr));
            regs.pc = alignHalf(regs.pc);
            advance();
        }
        else
        {
            // Save address of next instruction
            mmu.writeWord(alignWord(addr), regs.pc + 2);
        }
        addr += 0x40;
    }
}

// THUMB 16
void ARM::conditionalBranch(u16 instr)
{
    int condition = (instr >> 8) & 0x0F;
    int offset    = (instr >> 0) & 0xFF;

    if (regs.check(static_cast<Condition>(condition)))
    {
        offset = twos<8>(offset);
        offset <<= 1;

        cycle(regs.pc, NSEQ);

        regs.pc += offset;
        advance();

        cycle(regs.pc, SEQ);
    }
    cycle(regs.pc + 2, SEQ);
}

// THUMB 17
void ARM::softwareInterruptThumb(u16 instr)
{
    cycle(regs.pc, NSEQ);

    u32 cpsr = regs.cpsr;
    u32 next = regs.pc - 2;

    regs.switchMode(MODE_SVC);
    regs.spsr = cpsr;
    regs.lr = next;

    regs.thumb = false;
    regs.irqd = true;

    regs.pc = EXV_SWI;
    advance();

    cycle(regs.pc, SEQ);
    cycle(regs.pc + 4, SEQ);
}

// THUMB 18
void ARM::unconditionalBranch(u16 instr)
{
    int offset = (instr >> 0 ) & 0x7FF;
    
    offset = twos<11>(offset);
    offset <<= 1;

    cycle(regs.pc, NSEQ);

    regs.pc += offset;
    advance();

    cycle(regs.pc, SEQ);
    cycle(regs.pc + 2, SEQ);
}

// THUMB 19
void ARM::longBranchLink(u16 instr)
{
    int second = (instr >> 11) & 0x001;
    int offset = (instr >>  0) & 0x7FF;

    if (second)
    {
        offset <<= 1;

        u32 next = (regs.pc - 2) | 1;

        cycle(regs.pc, NSEQ);

        regs.pc = regs.lr + offset;
        regs.lr = next;
        advance();

        cycle(regs.pc, SEQ);
        cycle(regs.pc + 2, SEQ);
    }
    else
    {
        offset = twos<11>(offset);
        offset <<= 12;

        cycle(regs.pc, SEQ);

        regs.lr = regs.pc + offset;
    }
}
