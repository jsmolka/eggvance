#include "arm.h"

#include "utility.h"

u32 ARM::shiftedRegister(int data, bool& carry)
{
    int type    = (data >> 5) & 0x3;
    int use_reg = (data >> 4) & 0x1;
    int rm      = (data >> 0) & 0xF;

    int offset;
    if (use_reg)
    {
        int rs = (data >> 8) & 0xF;
        offset = regs[rs];
        offset &= 0xFF;
    }
    else
    {
        offset = (data >> 7) & 0x1F;
    }

    u32 value = regs[rm];
    switch (type)
    {
    case 0b00: return lsl(value, offset, carry); break;
    case 0b01: return lsr(value, offset, carry, !use_reg); break;
    case 0b10: return asr(value, offset, carry, !use_reg); break;
    case 0b11: return ror(value, offset, carry, !use_reg); break;

    default:
        return value;
    }
}

u32 ARM::rotatedImmediate(int data, bool& carry)
{
    int rotation = (data >> 8) & 0x0F;
    u32 value    = (data >> 0) & 0xFF;

    // Apply twice the rotation
    rotation <<= 1;

    return ror(value, rotation, carry, false);
}

// ARM 1
void ARM::branchExchange(u32 instr)
{
    int rn = (instr >> 0) & 0xF;

    u32 addr = regs[rn];
    if (addr & 0x1)
    {
        addr = alignHalf(addr);
        // Change instruction set
        regs.thumb = true;
    }
    else
    {
        addr = alignWord(addr);
    }

    cycle(regs.pc, NSEQ);

    regs.pc = addr;
    needs_flush = true;

    cycle(regs.pc, SEQ);
    cycle(regs.pc + (regs.thumb ? 2 : 4), SEQ);
}

// ARM 2
void ARM::branchLink(u32 instr)
{
    int link   = (instr >> 24) & 0x000001;
    int offset = (instr >>  0) & 0xFFFFFF;

    offset = twos<24>(offset);
    offset <<= 2;

    if (link)
        regs.lr = regs.pc - 4;

    cycle(regs.pc, NSEQ);

    regs.pc += offset;
    needs_flush = true;

    cycle(regs.pc, SEQ);
    cycle(regs.pc + 4, SEQ);
}

// ARM 3
void ARM::dataProcessing(u32 instr)
{
    int use_imm = (instr >> 25) & 0x001;
    int opcode  = (instr >> 21) & 0x00F;
    int flags   = (instr >> 20) & 0x001;
    int rn      = (instr >> 16) & 0x00F;
    int rd      = (instr >> 12) & 0x00F;
    int data    = (instr >>  0) & 0xFFF;

    u32& dst = regs[rd];
    u32  op1 = regs[rn];
    u32  op2;

    bool carry;
    if (use_imm)
    {
        op2 = rotatedImmediate(data, carry);
    }
    else
    {
        int type    = (data >> 5) & 0x3;
        int use_reg = (data >> 4) & 0x1;
        int rm      = (data >> 0) & 0xF;

        u32 value = regs[rm];

        int offset;
        if (use_reg)
        {
            int rs = (data >> 8) & 0xF;
            offset = regs[rs];
            offset &= 0xFF;

            // Account for prefetch
            if (rn == 15) op1 += 4;
            if (rm == 15) value += 4;
        }
        else
        {
            offset = (data >> 7) & 0x1F;
        }

        switch (type)
        {
        case 0b00: op2 = lsl(value, offset, carry); break;
        case 0b01: op2 = lsr(value, offset, carry, !use_reg); break;
        case 0b10: op2 = asr(value, offset, carry, !use_reg); break;
        case 0b11: op2 = ror(value, offset, carry, !use_reg); break;
        }

        cycle();
    }

    if (rd == 15)
    {
        cycle(regs.pc, NSEQ);

        if (flags)
        {
            // Move SPSR into CPSR
            u32 spsr = regs.spsr;
            regs.switchMode(static_cast<Mode>(spsr & CPSR_M));
            regs.cpsr = spsr;

            flags = false;
        }
    }

    switch (opcode)
    {
    // AND
    case 0b0000:
        dst = op1 & op2;
        if (flags) 
            logical(dst, carry);
        break;

    // EOR
    case 0b0001:
        dst = op1 ^ op2;
        if (flags) 
            logical(dst, carry);
        break;

    // SUB
    case 0b0010:
        dst = op1 - op2;
        if (flags) 
            arithmetic(op1, op2, false);
        break;

    // RSB
    case 0b0011:
        dst = op2 - op1;
        if (flags) 
            arithmetic(op2, op1, false);
        break;

    // ADD
    case 0b0100:
        dst = op1 + op2;
        if (flags) 
            arithmetic(op1, op2, true);
        break;

    // ADC
    case 0b0101:
        op2 += regs.c;
        dst = op1 + op2;
        if (flags) 
            arithmetic(op1, op2, true);
        break;

    // SBC
    case 0b0110:
        op2 += 1 - regs.c;
        dst = op1 - op2;
        if (flags) 
            arithmetic(op1, op2, false);
        break;

    // RBC
    case 0b0111:
        op1 += 1 - regs.c;
        dst = op2 - op1;
        if (flags) 
            arithmetic(op2, op1, false);
        break;

    // TST
    case 0b1000:
        logical(op1 & op2, carry);
        break;

    // TEQ
    case 0b1001:
        logical(op1 ^ op2, carry);
        break;

    // CMP
    case 0b1010:
        arithmetic(op1, op2, false);
        break;

    // CMN
    case 0b1011:
        arithmetic(op1, op2, true);
        break;

    // ORR
    case 0b1100:
        dst = op1 | op2;
        if (flags) 
            logical(dst, carry);
        break;

    // MOV
    case 0b1101:
        dst = op2;
        if (flags) 
            logical(dst, carry);
        break;

    // BIC
    case 0b1110:
        dst = op1 & ~op2;
        if (flags) 
            logical(dst, carry);
        break;

    // MVN
    case 0b1111:
        dst = ~op2;
        if (flags) 
            logical(dst, carry);
        break;
    }

    if (rd == 15)
    {
        // Interrupt return from ARM into THUMB possible
        dst = regs.thumb ? alignHalf(dst) : alignWord(dst);
        needs_flush = true;

        cycle(regs.pc, SEQ);
    }
    cycle(regs.pc + 4, SEQ);
}

// ARM 4
void ARM::psrTransfer(u32 instr)
{
    int write    = (instr >> 21) & 0x1;
    int use_spsr = (instr >> 22) & 0x1;

    if (write)
    {
        int use_imm = (instr >> 25) & 0x001;
        int data    = (instr >>  0) & 0xFFF;

        u32 op;
        if (use_imm)
        {
            bool carry;
            op = rotatedImmediate(data, carry);
        }
        else
        {
            int rm = (data >> 0) & 0xF;
            op = regs[rm];
        }

        // Mask based on fsxc-bits
        u32 mask = 0;
        if (instr & (1 << 19))
            mask |= 0xFF000000;
        if (instr & (1 << 18))
            mask |= 0x00FF0000;
        if (instr & (1 << 17))
            mask |= 0x0000FF00;

        if (instr & (1 << 16))
        {
            // Control bits are protected in USR mode
            if (regs.mode() != MODE_USR)
                mask |= 0x000000FF;
        }

        op &= mask;

        if (use_spsr)
        {
            regs.spsr = (regs.spsr & ~mask) | op;
        }
        else
        {
            if (mask & 0xFF)
                regs.switchMode(static_cast<Mode>(op & CPSR_M));

            regs.cpsr = (regs.cpsr & ~mask) | op;

            // Undefined behavior
            if (regs.thumb)
            {
                regs.pc = alignHalf(regs.pc);
                needs_flush = true;
            }
        }
    }
    else
    {
        int rd = (instr >> 12) & 0xF;
        regs[rd] = use_spsr ? regs.spsr : regs.cpsr;
    }
    cycle(regs.pc + 4, SEQ);
}

// ARM 5
void ARM::multiply(u32 instr)
{
    int accumulate = (instr >> 21) & 0x1;
    int flags      = (instr >> 20) & 0x1;
    int rd           = (instr >> 16) & 0xF;
    int rn         = (instr >> 12) & 0xF;
    int rs         = (instr >>  8) & 0xF;
    int rm           = (instr >>  0) & 0xF;

    u32& dst = regs[rd];
    u32  op1 = regs[rs];
    u32  op2 = regs[rm];
    u32  acc = regs[rn];

    dst = op1 * op2;
    if (accumulate) 
        dst += acc;

    if (flags)
        logical(dst);

    cycleMultiplication(op1, true);

    if (accumulate)
        cycle();

    cycle(regs.pc + 4, SEQ);
}

// ARM 6
void ARM::multiplyLong(u32 instr)
{
    int sign       = (instr >> 22) & 0x1;
    int accumulate = (instr >> 21) & 0x1;
    int flags      = (instr >> 20) & 0x1;
    int rdhi       = (instr >> 16) & 0xF;
    int rdlo       = (instr >> 12) & 0xF;
    int rs         = (instr >>  8) & 0xF;
    int rm         = (instr >>  0) & 0xF;

    u32& dsthi = regs[rdhi];
    u32& dstlo = regs[rdlo];

    u64 op1 = regs[rs];
    u64 op2 = regs[rm];

    if (sign)
    {
        if (op1 & (1 << 31))
            op1 |= 0xFFFFFFFF00000000;
        if (op2 & (1 << 31))
            op2 |= 0xFFFFFFFF00000000;
    }
    
    u64 result = op1 * op2;
    if (accumulate)
        result += (static_cast<u64>(dsthi) << 32) | dstlo;

    dsthi = static_cast<u32>(result >> 32);
    dstlo = static_cast<u32>(result);

    if (flags)
    {
        regs.z = result == 0;
        regs.n = result >> 63;
    }

    cycleMultiplication(static_cast<u32>(op1), sign);

    cycle();
    if (accumulate)
        cycle();

    cycle(regs.pc + 4, SEQ);
}

// ARM 7
void ARM::singleTransfer(u32 instr)
{
    int use_reg   = (instr >> 25) & 0x001;
    int pre_index = (instr >> 24) & 0x001;
    int increment = (instr >> 23) & 0x001;
    int byte      = (instr >> 22) & 0x001;
    int writeback = (instr >> 21) & 0x001;
    int load      = (instr >> 20) & 0x001;
    int rn        = (instr >> 16) & 0x00F;
    int rd        = (instr >> 12) & 0x00F;
    int data      = (instr >>  0) & 0xFFF;
    
    u32& dst = regs[rd];
    u32 addr = regs[rn];

    // Post-indexing always writes back
    writeback |= !pre_index;

    u32 offset;
    if (use_reg)
    {
        bool carry;
        offset = shiftedRegister(data, carry);
    }
    else
    {
        offset = data;
    }

    if (pre_index)
    {
        if (increment)
            addr += offset;
        else
            addr -= offset;
    }

    cycle(regs.pc, NSEQ);

    if (load)
    {
        cycle();
        if (rd == 15)
            cycle(regs.pc + 4, NSEQ);

        if (byte)
            dst = mmu.readByte(addr);
        else
            dst = ldr(addr);

        // Prevent overwriting the loaded value
        if (rd == rn)
            writeback = false;

        if (rd == 15)
        {
            dst = alignWord(dst);
            needs_flush = true;

            cycle(regs.pc, SEQ);
        }
        cycle(regs.pc + 4, SEQ);
    }
    else
    {
        u32 value = dst;
        // Account for prefetch
        if (rd == 15) value += 4;

        if (byte)
            mmu.writeByte(addr, static_cast<u8>(value));
        else
            mmu.writeWord(alignWord(addr), value);

        cycle(addr, NSEQ);
    }

    if (!pre_index)
    {
        if (increment)
            addr += offset;
        else
            addr -= offset;
    }
    
    if (writeback)
        regs[rn] = addr;
}

// ARM 8
void ARM::halfSignedTransfer(u32 instr)
{
    int pre_index = (instr >> 24) & 0x1;
    int use_imm   = (instr >> 22) & 0x1;
    int increment = (instr >> 23) & 0x1;
    int writeback = (instr >> 21) & 0x1;
    int load      = (instr >> 20) & 0x1;
    int rn        = (instr >> 16) & 0xF;
    int rd        = (instr >> 12) & 0xF;
    int sign      = (instr >>  6) & 0x1;
    int half      = (instr >>  5) & 0x1;

    u32 addr = regs[rn];
    u32& dst = regs[rd];

    // Post-indexing always writes back
    writeback |= !pre_index;

    u32 offset;
    if (use_imm)
    {
        int upper = (instr >> 8) & 0xF;
        int lower = (instr >> 0) & 0xF;
        offset = upper << 4 | lower;
    }
    else
    {
        int rm = (instr >> 0) & 0xF;
        offset = regs[rm];
    }

    if (pre_index)
    {
        if (increment)
            addr += offset;
        else
            addr -= offset;
    }

    cycle(regs.pc, NSEQ);

    if (load)
    {
        cycle();
        if (rd == 15)
            cycle(regs.pc + 4, NSEQ);

        switch ((sign << 1) | half)
        {
        // LDRH
        case 0b01:
            dst = ldrh(addr);
            break;

        // LDRSB
        case 0b10:
            dst = mmu.readByte(addr);
            if (dst & (1 << 7))
                dst |= 0xFFFFFF00;
            break;

        // LDRSH
        case 0b11:
            dst = ldrsh(addr);
            break;
        }

        // Prevent overwriting the loaded value
        if (rd == rn)
            writeback = false;

        if (rd == 15)
        {
            dst = alignWord(dst);
            needs_flush = true;

            cycle(regs.pc, SEQ);
        }
        cycle(regs.pc + 4, SEQ);
    }
    else
    {
        u32 value = dst;
        // Account for prefetch
        if (rd == 15) value += 4;

        if (half)
            mmu.writeHalf(alignHalf(addr), static_cast<u16>(value));

        cycle(addr, NSEQ);
    }

    if (!pre_index)
    {
        if (increment)
            addr += offset;
        else
            addr -= offset;
    }

    if (writeback)
        regs[rn] = addr;
}

// ARM 9
void ARM::blockTransfer(u32 instr)
{
    int full      = (instr >> 24) & 0x0001;
    int ascending = (instr >> 23) & 0x0001;
    int user      = (instr >> 22) & 0x0001;
    int writeback = (instr >> 21) & 0x0001;
    int load      = (instr >> 20) & 0x0001;
    int rn        = (instr >> 16) & 0x000F;
    int rlist     = (instr >>  0) & 0xFFFF;

    u32 addr = regs[rn];

    // Force user register transfer
    Mode mode = regs.mode();
    if (user)
        regs.switchMode(MODE_USR);

    if (rlist != 0)
    {
        cycle(regs.pc, NSEQ);

        // Register count needed for cycles
        int rcount = 0;
        for (int temp = rlist; temp != 0; temp >>= 1)
            rcount += temp & 0x1;

        int init = ascending ? 0 : 15;
        int loop = ascending ? 1 : -1;
        int step = ascending ? 4 : -4;

        // Lowest register is stored at the lowest address
        if (load)
        {
            // Prevent overwriting the loaded value
            if (rlist & (1 << rn))
                writeback = false;

            for (int x = init; rcount > 0; x += loop)
            {
                if (rlist & (1 << x))
                {
                    if (full) addr += step;

                    if (--rcount > 0)
                        cycle(addr, SEQ);
                    else
                        cycle();

                    if (x == 15)
                        cycle(regs.pc + 4, NSEQ);

                    regs[x] = mmu.readWord(alignWord(addr));

                    if (x == 15)
                    {
                        regs.pc = alignWord(regs.pc);
                        needs_flush = true;

                        cycle(regs.pc, SEQ);
                    }

                    if (!full) addr += step;
                }
            }
            cycle(regs.pc + 4, SEQ);
        }
        else
        {
            for (int x = init; rcount > 0; x += loop)
            {
                if (rlist & (1 << x))
                {
                    if (full) addr += step;

                    if (--rcount > 0)
                        cycle(addr, SEQ);

                    mmu.writeWord(alignWord(addr), regs[x]);

                    if (!full) addr += step;
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
            regs.pc = alignWord(regs.pc);
            needs_flush = true;
        }
        else
        {
            // Save address of next instruction
            mmu.writeWord(alignWord(addr), regs.pc + 4);
        }
        addr += ascending ? 0x40 : -0x40;
    }

    if (writeback)
        regs[rn] = addr;

    if (user)
        regs.switchMode(mode);
}

// ARM 10
void ARM::singleSwap(u32 instr)
{
    int byte = (instr >> 22) & 0x1;
    int rn   = (instr >> 16) & 0xF;
    int rd   = (instr >> 12) & 0xF;
    int rm   = (instr >>  0) & 0xF;

    u32 addr = regs[rn];
    u32& dst = regs[rd];
    u32  src = regs[rm];

    cycle(regs.pc, NSEQ);
    cycle(addr, NSEQ);

    if (byte)
    {
        dst = mmu.readByte(addr);
        mmu.writeByte(addr, static_cast<u8>(src));
    }
    else
    {
        dst = ldr(addr);
        mmu.writeWord(alignWord(addr), src);
    }

    cycle();
    cycle(regs.pc + 4, SEQ);
}

// ARM 11
void ARM::swiArm(u32 instr)
{
    cycle(regs.pc, NSEQ);

    u32 cpsr = regs.cpsr;
    u32 next = regs.pc - 4;

    regs.switchMode(MODE_SVC);
    regs.cpsr |= CPSR_I;

    regs.spsr = cpsr;
    regs.lr = next;

    regs.pc = EXV_SWI;
    needs_flush = true;

    cycle(regs.pc, SEQ);
    cycle(regs.pc + 4, SEQ);
}
