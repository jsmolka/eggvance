#include "arm.h"

#include "common/macros.h"
#include "common/utility.h"

enum class ALU
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
    int rd     = bits<12, 4>(instr);
    int rn     = bits<16, 4>(instr);
    int opcode = bits<21, 4>(instr);

    u32& dst = regs[rd];
    u32  op1 = regs[rn];
    u32  op2 = 0;

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
            op2 = shift(Shift(type), op2, amount, carry, false);
        }
        else
        {
            int amount = bits<7, 5>(instr);
            op2 = shift(Shift(type), op2, amount, carry, true);
        }
    }

    bool flags = isset<20>(instr);
    bool write = (opcode >> 2) != 0b10;

    if (rd == 15 && write)
    {
        if (flags)
        {
            PSR spsr = this->spsr;
            switchMode(spsr.mode);
            cpsr = spsr;

            flags = false;
        }
        cycle<Access::Nonseq>(pc + 8);
    }
    else
    {
        cycle<Access::Seq>(pc + 8);
    }

    switch (ALU(opcode))
    {
    case ALU::CMN:       add(op1, op2, flags); break;
    case ALU::CMP:       sub(op1, op2, flags); break;
    case ALU::ADD: dst = add(op1, op2, flags); break;
    case ALU::SUB: dst = sub(op1, op2, flags); break;
    case ALU::RSB: dst = sub(op2, op1, flags); break;
    case ALU::ADC: dst = add(op1, op2 + cpsr.c + 0, flags); break;
    case ALU::SBC: dst = sub(op1, op2 - cpsr.c + 1, flags); break;
    case ALU::RSC: dst = sub(op2, op1 - cpsr.c + 1, flags); break;
    case ALU::TST:       logical(op1 &  op2, carry, flags); break;
    case ALU::TEQ:       logical(op1 ^  op2, carry, flags); break;
    case ALU::AND: dst = logical(op1 &  op2, carry, flags); break;
    case ALU::EOR: dst = logical(op1 ^  op2, carry, flags); break;
    case ALU::ORR: dst = logical(op1 |  op2, carry, flags); break;
    case ALU::MOV: dst = logical(       op2, carry, flags); break;
    case ALU::BIC: dst = logical(op1 & ~op2, carry, flags); break;
    case ALU::MVN: dst = logical(      ~op2, carry, flags); break;

    default:
        EGG_UNREACHABLE;
        break;
    }

    if (rd == 15 && write)
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
    bool use_spsr = isset<22>(instr);

    if (isset<21>(instr))
    {
        constexpr int f_bit = 1 << 19;
        constexpr int s_bit = 1 << 18;
        constexpr int x_bit = 1 << 17;
        constexpr int c_bit = 1 << 16;

        u32 op = 0;
        if (isset<25>(instr))
        {
            u32 value  = bits<0, 8>(instr);
            int amount = bits<8, 4>(instr);
            
            op = ror(value, amount << 1, false);
        }
        else
        {
            op = regs[bits<0, 4>(instr)];
        }

        u32 mask = 0;
        if (instr & f_bit) mask |= 0xFF00'0000;
        if (instr & s_bit) mask |= 0x00FF'0000;
        if (instr & x_bit) mask |= 0x0000'FF00;
        if (instr & c_bit) mask |= 0x0000'00FF;

        op &= mask;
        if (use_spsr)
        {
            spsr = (spsr & ~mask) | op;
        }
        else
        {
            if (instr & c_bit)
                switchMode(PSR::Mode(op & 0x1F));

            cpsr = (cpsr & ~mask) | op;
        }
    }
    else
    {
        regs[bits<12, 4>(instr)] = use_spsr ? spsr : cpsr;
    }
    cycle<Access::Seq>(pc + 8);
}

void ARM::multiply(u32 instr)
{
    u32  op1 = regs[bits< 0, 4>(instr)];
    u32  op2 = regs[bits< 8, 4>(instr)];
    u32  op3 = regs[bits<12, 4>(instr)];
    u32& dst = regs[bits<16, 4>(instr)];

    dst = op1 * op2;
    if (isset<21>(instr))
    {
        dst += op3;
        cycle();
    }
    logical(dst, isset<20>(instr));

    cycleBooth(op2, true);
    cycle<Access::Seq>(pc + 8);
}

void ARM::multiplyLong(u32 instr)
{
    u64  op1  = regs[bits< 0, 4>(instr)];
    u64  op2  = regs[bits< 8, 4>(instr)];
    u32& dstl = regs[bits<12, 4>(instr)];
    u32& dsth = regs[bits<16, 4>(instr)];

    int sign = isset<22>(instr);
    if (sign)
    {
        op1 = signExtend<32>(op1);
        op2 = signExtend<32>(op2);
    }
    
    u64 result = op1 * op2;
    if (isset<21>(instr))
    {
        result += (static_cast<u64>(dsth) << 32) | dstl;
        cycle();
    }

    if (isset<20>(instr))
    {
        cpsr.z = result == 0;
        cpsr.n = result >> 63;
    }

    dstl = static_cast<u32>(result);
    dsth = static_cast<u32>(result >> 32);

    cycle();
    cycleBooth(static_cast<u32>(op2), sign);
    cycle<Access::Seq>(pc + 8);
}

#define INDEX           \
    if (increment)      \
        addr += offset; \
    else                \
        addr -= offset

#define PRE_INDEX       \
    if (pre_index)      \
        INDEX

#define POST_INDEX      \
    if (!pre_index)     \
        INDEX

void ARM::singleDataTransfer(u32 instr)
{
    int rd = bits<12, 4>(instr);
    int rn = bits<16, 4>(instr);

    u32 addr = regs[rn];
    u32& dst = regs[rd];

    bool load      = isset<20>(instr);
    bool writeback = isset<21>(instr);
    bool byte      = isset<22>(instr);
    bool increment = isset<23>(instr);
    bool pre_index = isset<24>(instr);

    writeback |= !pre_index;

    u32 offset = 0;
    if (isset<25>(instr))
    {
        int rm   = bits<0, 4>(instr);
        int type = bits<5, 2>(instr);

        int amount = 0;
        if (isset<4>(instr))
        {
            amount = regs[bits<8, 4>(instr)];
            amount &= 0xFF;
        }
        else
        {
            amount = bits<7, 5>(instr);
        }
        offset = shift(Shift(type), regs[rm], amount, true);
    }
    else
    {
        offset = bits<0, 12>(instr);
    }

    PRE_INDEX;

    cycle<Access::Nonseq>(addr);
    cycle<Access::Nonseq>(pc + 8);

    if (load)
    {
        dst = byte
            ? readByte(addr)
            : readWordRotated(addr);

        if (rd == 15)
        {
            pc = alignWord(pc);
            cycle<Access::Seq>(pc);
            cycle<Access::Seq>(pc + 4);
            advance<4>();
        }
        cycle();
    }
    else
    {
        u32 value = (rd == rn)
            ? addr
            : (rd == 15)
                ? dst + 4
                : dst + 0;

        if (byte)
            writeByte(addr, value);
        else
            writeWord(addr, value);

    }

    if (writeback && (rd != rn || !load))
    {
        POST_INDEX;
        regs[rn] = addr;
    }
}

void ARM::halfwordSignedDataTransfer(u32 instr)
{
    int rd = bits<12, 4>(instr);
    int rn = bits<16, 4>(instr);

    u32 addr = regs[rn];
    u32& dst = regs[rd];

    bool writeback = isset<21>(instr);
    bool increment = isset<23>(instr);
    bool pre_index = isset<24>(instr);

    writeback |= !pre_index;

    u32 offset = 0;
    if (isset<22>(instr))
    {
        int lower = bits<0, 4>(instr);
        int upper = bits<8, 4>(instr);
        offset = (upper << 4) | lower;
    }
    else
    {
        offset = regs[bits<0, 4>(instr)];
    }

    PRE_INDEX;

    cycle<Access::Nonseq>(pc + 8);

    if (isset<20>(instr))
    {
        switch (bits<5, 2>(instr))
        {
        case 0b00:
            break;

        case 0b01:
            dst = readHalfRotated(addr);
            break;

        case 0b10:
            dst = readByte(addr);
            dst = signExtend<8>(dst);
            break;

        case 0b11:
            dst = readHalfSigned(addr);
            break;

        default:
            EGG_UNREACHABLE;
            break;
        }

        cycle();
        cycle<Access::Nonseq>(addr);

        if (rd == 15)
        {
            dst = alignWord(dst);
            cycle<Access::Seq>(pc);
            cycle<Access::Seq>(pc + 4);
            advance<4>();
        }
    }
    else
    {
        u32 value = (rd == 15)
            ? dst + 4
            : dst + 0;

        writeHalf(addr, value);

        cycle<Access::Nonseq>(addr);
    }

    if (writeback && rd != rn)
    {
        POST_INDEX;
        regs[rn] = addr;
    }
}

#undef INDEX
#undef PRE_INDEX
#undef POST_INDEX

void ARM::blockDataTransfer(u32 instr)
{
    int rlist = bits< 0, 16>(instr);
    int rn    = bits<16,  4>(instr);

    u32 addr = regs[rn];

    bool load      = isset<20>(instr);
    bool writeback = isset<21>(instr);
    bool user      = isset<22>(instr);
    bool ascending = isset<23>(instr);
    bool full      = isset<24>(instr);

    PSR::Mode mode = cpsr.mode;
    if (user)
        switchMode(PSR::Mode::USR);

    cycle<Access::Nonseq>(pc + 8);

    if (rlist != 0)
    {
        int rcount = countBits(rlist) + countBits(rlist >> 8);

        int init = ascending ? 0 : 15;
        int loop = ascending ? 1 : -1;
        int step = ascending ? 4 : -4;

        if (load)
        {
            if (rlist & (1 << rn))
                writeback = false;

            if (rlist & (1 << 15))
                cycle<Access::Nonseq>(pc + 4);

            for (int x = init; rcount > 0; x += loop)
            {
                if (rlist & (1 << x))
                {
                    if (full) addr += step;

                    if (--rcount > 0)
                        cycle<Access::Seq>(addr);
                    else
                        cycle();

                    regs[x] = readWord(addr);

                    if (!full) addr += step;
                }
            }

            if (rlist & (1 << 15))
            {
                pc = alignWord(pc);
                cycle<Access::Seq>(pc);
                cycle<Access::Seq>(pc + 4);
                advance();
            }
        }
        else
        {
            for (int x = init; rcount > 0; x += loop)
            {
                if (rlist & (1 << x))
                {
                    if (full) addr += step;

                    if (--rcount > 0)
                        cycle<Access::Seq>(addr);

                    writeWord(addr, regs[x]);

                    if (!full) addr += step;
                }
            }
            cycle<Access::Nonseq>(addr);
        }
    }
    else
    {
        if (load)
        {
            pc = readWord(addr);
            pc = alignWord(pc);
            advance<4>();
        }
        else
        {
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
    u32  src = regs[bits< 0, 4>(instr)];
    u32& dst = regs[bits<12, 4>(instr)];
    u32 addr = regs[bits<16, 4>(instr)];

    cycle<Access::Nonseq>(pc + 8);
    cycle<Access::Nonseq>(addr);

    if (isset<22>(instr))
    {
        dst = readByte(addr);
        writeByte(addr, src);
    }
    else
    {
        dst = readWordRotated(addr);
        writeWord(addr, src);
    }

    cycle<Access::Seq>(pc + 4);
    cycle();
}
