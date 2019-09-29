#include "arm.h"

#include "common/utility.h"

void ARM::moveShiftedRegister(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rs     = bits< 3, 3>(instr);
    int offset = bits< 6, 5>(instr);
    int opcode = bits<11, 2>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    bool carry;
    switch (opcode)
    {
    case 0b00: dst = lsl(src, offset, carry); break;
    case 0b01: dst = lsr(src, offset, carry); break;
    case 0b10: dst = asr(src, offset, carry); break;
    default: carry = regs.c; break;
    }
    logical(dst, carry);

    cycle(regs.pc + 2, SEQ);
}

void ARM::addSubtractImmediate(u16 instr)
{
    int rd       = bits< 0, 3>(instr);
    int rs       = bits< 3, 3>(instr);
    int rn       = bits< 6, 3>(instr);
    int subtract = bits< 9, 1>(instr);
    int use_imm  = bits<10, 1>(instr);

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

void ARM::addSubtractMoveCompareImmediate(u16 instr)
{
    int offset = bits< 0, 8>(instr);
    int rd     = bits< 8, 3>(instr);
    int opcode = bits<11, 2>(instr);

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

void ARM::aluOperations(u16 instr)
{
    int rd     = bits<0, 3>(instr);
    int rs     = bits<3, 3>(instr);
    int opcode = bits<6, 4>(instr);

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

void ARM::highRegisterBranchExchange(u16 instr)
{
    int rd     = bits<0, 3>(instr);
    int rs     = bits<3, 3>(instr);
    int hs     = bits<6, 1>(instr);
    int hd     = bits<7, 1>(instr);
    int opcode = bits<8, 2>(instr);

    rs |= hs << 3;
    rd |= hd << 3;

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (opcode)
    {
    // ADD
    case 0b00: 
        if (rd != 15)
        {
            dst += src;
        }
        else
        {
            cycle(regs.pc, NSEQ);

            dst += src;
            dst = alignHalf(dst);
            advance();

            cycle(regs.pc, SEQ);
        }
        break;

    // CMP
    case 0b01:
        arithmetic(dst, src, false);
        break;

    // MOV
    case 0b10: 
        if (rd != 15)
        {
            dst = src;
        }
        else
        {
            cycle(regs.pc, NSEQ);

            dst = alignHalf(src);
            advance();

            cycle(regs.pc, SEQ);
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

void ARM::loadPCRelative(u16 instr)
{
    int offset = bits<0, 8>(instr);
    int rd     = bits<8, 3>(instr);

    offset <<= 2;

    cycle(regs.pc, NSEQ);
    cycle();

    u32& dst = regs[rd];
    u32 addr = alignWord(regs.pc) + offset;

    dst = mmu.readWord(addr);

    cycle(regs.pc + 2, SEQ);
}

void ARM::loadStoreRegisterOffset(u16 instr)
{
    int rd   = bits< 0, 3>(instr);
    int rb   = bits< 3, 3>(instr);
    int ro   = bits< 6, 3>(instr);
    int byte = bits<10, 1>(instr);
    int load = bits<11, 1>(instr);

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
            mmu.writeByte(addr, dst);
        else
            mmu.writeWord(addr, dst);

        cycle(addr, NSEQ);
    }
}

void ARM::loadStoreHalfwordSigned(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int ro     = bits< 6, 3>(instr);
    int opcode = bits<10, 2>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[ro];

    cycle(regs.pc, NSEQ);

    if (opcode != 0b00)
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
    else  // STRH
    {
        mmu.writeHalf(addr, dst);

        cycle(addr, NSEQ);
    }
}

void ARM::loadStoreImmediateOffset(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int offset = bits< 6, 5>(instr);
    int load   = bits<11, 1>(instr);
    int byte   = bits<12, 1>(instr);

    if (!byte)
        offset <<= 2;

    u32& dst = regs[rd];
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
            mmu.writeByte(addr, dst);
        else
            mmu.writeWord(addr, dst);

        cycle(addr, NSEQ);
    }
}

void ARM::loadStoreHalfword(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int offset = bits< 6, 5>(instr);
    int load   = bits<11, 1>(instr);

    offset <<= 1;

    u32& dst = regs[rd];
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
        mmu.writeHalf(addr, dst);
        cycle(addr, NSEQ);
    }
}

void ARM::loadStoreSPRelative(u16 instr)
{
    int offset = bits< 0, 8>(instr);
    int rd     = bits< 8, 3>(instr);
    int load   = bits<11, 1>(instr);

    offset <<= 2;

    u32& dst = regs[rd];
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
        mmu.writeWord(addr, dst);
        cycle(addr, NSEQ);
    }
}

void ARM::loadAddress(u16 instr)
{
    int offset = bits< 0, 8>(instr);
    int rd     = bits< 8, 3>(instr);
    int use_sp = bits<11, 1>(instr);

    offset <<= 2;

    u32& dst = regs[rd];

    if (use_sp)
        dst = regs.sp + offset;
    else
        dst = alignWord(regs.pc) + offset;

    cycle(regs.pc + 2, SEQ);
}

void ARM::addOffsetSP(u16 instr)
{
    int offset = bits<0, 7>(instr);
    int sign   = bits<7, 1>(instr);

    offset <<= 2;

    if (sign)
        regs.sp -= offset;
    else
        regs.sp += offset; 

    cycle(regs.pc + 2, SEQ);
}

void ARM::pushPopRegisters(u16 instr)
{
    int rlist = bits< 0, 8>(instr);
    int pc_lr = bits< 8, 1>(instr);
    int pop   = bits<11, 1>(instr);

    // Register count needed for cycles
    int rcount = countBits(rlist);

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

                regs[x] = mmu.readWord(regs.sp);
                regs.sp += 4;
            }
        }

        if (pc_lr)
        {
            cycle(regs.pc + 2, NSEQ);

            regs.pc = mmu.readWord(regs.sp);
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
            mmu.writeWord(regs.sp, regs.lr);
        }

        for (int x = 7; rcount > 0; --x)
        {
            if (rlist & (1 << x))
            {
                if (--rcount > 0)
                    cycle(regs.sp, SEQ);

                regs.sp -= 4;
                mmu.writeWord(regs.sp, regs[x]);
            }
        }
        cycle(regs.sp, NSEQ);
    }
}

void ARM::loadStoreMultiple(u16 instr)
{
    int rlist = bits< 0, 8>(instr);
    int rb    = bits< 8, 3>(instr);
    int load  = bits<11, 1>(instr);

    u32 addr = alignWord(regs[rb]);

    bool writeback = true;

    if (rlist != 0)
    {
        cycle(regs.pc, NSEQ);
    
        // Register count needed for cycles
        int rcount = countBits(rlist);

        if (load)
        {
            // Prevent overwriting loaded value
            if (rlist & (1 << rb))
                writeback = false;

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

    if (writeback)
        regs[rb] = addr;
}

void ARM::conditionalBranch(u16 instr)
{
    int offset    = bits<0, 8>(instr);
    int condition = bits<8, 4>(instr);

    if (regs.check(static_cast<Condition>(condition)))
    {
        offset = signExtend<8>(offset);
        offset <<= 1;

        cycle(regs.pc, NSEQ);

        regs.pc += offset;
        advance();

        cycle(regs.pc, SEQ);
    }
    cycle(regs.pc + 2, SEQ);
}

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

void ARM::unconditionalBranch(u16 instr)
{
    int offset = bits<0, 11>(instr);
    
    offset = signExtend<11>(offset);
    offset <<= 1;

    cycle(regs.pc, NSEQ);

    regs.pc += offset;
    advance();

    cycle(regs.pc, SEQ);
    cycle(regs.pc + 2, SEQ);
}

void ARM::longBranchLink(u16 instr)
{
    int offset = bits< 0, 11>(instr);
    int second = bits<11,  1>(instr);

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
        offset = signExtend<11>(offset);
        offset <<= 12;

        cycle(regs.pc, SEQ);

        regs.lr = regs.pc + offset;
    }
}
