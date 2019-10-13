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

void ARM::branchExchange(u32 instr)
{
    u32 addr = regs[bits<0, 4>(instr)];

    cycle<Access::Nonseq>(pc + 8);

    if (cpsr.thumb = addr & 0x1)
    {
        pc = alignHalf(addr);
        cycle<Access::Seq>(pc);
        cycle<Access::Seq>(pc + 2);
        advance<2>();
    }
    else
    {
        pc = alignWord(addr);
        cycle<Access::Seq>(pc);
        cycle<Access::Seq>(pc + 4);
        advance<4>();
    }
}

void ARM::branchLink(u32 instr)
{
    if (isset<24>(instr))
        lr = pc - 4;

    cycle<Access::Nonseq>(pc + 8);

    pc += signExtend<24>(instr) << 2;

    cycle<Access::Seq>(pc);
    cycle<Access::Seq>(pc + 4);

    advance<4>();
}

void ARM::dataProcessing(u32 instr)
{
    int rd = bits<12, 4>(instr);
    int rn = bits<16, 4>(instr);

    u32& dst = regs[rd];
    u32  op1 = regs[rn];
    u32  op2 = 0;

    cycle<Access::Nonseq>(pc + 8);

    bool carry = cpsr.c;
    if (isset<25>(instr))
    {
        u32 value  = bits<0, 8>(instr);
        int amount = bits<8, 4>(instr);

        op2 = ror(value, amount << 1, carry, false);
    }
    else
    {
        int rm   = bits<0, 4>(instr);
        int type = bits<5, 2>(instr);

        op2 = regs[rm];
        if (isset<4>(instr))
        {
            cycle();

            if (rn == 15) op1 += 4;
            if (rm == 15) op2 += 4;

            int amount = regs[bits<8, 4>(instr)] & 0xFF;
            op2 = applyShift(Shift(type), op2, amount, carry, false);
        }
        else
        {
            int amount = bits<7, 5>(instr);
            op2 = applyShift(Shift(type), op2, amount, carry, true);
        }
    }

    bool flags = isset<20>(instr);
    if (rd == 15 && flags)
    {
        PSR spsr = this->spsr;
        switchMode(spsr.mode);
        cpsr = spsr;

        flags = false;
    }

    int opcode = bits<21, 4>(instr);
    switch (Operation(opcode))
    {
    case Operation::CMN:       add(op1, op2, flags); break;
    case Operation::CMP:       sub(op1, op2, flags); break;
    case Operation::ADD: dst = add(op1, op2, flags); break;
    case Operation::SUB: dst = sub(op1, op2, flags); break;
    case Operation::RSB: dst = sub(op2, op1, flags); break;
    case Operation::ADC: dst = add(op1, op2 + cpsr.c + 0, flags); break;
    case Operation::SBC: dst = sub(op1, op2 - cpsr.c + 1, flags); break;
    case Operation::RSC: dst = sub(op2, op1 - cpsr.c + 1, flags); break;
    case Operation::TST:       logical(op1 &  op2, carry, flags); break;
    case Operation::TEQ:       logical(op1 ^  op2, carry, flags); break;
    case Operation::AND: dst = logical(op1 &  op2, carry, flags); break;
    case Operation::EOR: dst = logical(op1 ^  op2, carry, flags); break;
    case Operation::ORR: dst = logical(op1 |  op2, carry, flags); break;
    case Operation::MOV: dst = logical(       op2, carry, flags); break;
    case Operation::BIC: dst = logical(op1 & ~op2, carry, flags); break;
    case Operation::MVN: dst = logical(      ~op2, carry, flags); break;

    default:
        EGG_UNREACHABLE;
        break;
    }

    if (rd == 15 && (opcode >> 2) != 0b10)
    {
        if (cpsr.thumb)
        {
            dst = alignHalf(dst);
            cycle<Access::Seq>(pc);
            cycle<Access::Seq>(pc + 2);
            advance<2>();
        }
        else
        {
            dst = alignWord(dst);
            cycle<Access::Seq>(pc);
            cycle<Access::Seq>(pc + 4);
            advance<4>();
        }
    }
}

void ARM::psrTransfer(u32 instr)
{
    int write    = bits<21, 1>(instr);
    int use_spsr = bits<22, 1>(instr);

    if (write)
    {
        int use_imm = bits<25,  1>(instr);

        u32 op;
        if (use_imm)
        {
            u32 value  = bits<0, 8>(instr);
            int amount = bits<8, 4>(instr);
            
            bool carry;
            op = ror(value, amount << 1, carry, false);
        }
        else
        {
            int rm = bits<0, 4>(instr);
            op = regs[rm];
        }

        u32 mask = 0;
        if (instr & (1 << 16)) mask |= 0x0000'00FF;
        if (instr & (1 << 17)) mask |= 0x0000'FF00;
        if (instr & (1 << 18)) mask |= 0x00FF'0000;
        if (instr & (1 << 19)) mask |= 0xFF00'0000;

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
        }
    }
    else
    {
        int rd = bits<12, 4>(instr);
        regs[rd] = use_spsr ? spsr : cpsr;
    }
    cycle<Access::Seq>(pc + 4);
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
        logical_old(dst);

    cycleBooth(op1, true);
    cycle<Access::Seq>(pc + 4);
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
    cycle<Access::Seq>(pc + 4);
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
        int shift   = bits<5, 2>(data);

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

        bool carry = true;
        switch (static_cast<Shift>(shift))
        {
        case Shift::LSL: offset = lsl(value, amount, carry); break;
        case Shift::LSR: offset = lsr(value, amount, carry); break;
        case Shift::ASR: offset = asr(value, amount, carry); break;
        case Shift::ROR: offset = ror(value, amount, carry); break;

        default:
            EGG_UNREACHABLE;
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
            dst = readByte(addr);
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
            writeByte(addr, value);
        else
            writeWord(addr, value);

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
        case 0b00:
            break;

        // LDRH
        case 0b01:
            dst = ldrh(addr);
            break;

        // LDRSB
        case 0b10:
            dst = readByte(addr);
            dst = signExtend<8>(dst);
            break;

        // LDRSH
        case 0b11:
            dst = ldrsh(addr);
            break;

        default:
            EGG_UNREACHABLE;
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

        writeHalf(addr, value);

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
        switchMode(PSR::Mode::USR);

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

                    regs[x] = readWord(addr);

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

                    writeWord(addr, regs[x]);

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
            pc = readWord(addr);
            pc = alignWord(pc);
            advance();
        }
        else
        {
            // Save address of next instruction
            writeWord(addr, pc + 4);
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
        dst = readByte(addr);
        writeByte(addr, src);
    }
    else
    {
        dst = ldr(addr);
        writeWord(addr, src);
    }

    cycle();
    cycle(pc + 4, SEQ);
}
