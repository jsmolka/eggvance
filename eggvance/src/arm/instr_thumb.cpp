#include "arm.h"

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include "common/macros.h"
#include "common/utility.h"

void ARM::moveShiftedRegister(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rs     = bits< 3, 3>(instr);
    int offset = bits< 6, 5>(instr);
    int opcode = bits<11, 2>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    bool carry = cpsr.c;
    switch (Shift(opcode))
    {
    case Shift::LSL: dst = lsl(src, offset, carry); break;
    case Shift::LSR: dst = lsr(src, offset, carry, true); break;
    case Shift::ASR: dst = asr(src, offset, carry, true); break;
    case Shift::ROR: break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    logical(dst, carry, true);

    cycle<Access::Seq>(pc + 4);
}

void ARM::addSubtractImmediate(u16 instr)
{
    enum class Operation
    {
        ADD_REG = 0b00,
        SUB_REG = 0b01,
        ADD_IMM = 0b10,
        SUB_IMM = 0b11
    };

    int rd     = bits<0, 3>(instr);
    int rs     = bits<3, 3>(instr);
    int rn     = bits<6, 3>(instr);
    int opcode = bits<9, 2>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (Operation(opcode))
    {
    case Operation::ADD_REG: dst = add(src, regs[rn], true); break;
    case Operation::SUB_REG: dst = sub(src, regs[rn], true); break;
    case Operation::ADD_IMM: dst = add(src, rn, true); break;
    case Operation::SUB_IMM: dst = sub(src, rn, true); break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    cycle<Access::Seq>(pc + 4);
}

void ARM::addSubtractMoveCompareImmediate(u16 instr)
{
    enum class Operation
    {
        MOV = 0b00,
        CMP = 0b01,
        ADD = 0b10,
        SUB = 0b11
    };

    int offset = bits< 0, 8>(instr);
    int rd     = bits< 8, 3>(instr); 
    int opcode = bits<11, 2>(instr);

    u32& dst = regs[rd];

    switch (Operation(opcode))
    {
    case Operation::MOV: dst = logical( offset, true); break;
    case Operation::CMP:       sub(dst, offset, true); break;
    case Operation::ADD: dst = add(dst, offset, true); break;
    case Operation::SUB: dst = sub(dst, offset, true); break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    cycle<Access::Seq>(pc + 4);
}

void ARM::aluOperations(u16 instr)
{
    enum class Operation
    {
        AND = 0b0000,
        EOR = 0b0001,
        LSL = 0b0010,
        LSR = 0b0011,
        ASR = 0b0100,
        ADC = 0b0101,
        SBC = 0b0110,
        ROR = 0b0111,
        TST = 0b1000,
        NEG = 0b1001,
        CMP = 0b1010,
        CMN = 0b1011,
        ORR = 0b1100,
        MUL = 0b1101,
        BIC = 0b1110,
        MVN = 0b1111
    };

    int rd     = bits<0, 3>(instr);
    int rs     = bits<3, 3>(instr);
    int opcode = bits<6, 4>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    bool carry = cpsr.c;
    switch (Operation(opcode))
    {
    case Operation::AND:
        dst = logical(dst & src, true);
        break;

    case Operation::ORR:
        dst = logical(dst | src, true);
        break;

    case Operation::EOR:
        dst = logical(dst ^ src, true);
        break;

    case Operation::LSL:
        dst = lsl(dst, src, carry);
        logical(dst, carry, true);
        cycle();
        break;

    case Operation::LSR:
        dst = lsr(dst, src, carry, false); 
        logical(dst, carry, true);
        cycle();
        break;

    case Operation::ASR:
        dst = asr(dst, src, carry, false); 
        logical(dst, carry, true);
        cycle();
        break;

    case Operation::ROR:
        dst = ror(dst, src, carry, false);
        logical(dst, carry, true);
        cycle();
        break;

    case Operation::MUL:
        cycleBooth(dst, true);
        dst = logical(dst * src, true);
        break;

    case Operation::ADC:
        dst = add(dst, src + cpsr.c, true);
        break;

    case Operation::SBC:
        dst = sub(dst, src - cpsr.c + 1, true);
        break;

    case Operation::TST:
        logical(dst & src, true);
        break;

    case Operation::NEG:
        dst = sub(0, src, true);
        break;

    case Operation::CMP:
        sub(dst, src, true);
        break;

    case Operation::CMN:       
        add(dst, src, true);
        break;

    case Operation::BIC:
        dst = logical(dst & ~src, true);
        break;

    case Operation::MVN: 
        dst = logical(~src, true);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    cycle<Access::Seq>(pc + 4);
}

void ARM::highRegisterBranchExchange(u16 instr)
{
    enum class Operation
    {
        ADD = 0b00,
        CMP = 0b01,
        MOV = 0b10,
        BX  = 0b11
    };

    int rd     = bits<0, 3>(instr);
    int rs     = bits<3, 3>(instr);
    int hs     = bits<6, 1>(instr);
    int hd     = bits<7, 1>(instr);
    int opcode = bits<8, 2>(instr);

    rs |= hs << 3;
    rd |= hd << 3;

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (Operation(opcode))
    {
    case Operation::ADD:
        if (rd == 15)
        {
            cycle<Access::Nonseq>(pc + 4);
            dst = alignHalf(dst + src);
            refill<State::Thumb>();
        }
        else
        {
            dst += src;
            cycle<Access::Seq>(pc + 4);
        }
        break;

    case Operation::MOV:
        if (rd == 15)
        {
            cycle<Access::Nonseq>(pc + 4);
            dst = alignHalf(src);
            refill<State::Thumb>();
        }
        else
        {
            dst = src;
            cycle<Access::Seq>(pc + 4);
        }
        break;

    case Operation::CMP:
        sub(dst, src, true);
        cycle<Access::Seq>(pc + 4);
        break;

    case Operation::BX:
        cycle<Access::Nonseq>(pc + 4);
        if (cpsr.thumb = src & 0x1)
        {
            pc = alignHalf(src);
            refill<State::Thumb>();
        }
        else
        {
            pc = alignWord(src);
            refill<State::Arm>();
        }
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void ARM::loadPCRelative(u16 instr)
{
    int offset = bits<0, 8>(instr);
    int rd     = bits<8, 3>(instr);

    offset <<= 2;

    u32 addr = alignWord(pc + offset);

    regs[rd] = readWord(addr);

    cycle<Access::Nonseq>(addr);
    cycle<Access::Seq>(pc + 4);
    cycle();
}

void ARM::loadStoreRegisterOffset(u16 instr)
{
    enum class Operation
    {
        STR  = 0b00,
        STRB = 0b01,
        LDR  = 0b10,
        LDRB = 0b11
    };

    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int ro     = bits< 6, 3>(instr);
    int opcode = bits<10, 2>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[ro];

    cycle<Access::Nonseq>(pc + 4);

    switch (Operation(opcode))
    {
    case Operation::STR:
        writeWord(addr, dst);
        cycle<Access::Nonseq>(addr);
        break;

    case Operation::STRB:
        writeByte(addr, dst);
        cycle<Access::Nonseq>(addr);
        break;

    case Operation::LDR:
        dst = readWordRotated(addr);
        cycle<Access::Seq>(pc + 4);
        cycle();
        break;

    case Operation::LDRB:
        dst = readByte(addr);
        cycle<Access::Seq>(pc + 4);
        cycle();
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void ARM::loadStoreHalfwordSigned(u16 instr)
{
    enum class Operation
    {
        STRH  = 0b00,
        LDRSB = 0b01,
        LDRH  = 0b10,
        LDRSH = 0b11
    };

    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int ro     = bits< 6, 3>(instr);
    int opcode = bits<10, 2>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[ro];

    cycle<Access::Nonseq>(pc + 4);

    switch (Operation(opcode))
    {
    case Operation::STRH:
        writeHalf(addr, dst);
        cycle<Access::Nonseq>(addr);
        break;

    case Operation::LDRSB:
        dst = readByte(addr);
        dst = signExtend<8>(dst);
        cycle<Access::Seq>(pc + 4);
        cycle();
        break;

    case Operation::LDRH:
        dst = readHalfRotated(addr);
        cycle<Access::Seq>(pc + 4);
        cycle();
        break;

    case Operation::LDRSH:
        dst = readHalfSigned(addr);
        cycle<Access::Seq>(pc + 4);
        cycle();
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void ARM::loadStoreImmediateOffset(u16 instr)
{
    enum class Operation
    {
        STR  = 0b00,
        LDR  = 0b01,
        STRB = 0b10,
        LDRB = 0b11
    };

    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int offset = bits< 6, 5>(instr);
    int opcode = bits<11, 2>(instr);

    if (~opcode & 0x2)
        offset <<= 2;

    u32& dst = regs[rd];
    u32 addr = regs[rb] + offset;

    cycle<Access::Nonseq>(pc + 4);

    switch (Operation(opcode))
    {
    case Operation::STR:
        writeWord(addr, dst);
        cycle<Access::Nonseq>(addr);
        break;

    case Operation::STRB:
        writeByte(addr, dst);
        cycle<Access::Nonseq>(addr);
        break;

    case Operation::LDR:
        dst = readWordRotated(addr);
        cycle<Access::Seq>(pc + 4);
        cycle();
        break;

    case Operation::LDRB:
        dst = readByte(addr);
        cycle<Access::Seq>(pc + 4);
        cycle();
        break;

    default:
        EGG_UNREACHABLE;
        break;
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

    cycle<Access::Nonseq>(pc + 4);

    if (load)
    {
        dst = readHalfRotated(addr);
        cycle<Access::Seq>(pc + 4);
        cycle();
    }
    else
    {
        writeHalf(addr, dst);
        cycle<Access::Nonseq>(addr);
    }
}

void ARM::loadStoreSPRelative(u16 instr)
{
    int offset = bits< 0, 8>(instr);
    int rd     = bits< 8, 3>(instr);
    int load   = bits<11, 1>(instr);

    offset <<= 2;

    u32& dst = regs[rd];
    u32 addr = sp + offset;
    
    cycle<Access::Nonseq>(pc + 4);

    if (load)
    {
        dst = readWordRotated(addr);
        cycle<Access::Seq>(pc + 4);
        cycle();
    }
    else
    {
        writeWord(addr, dst);
        cycle<Access::Nonseq>(addr);
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
        dst = sp + offset;
    else
        dst = alignWord(pc + offset);

    cycle<Access::Seq>(pc + 4);
}

void ARM::addOffsetSP(u16 instr)
{
    int offset = bits<0, 7>(instr);
    int sign   = bits<7, 1>(instr);

    offset <<= 2;

    if (sign)
        sp -= offset;
    else
        sp += offset; 

    cycle<Access::Seq>(pc + 4);
}

void ARM::pushPopRegisters(u16 instr)
{
    int rlist = bits< 0, 8>(instr);
    int pc_lr = bits< 8, 1>(instr);
    int pop   = bits<11, 1>(instr);

    cycle<Access::Nonseq>(pc + 4);

    if (pop)
    {
        unsigned long begl = 0;
        unsigned long endl = 0;
        EGG_MSVC(_BitScanForward(&begl, rlist));
        EGG_MSVC(_BitScanReverse(&endl, rlist));

        int beg = static_cast<int>(begl);
        int end = static_cast<int>(endl);

        for (int x = beg; x <= end; ++x)
        {
            if (~rlist & (1 << x))
                continue;

            if (beg != end)
                cycle<Access::Seq>(sp);
            else
                cycle();

            regs[x] = readWord(sp);

            sp += 4;
        }

        if (pc_lr)
        {
            cycle<Access::Nonseq>(pc + 4);

            pc = readWord(sp);
            pc = alignHalf(pc);
            refill<State::Thumb>();

            sp += 4;
        }
        else
        {
            cycle<Access::Seq>(pc + 4);
        }
    }
    else
    {
        unsigned long begl = 0;
        unsigned long endl = 0;
        EGG_MSVC(_BitScanReverse(&begl, rlist));
        EGG_MSVC(_BitScanForward(&endl, rlist));

        int beg = static_cast<int>(begl);
        int end = static_cast<int>(endl);

        if (pc_lr)
        {
            sp -= 4;
            writeWord(sp, lr);
        }

        for (int x = beg; x >= end; --x)
        {
            if (~rlist & (1 << x))
                continue;

            if (beg != end)
                cycle<Access::Seq>(sp);

            sp -= 4;
            writeWord(sp, regs[x]);
        }
        cycle<Access::Nonseq>(sp);
    }
}

// Todo: block data transfer addr? :)
void ARM::loadStoreMultiple(u16 instr)
{
    int rlist = bits< 0, 8>(instr);
    int rb    = bits< 8, 3>(instr);
    int load  = bits<11, 1>(instr);

    u32 addr = alignWord(regs[rb]);

    bool writeback = true;

    cycle<Access::Nonseq>(pc + 4);

    if (rlist != 0)
    {
        unsigned long begl = 0;
        unsigned long endl = 0;
        EGG_MSVC(_BitScanForward(&begl, rlist));
        EGG_MSVC(_BitScanReverse(&endl, rlist));

        int beg = static_cast<int>(begl);
        int end = static_cast<int>(endl);

        if (load)
        {
            if (rlist & (1 << rb))
                writeback = false;

            for (int x = beg; x <= end; ++x)
            {
                if (~rlist & (1 << x))
                    continue;

                if (beg != end)
                    cycle<Access::Seq>(addr);
                else
                    cycle();

                regs[x] = readWord(addr);

                addr += 4;
            }
            cycle<Access::Seq>(pc + 4);
        }
        else
        {
            for (int x = beg; x <= end; ++x)
            {
                if (~rlist & (1 << x))
                    continue;

                if (beg != end)
                    cycle<Access::Seq>(addr);

                writeWord(addr, regs[x]);

                addr += 4;
            }
            cycle<Access::Nonseq>(addr);
        }
    }
    else
    {
        // Todo: need to align here?
        if (load)
        {
            pc = readWord(alignWord(addr));
            pc = alignHalf(pc);
            refill<State::Thumb>();
        }
        else
        {
            writeWord(alignWord(addr), pc + 2);
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

    if (cpsr.check(PSR::Condition(condition)))
    {
        offset = signExtend<8>(offset);
        offset <<= 1;

        cycle<Access::Nonseq>(pc + 4);

        pc += offset;
        refill<State::Thumb>();
    }
    else
    {
        cycle<Access::Seq>(pc + 4);
    }
}

void ARM::unconditionalBranch(u16 instr)
{
    int offset = bits<0, 11>(instr);
    
    offset = signExtend<11>(offset);
    offset <<= 1;

    cycle<Access::Nonseq>(pc + 4);

    pc += offset;
    refill<State::Thumb>();
}

void ARM::longBranchLink(u16 instr)
{
    int offset = bits<0, 11>(instr);
    int second = bits<11, 1>(instr);

    cycle<Access::Nonseq>(pc + 4);

    if (second)
    {
        offset <<= 1;

        u32 next = (pc - 2) | 1;
        pc = lr + offset;
        lr = next;

        refill<State::Thumb>();
    }
    else
    {
        offset = signExtend<11>(offset);
        offset <<= 12;

        lr = pc + offset;
    }
}
