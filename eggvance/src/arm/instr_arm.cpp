#include "arm.h"

#include "common/macros.h"
#include "common/utility.h"

enum class Operation
{
    AND = 0b0000,
    EOR = 0b0001,
    SUB = 0b0010,
    RSB = 0b0011,
    ADD = 0b0100,
    ADC = 0b0101,
    SBC = 0b0110,
    RSC = 0b0111,
    TST = 0b1000,
    TEQ = 0b1001,
    CMP = 0b1010,
    CMN = 0b1011,
    ORR = 0b1100,
    MOV = 0b1101,
    BIC = 0b1110,
    MVN = 0b1111
};

enum class Shift
{
    LSL = 0b00,
    LSR = 0b01,
    ASR = 0b10,
    ROR = 0b11
};

void ARM::branchExchange(u32 instr)
{
    int rn = bits<0, 4>(instr);

    EGG_ASSERT(rn != 15, "Undefined behavior");

    u32 addr = regs[rn];
    cpsr.thumb = addr & 0x1;
    addr = align(addr, length());

    cycle<NSEQ>(pc);

    pc = addr;
    advance();

    cycle<SEQ>(pc);
    cycle<SEQ>(pc + length());
}

void ARM::branchLink(u32 instr)
{
    int offset = bits< 0, 24>(instr);
    int link   = bits<24,  1>(instr);

    offset = signExtend<24>(offset);
    offset <<= 2;

    if (link)
        lr = pc - 4;

    cycle<NSEQ>(pc);

    pc += offset;
    advance();

    cycle<SEQ>(pc);
    cycle<SEQ>(pc + 4);
}

void ARM::dataProcessing(u32 instr)
{
    int rd      = bits<12, 4>(instr);
    int rn      = bits<16, 4>(instr);
    int flags   = bits<20, 1>(instr);
    int opcode  = bits<21, 4>(instr);
    int use_imm = bits<25, 1>(instr);

    u32& dst = regs[rd];
    u32  op1 = regs[rn];
    u32  op2;

    bool carry;
    if (use_imm)
    {
        u32 value    = bits<0, 8>(instr);
        int rotation = bits<8, 4>(instr);

        op2 = ror(value, rotation << 1, carry, false);
    }
    else
    {
        int rm      = bits<0, 4>(instr);
        int use_reg = bits<4, 1>(instr);
        int shift   = bits<5, 2>(instr);

        u32 value = regs[rm];

        int amount;
        if (use_reg)
        {
            int rs = bits<8, 4>(instr);
            amount = regs[rs];
            amount &= 0xFF;

            if (rn == 15) op1 += 4;
            if (rm == 15) value += 4;
        }
        else
        {
            amount = bits<7, 5>(instr);
        }

        switch (static_cast<Shift>(shift))
        {
        case Shift::LSL: op2 = lsl(value, amount, carry); break;
        case Shift::LSR: op2 = lsr(value, amount, carry, !use_reg); break;
        case Shift::ASR: op2 = asr(value, amount, carry, !use_reg); break;
        case Shift::ROR: op2 = ror(value, amount, carry, !use_reg); break;

        default:
            EGG_UNREACHABLE;
            break;
        }
        cycle();
    }

    if (rd == 15)
    {
        cycle<NSEQ>(pc);

        if (flags)
        {
            EGG_ASSERT(cpsr.mode != PSR::Mode::USR, "Requires privileged mode");

            PSR spsr = this->spsr;
            switchMode(spsr.mode);
            cpsr = spsr;

            flags = false;
        }
    }

    switch (static_cast<Operation>(opcode))
    {
    case Operation::AND:
        dst = op1 & op2;
        if (flags) 
            logical(dst, carry);
        break;

    case Operation::EOR:
        dst = op1 ^ op2;
        if (flags) 
            logical(dst, carry);
        break;

    case Operation::SUB:
        dst = op1 - op2;
        if (flags) 
            arithmetic<SUB>(op1, op2);
        break;

    case Operation::RSB:
        dst = op2 - op1;
        if (flags) 
            arithmetic<SUB>(op2, op1);
        break;

    case Operation::ADD:
        dst = op1 + op2;
        if (flags) 
            arithmetic<ADD>(op1, op2);
        break;

    case Operation::ADC:
        op2 += cpsr.c;
        dst = op1 + op2;
        if (flags) 
            arithmetic<ADD>(op1, op2);
        break;

    case Operation::SBC:
        op2 += 1 - cpsr.c;
        dst = op1 - op2;
        if (flags) 
            arithmetic<SUB>(op1, op2);
        break;

    case Operation::RSC:
        op1 += 1 - cpsr.c;
        dst = op2 - op1;
        if (flags) 
            arithmetic<SUB>(op2, op1);
        break;

    case Operation::TST:
        EGG_ASSERT(flags, "Flags should be set");
        logical(op1 & op2, carry);
        break;

    case Operation::TEQ:
        EGG_ASSERT(flags, "Flags should be set");
        logical(op1 ^ op2, carry);
        break;

    case Operation::CMP:
        EGG_ASSERT(flags, "Flags should be set");
        arithmetic<SUB>(op1, op2);
        break;

    case Operation::CMN:
        EGG_ASSERT(flags, "Flags should be set");
        arithmetic<ADD>(op1, op2);
        break;

    case Operation::ORR:
        dst = op1 | op2;
        if (flags) 
            logical(dst, carry);
        break;

    case Operation::MOV:
        dst = op2;
        if (flags) 
            logical(dst, carry);
        break;

    case Operation::BIC:
        dst = op1 & ~op2;
        if (flags) 
            logical(dst, carry);
        break;

    case Operation::MVN:
        dst = ~op2;
        if (flags) 
            logical(dst, carry);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }

    if (rd == 15)
    {
        dst = align(dst, length());
        advance();

        cycle<SEQ>(pc);
    }
    cycle<SEQ>(pc + 4);
}

void ARM::psrTransfer(u32 instr)
{
    int write    = bits<21, 1>(instr);
    int use_spsr = bits<22, 1>(instr);

    if (write)
    {
        int data    = bits< 0, 12>(instr);
        int use_imm = bits<25,  1>(instr);

        u32 op;
        if (use_imm)
        {
            u32 value    = bits<0, 8>(data);
            int rotation = bits<8, 4>(data);
            
            bool carry;
            op = ror(value, rotation << 1, carry, false);
        }
        else
        {
            int rm = bits<0, 4>(data);
            op = regs[rm];
        }

        u32 mask = 0;
        if (instr & (1 << 19)) mask |= 0xFF00'0000;
        if (instr & (1 << 18)) mask |= 0x00FF'0000;
        if (instr & (1 << 17)) mask |= 0x0000'FF00;
        if (instr & (1 << 16))
        {
            EGG_ASSERT(cpsr.mode != PSR::Mode::USR, "Requires privileged mode");
            mask |= 0x0000'00FF;
        }

        op &= mask;

        if (use_spsr)
        {
            spsr = (spsr & ~mask) | op;
        }
        else
        {
            if (mask & 0xFF)
                switchMode(static_cast<PSR::Mode>(op & 0x1F));

            cpsr = (cpsr & ~mask) | op;

            if (cpsr.thumb)
            {
                // Undefined behavior
                pc = alignHalf(pc);
                advance();
            }
        }
    }
    else
    {
        int rd = bits<12, 4>(instr);
        regs[rd] = use_spsr ? spsr : cpsr;
    }
    cycle(pc + 4, SEQ);
}

void ARM::multiply(u32 instr)
{
    int rm         = bits< 0, 4>(instr);
    int rs         = bits< 8, 4>(instr);
    int rn         = bits<12, 4>(instr);
    int rd         = bits<16, 4>(instr);
    int flags      = bits<20, 1>(instr);
    int accumulate = bits<21, 1>(instr);

    u32& dst = regs[rd];
    u32  op1 = regs[rs];
    u32  op2 = regs[rm];
    u32  acc = regs[rn];

    dst = op1 * op2;
    if (accumulate)
    {
        dst += acc;
        cycle();
    }

    if (flags)
        logical(dst);

    cycleBooth(op1, true);
    cycle(pc + 4, SEQ);
}

void ARM::multiplyLong(u32 instr)
{
    int rm         = bits< 0, 4>(instr);
    int rs         = bits< 8, 4>(instr);
    int rdlo       = bits<12, 4>(instr);
    int rdhi       = bits<16, 4>(instr);
    int flags      = bits<20, 1>(instr);
    int accumulate = bits<21, 1>(instr);
    int sign       = bits<22, 1>(instr);

    u32& dsthi = regs[rdhi];
    u32& dstlo = regs[rdlo];

    u64 op1 = regs[rs];
    u64 op2 = regs[rm];

    if (sign)
    {
        op1 = signExtend<32>(op1);
        op2 = signExtend<32>(op2);
    }
    
    u64 result = op1 * op2;
    if (accumulate)
    {
        result += (static_cast<u64>(dsthi) << 32) | dstlo;
        cycle();
    }

    dstlo = static_cast<u32>(result);
    dsthi = static_cast<u32>(result >> 32);

    if (flags)
    {
        cpsr.z = result == 0;
        cpsr.n = result >> 63;
    }

    cycle();
    cycleBooth(static_cast<u32>(op1), sign);
    cycle(pc + 4, SEQ);
}

void ARM::singleDataTransfer(u32 instr)
{
    int data      = bits< 0, 12>(instr);
    int rd        = bits<12,  4>(instr);
    int rn        = bits<16,  4>(instr);
    int load      = bits<20,  1>(instr);
    int writeback = bits<21,  1>(instr);
    int byte      = bits<22,  1>(instr);
    int increment = bits<23,  1>(instr);
    int pre_index = bits<24,  1>(instr);
    int use_reg   = bits<25,  1>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rn];

    // Post-indexing forces writeback
    writeback |= !pre_index;

    u32 offset;
    if (use_reg)
    {
        int rm      = bits<0, 4>(data);
        int use_reg = bits<4, 1>(data);
        int type    = bits<5, 2>(data);

        u32 value = regs[rm];

        int amount;
        if (use_reg)
        {
            int rs = bits<8, 4>(data);
            amount = regs[rs];
            amount &= 0xFF;
        }
        else
        {
            amount = bits<7, 5>(data);
        }

        bool carry;
        switch (type)
        {
        case 0b00: offset = lsl(value, amount, carry); break;
        case 0b01: offset = lsr(value, amount, carry); break;
        case 0b10: offset = asr(value, amount, carry); break;
        case 0b11: offset = ror(value, amount, carry); break;

        default:
            EGG_UNREACHABLE;
            offset = 0;
            break;
        }
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

    cycle(pc, NSEQ);

    if (load)
    {
        cycle();
        if (rd == 15)
            cycle(pc + 4, NSEQ);

        if (byte)
            dst = mmu.readByte(addr);
        else
            dst = ldr(addr);

        // Prevent overwriting loaded value
        if (rd == rn)
            writeback = false;

        if (rd == 15)
        {
            dst = alignWord(dst);
            advance();

            cycle(pc, SEQ);
        }
        cycle(pc + 4, SEQ);
    }
    else
    {
        u32 value = dst;
        // Account for prefetch
        if (rd == 15) value += 4;

        if (byte)
            mmu.writeByte(addr, value);
        else
            mmu.writeWord(addr, value);

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

void ARM::halfwordSignedDataTransfer(u32 instr)
{
    int half      = bits< 5, 1>(instr);
    int sign      = bits< 6, 1>(instr);
    int rd        = bits<12, 4>(instr);
    int rn        = bits<16, 4>(instr);
    int load      = bits<20, 1>(instr);
    int writeback = bits<21, 1>(instr);
    int use_imm   = bits<22, 1>(instr);
    int increment = bits<23, 1>(instr);
    int pre_index = bits<24, 1>(instr);

    u32 addr = regs[rn];
    u32& dst = regs[rd];

    // Post-indexing forces writeback
    writeback |= !pre_index;

    u32 offset;
    if (use_imm)
    {
        int lower = bits<0, 4>(instr);
        int upper = bits<8, 4>(instr);
        offset = (upper << 4) | lower;
    }
    else
    {
        int rm = bits<0, 4>(instr);
        offset = regs[rm];
    }

    if (pre_index)
    {
        if (increment)
            addr += offset;
        else
            addr -= offset;
    }

    cycle(pc, NSEQ);

    if (load)
    {
        cycle();
        if (rd == 15)
            cycle(pc + 4, NSEQ);

        switch ((sign << 1) | half)
        {
        // LDRH
        case 0b01:
            dst = ldrh(addr);
            break;

        // LDRSB
        case 0b10:
            dst = mmu.readByte(addr);
            dst = signExtend<8>(dst);
            break;

        // LDRSH
        case 0b11:
            dst = ldrsh(addr);
            break;
        }

        // Prevent overwriting loaded value
        if (rd == rn)
            writeback = false;

        if (rd == 15)
        {
            dst = alignWord(dst);
            advance();

            cycle(pc, SEQ);
        }
        cycle(pc + 4, SEQ);
    }
    else  // STRH
    {
        u32 value = dst;
        // Account for prefetch
        if (rd == 15) value += 4;

        mmu.writeHalf(addr, value);

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

void ARM::blockDataTransfer(u32 instr)
{
    int rlist     = bits< 0, 16>(instr);
    int rn        = bits<16,  4>(instr);
    int load      = bits<20,  1>(instr);
    int writeback = bits<21,  1>(instr);
    int user      = bits<22,  1>(instr);
    int ascending = bits<23,  1>(instr);
    int full      = bits<24,  1>(instr);

    u32 addr = regs[rn];

    // Force user register transfer
    PSR::Mode mode = static_cast<PSR::Mode>(cpsr.mode);
    if (user)
        switchMode(PSR::USR);

    if (rlist != 0)
    {
        cycle(pc, NSEQ);

        // Register count needed for cycles
        int rcount = countBits(rlist) + countBits(rlist >> 8);

        int init = ascending ? 0 : 15;
        int loop = ascending ? 1 : -1;
        int step = ascending ? 4 : -4;

        // Lowest register is stored at the lowest address
        if (load)
        {
            // Prevent overwriting loaded value
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
                        cycle(pc + 4, NSEQ);

                    regs[x] = mmu.readWord(addr);

                    if (x == 15)
                    {
                        pc = alignWord(pc);
                        advance();

                        cycle(pc, SEQ);
                    }

                    if (!full) addr += step;
                }
            }
            cycle(pc + 4, SEQ);
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

                    mmu.writeWord(addr, regs[x]);

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
            pc = mmu.readWord(addr);
            pc = alignWord(pc);
            advance();
        }
        else
        {
            // Save address of next instruction
            mmu.writeWord(addr, pc + 4);
        }
        addr += ascending ? 0x40 : -0x40;
    }

    if (writeback)
        regs[rn] = addr;

    if (user)
        switchMode(mode);
}

void ARM::singleDataSwap(u32 instr)
{
    int rm   = bits< 0, 4>(instr);
    int rd   = bits<12, 4>(instr);
    int rn   = bits<16, 4>(instr);
    int byte = bits<22, 1>(instr);

    u32 addr = regs[rn];
    u32& dst = regs[rd];
    u32  src = regs[rm];

    cycle(pc, NSEQ);
    cycle(addr, NSEQ);

    if (byte)
    {
        dst = mmu.readByte(addr);
        mmu.writeByte(addr, src);
    }
    else
    {
        dst = ldr(addr);
        mmu.writeWord(addr, src);
    }

    cycle();
    cycle(pc + 4, SEQ);
}
