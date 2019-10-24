#include "arm.h"

#include "common/macros.h"
#include "common/utility.h"

void ARM::Thumb_MoveShiftedRegister(u16 instr)
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
}

void ARM::Thumb_AddSubtract(u16 instr)
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
}

void ARM::Thumb_ImmediateOperations(u16 instr)
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
}

void ARM::Thumb_ALUOperations(u16 instr)
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
        idle();
        break;

    case Operation::LSR:
        dst = lsr(dst, src, carry, false); 
        logical(dst, carry, true);
        idle();
        break;

    case Operation::ASR:
        dst = asr(dst, src, carry, false); 
        logical(dst, carry, true);
        idle();
        break;

    case Operation::ROR:
        dst = ror(dst, src, carry, false);
        logical(dst, carry, true);
        idle();
        break;

    case Operation::MUL:
        booth(dst, true);
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
}

void ARM::Thumb_HighRegisterOperations(u16 instr)
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
            dst = alignHalf(dst + src);
            flushPipeHalf();
        }
        else
        {
            dst += src;
        }
        break;

    case Operation::MOV:
        if (rd == 15)
        {
            dst = alignHalf(src);
            flushPipeHalf();
        }
        else
        {
            dst = src;
        }
        break;

    case Operation::CMP:
        sub(dst, src, true);
        break;

    case Operation::BX:
        if (cpsr.thumb = src & 0x1)
        {
            pc = alignHalf(src);
            flushPipeHalf();
        }
        else
        {
            pc = alignWord(src);
            flushPipeWord();
        }
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void ARM::Thumb_LoadPCRelative(u16 instr)
{
    int offset = bits<0, 8>(instr);
    int rd     = bits<8, 3>(instr);

    offset <<= 2;

    u32 addr = alignWord(pc + offset);

    regs[rd] = readWord(addr);

    idle();
}

void ARM::Thumb_LoadStoreRegisterOffset(u16 instr)
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

    switch (Operation(opcode))
    {
    case Operation::STR:
        writeWord(addr, dst);
        break;

    case Operation::STRB:
        writeByte(addr, dst);
        break;

    case Operation::LDR:
        dst = readWordRotated(addr);
        idle();
        break;

    case Operation::LDRB:
        dst = readByte(addr);
        idle();
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void ARM::Thumb_LoadStoreByteHalf(u16 instr)
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

    switch (Operation(opcode))
    {
    case Operation::STRH:
        writeHalf(addr, dst);
        break;

    case Operation::LDRSB:
        dst = readByte(addr);
        dst = signExtend<8>(dst);
        idle();
        break;

    case Operation::LDRH:
        dst = readHalfRotated(addr);
        idle();
        break;

    case Operation::LDRSH:
        dst = readHalfSigned(addr);
        idle();
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void ARM::Thumb_LoadStoreImmediateOffset(u16 instr)
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

    if (~opcode & (1 << 1))
        offset <<= 2;

    u32& dst = regs[rd];
    u32 addr = regs[rb] + offset;

    switch (Operation(opcode))
    {
    case Operation::STR:
        writeWord(addr, dst);
        break;

    case Operation::STRB:
        writeByte(addr, dst);
        break;

    case Operation::LDR:
        dst = readWordRotated(addr);
        idle();
        break;

    case Operation::LDRB:
        dst = readByte(addr);
        idle();
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

void ARM::Thumb_LoadStoreHalf(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int offset = bits< 6, 5>(instr);
    int load   = bits<11, 1>(instr);

    offset <<= 1;

    u32& dst = regs[rd];
    u32 addr = regs[rb] + offset;

    if (load)
    {
        dst = readHalfRotated(addr);
        idle();
    }
    else
    {
        writeHalf(addr, dst);
    }
}

void ARM::Thumb_LoadStoreSPRelative(u16 instr)
{
    int offset = bits< 0, 8>(instr);
    int rd     = bits< 8, 3>(instr);
    int load   = bits<11, 1>(instr);

    u32& dst = regs[rd];
    u32 addr = sp + (offset << 2);

    if (load)
    {
        dst = readWordRotated(addr);
        idle();
    }
    else
    {
        writeWord(addr, dst);
    }
}

void ARM::Thumb_LoadRelativeAddress(u16 instr)
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
}

void ARM::Thumb_AddOffsetSP(u16 instr)
{
    int offset = bits<0, 7>(instr);
    int sign   = bits<7, 1>(instr);

    offset <<= 2;

    if (sign)
        sp -= offset;
    else
        sp += offset; 
}

void ARM::Thumb_PushPopRegisters(u16 instr)
{
    int rlist   = bits< 0, 8>(instr);
    int special = bits< 8, 1>(instr);
    int pop     = bits<11, 1>(instr);
     
    if (pop)
    {
        int beg = bitScanForward(rlist);
        int end = bitScanReverse(rlist);

        for (int x = beg; x <= end; ++x)
        {
            if (~rlist & (1 << x))
                continue;

            regs[x] = readWord(sp);

            sp += 4;
        }
        idle();

        if (special)
        {
            pc = readWord(sp);
            pc = alignHalf(pc);
            flushPipeHalf();

            sp += 4;
        }
    }
    else
    {
        int beg = bitScanReverse(rlist);
        int end = bitScanForward(rlist);

        if (special)
        {
            sp -= 4;
            writeWord(sp, lr);
        }

        for (int x = beg; x >= end; --x)
        {
            if (~rlist & (1 << x))
                continue;

            sp -= 4;

            writeWord(sp, regs[x]);
        }
    }
}

void ARM::Thumb_LoadStoreMultiple(u16 instr)
{
    int rlist = bits< 0, 8>(instr);
    int rb    = bits< 8, 3>(instr);
    int load  = bits<11, 1>(instr);

    u32 addr = regs[rb];
    u32 base = regs[rb];

    bool writeback = true;

    if (rlist != 0)
    {
        int beg = bitScanForward(rlist);
        int end = bitScanReverse(rlist);

        if (load)
        {
            if (rlist & (1 << rb))
                writeback = false;

            for (int x = beg; x <= end; ++x)
            {
                if (~rlist & (1 << x))
                    continue;

                regs[x] = readWord(addr);

                addr += 4;
            }
            idle();
        }
        else
        {
            for (int x = beg; x <= end; ++x)
            {
                if (~rlist & (1 << x))
                    continue;

                if (x == rb)
                    writeWord(addr, base + 4 * bitCount(rlist));
                else
                    writeWord(addr, regs[x]);

                addr += 4;
            }
        }
    }
    else
    {
        if (load)
        {
            pc = readWord(addr);
            pc = alignHalf(pc);
            flushPipeHalf();
        }
        else
        {
            writeWord(addr, pc + 2);
        }
        addr += 0x40;
    }

    if (writeback)
        regs[rb] = addr;
}

void ARM::Thumb_ConditionalBranch(u16 instr)
{
    int condition = bits<8, 4>(instr);

    if (cpsr.check(PSR::Condition(condition)))
    {
        int offset = bits<0, 8>(instr);

        offset = signExtend<8>(offset);
        offset <<= 1;

        pc += offset;
        flushPipeHalf();
    }
}

void ARM::Thumb_SoftwareInterrupt(u16 instr)
{
    interruptSW();
}

void ARM::Thumb_UnconditionalBranch(u16 instr)
{
    int offset = bits<0, 11>(instr);

    offset = signExtend<11>(offset);
    offset <<= 1;

    pc += offset;
    flushPipeHalf();
}

void ARM::Thumb_LongBranchLink(u16 instr)
{
    int offset = bits< 0, 11>(instr);
    int second = bits<11,  1>(instr);

    if (second)
    {
        offset <<= 1;

        u32 next = (pc - 2) | 1;
        pc = lr + offset;
        lr = next;

        flushPipeHalf();
    }
    else
    {
        offset = signExtend<11>(offset);
        offset <<= 12;

        lr = pc + offset;
    }
}

void ARM::Thumb_Undefined(u16 instr)
{
    EGG_ASSERT(false, __FUNCTION__);
}
