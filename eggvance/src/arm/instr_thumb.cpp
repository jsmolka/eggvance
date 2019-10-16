#include "arm.h"

#ifdef _MSC_VER
#include <intrin.h>
#endif

#include "common/macros.h"
#include "common/utility.h"

void ARM::moveShiftedRegister(u16 instr)
{
    int rd     = bits<0, 3>(instr);
    int rs     = bits<3, 3>(instr);
    int offset = bits<6, 5>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    bool carry = cpsr.c;
    switch (Shift(bits<11, 2>(instr)))
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

    int rn = bits<6, 3>(instr);

    u32& dst = regs[bits<0, 3>(instr)];
    u32  src = regs[bits<3, 3>(instr)];

    switch (Operation(bits<9, 2>(instr)))
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

    int offset = bits<0, 8>(instr);

    u32& dst = regs[bits<8, 3>(instr)];

    switch (Operation(bits<11, 2>(instr)))
    {
    case Operation::MOV: dst = logical(dst, true); break;
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

    u32& dst = regs[bits<0, 3>(instr)];
    u32  src = regs[bits<3, 3>(instr)];

    bool carry = cpsr.c;
    switch (Operation(bits<6, 4>(instr)))
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
        sub(dst, src, false);
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

    int rd = bits<0, 3>(instr);
    int rs = bits<3, 3>(instr);
    int hs = bits<6, 1>(instr);
    int hd = bits<7, 1>(instr);

    rs |= hs << 3;
    rd |= hd << 3;

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (Operation(bits<8, 2>(instr)))
    {
    case Operation::ADD:
        if (rd != 15)
        {
            dst += src;
            cycle<Access::Seq>(pc + 4);
        }
        else
        {
            cycle<Access::Nonseq>(pc + 4);
            dst = alignHalf(dst + src);
            refill<State::Thumb>();
        }
        break;

    case Operation::CMP:
        sub(dst, src, true);
        cycle<Access::Seq>(pc + 4);
        break;

    case Operation::MOV:
        if (rd != 15)
        {
            dst = src;
            cycle<Access::Seq>(pc + 4);
        }
        else
        {
            cycle<Access::Nonseq>(pc + 4);
            dst = alignHalf(src);
            refill<State::Thumb>();
        }
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

    u32& dst = regs[bits<8, 3>(instr)];
    u32 addr = alignWord(pc) + (offset << 2);

    dst = readWord(addr);

    cycle<Access::Nonseq>(addr);
    cycle<Access::Seq>(pc + 4);
    cycle();
}

void ARM::loadStoreRegisterOffset(u16 instr)
{
    int rd = bits<0, 3>(instr);
    int rb = bits<3, 3>(instr);
    int ro = bits<6, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[ro];

    bool byte = isset<10>(instr);
    bool load = isset<11>(instr);

    cycle<Access::Nonseq>(pc + 4);

    if (load)
    {
        dst = byte
            ? readByte(addr)
            : readWordRotated(addr);

        cycle<Access::Seq>(pc + 4);
        cycle();
    }
    else
    {
        if (byte)
            writeByte(addr, dst);
        else
            writeWord(addr, dst);

        cycle<Access::Nonseq>(addr);
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
    enum Operation
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

    bool load = isset<11>(instr);

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
    
    cycle(pc, NSEQ);

    if (load)
    {
        cycle();
        dst = readWordRotated(addr);
        cycle(pc + 2, SEQ);
    }
    else
    {
        writeWord(addr, dst);
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
        dst = sp + offset;
    else
        dst = alignWord(pc) + offset;

    cycle(pc + 2, SEQ);
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

    cycle(pc + 2, SEQ);
}

void ARM::pushPopRegisters(u16 instr)
{
    int rlist = bits< 0, 8>(instr);
    int pc_lr = bits< 8, 1>(instr);
    int pop   = bits<11, 1>(instr);

    // Register count needed for cycles
    int rcount = EGG_MSVC(__popcnt16(rlist));

    cycle(pc, NSEQ);

    // Full descending stack
    if (pop)
    {
        for (int x = 0; rcount > 0; ++x)
        {
            if (rlist & (1 << x))
            {
                if (--rcount > 0)
                    cycle(sp, SEQ);
                else
                    cycle();

                regs[x] = readWord(sp);
                sp += 4;
            }
        }

        if (pc_lr)
        {
            cycle(pc + 2, NSEQ);

            pc = readWord(sp);
            pc = alignHalf(pc);
            advance();
            sp += 4;

            cycle(pc, SEQ);
        }
        cycle(pc + 2, SEQ);
    }
    else
    {
        if (pc_lr)
        {
            sp -= 4;
            writeWord(sp, lr);
        }

        for (int x = 7; rcount > 0; --x)
        {
            if (rlist & (1 << x))
            {
                if (--rcount > 0)
                    cycle(sp, SEQ);

                sp -= 4;
                writeWord(sp, regs[x]);
            }
        }
        cycle(sp, NSEQ);
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
        cycle(pc, NSEQ);
    
        // Register count needed for cycles
        int rcount = EGG_MSVC(__popcnt16(rlist));

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

                    regs[x] = readWord(addr);

                    addr += 4;
                }
            }
            cycle(pc + 2, SEQ);
        }
        else
        {
            for (int x = 0; rcount > 0; ++x)
            {
                if (rlist & (1 << x))
                {
                    if (--rcount > 0)
                        cycle(addr, SEQ);

                    writeWord(addr, regs[x]);

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
            pc = readWord(alignWord(addr));
            pc = alignHalf(pc);
            advance();
        }
        else
        {
            // Save address of next instruction
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

    if (cpsr.check(static_cast<PSR::Condition>(condition)))
    {
        offset = signExtend<8>(offset);
        offset <<= 1;

        cycle(pc, NSEQ);

        pc += offset;
        advance();

        cycle(pc, SEQ);
    }
    cycle(pc + 2, SEQ);
}

void ARM::unconditionalBranch(u16 instr)
{
    int offset = bits<0, 11>(instr);
    
    offset = signExtend<11>(offset);
    offset <<= 1;

    cycle(pc, NSEQ);

    pc += offset;
    advance();

    cycle(pc, SEQ);
    cycle(pc + 2, SEQ);
}

void ARM::longBranchLink(u16 instr)
{
    int offset = bits< 0, 11>(instr);
    int second = bits<11,  1>(instr);

    if (second)
    {
        offset <<= 1;

        u32 next = (pc - 2) | 1;

        cycle(pc, NSEQ);

        pc = lr + offset;
        lr = next;
        advance();

        cycle(pc, SEQ);
        cycle(pc + 2, SEQ);
    }
    else
    {
        offset = signExtend<11>(offset);
        offset <<= 12;

        cycle(pc, SEQ);

        lr = pc + offset;
    }
}
