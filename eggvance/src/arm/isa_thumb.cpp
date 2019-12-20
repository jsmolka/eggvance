#include "arm.h"

#include "common/bitutil.h"
#include "common/macros.h"
#include "common/utility.h"

template<int offset, int opcode>
void ARM::Thumb_MoveShiftedRegister(u16 instr)
{
    int rd = bits<0, 3>(instr);
    int rs = bits<3, 3>(instr);

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

template<int rn, int opcode>
void ARM::Thumb_AddSubtract(u16 instr)
{
    enum class Operation
    {
        ADD_REG = 0b00,
        SUB_REG = 0b01,
        ADD_IMM = 0b10,
        SUB_IMM = 0b11
    };

    int rd = bits<0, 3>(instr);
    int rs = bits<3, 3>(instr);

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

template<int rd, int opcode>
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

template<int opcode>
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

    int rd = bits<0, 3>(instr);
    int rs = bits<3, 3>(instr);

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
        dst = adc(dst, src, true);
        break;

    case Operation::SBC:
        dst = sbc(dst, src, true);
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

template<int hs, int hd, int opcode>
void ARM::Thumb_HighRegisterOperations(u16 instr)
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

    rs |= hs << 3;
    rd |= hd << 3;

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (Operation(opcode))
    {
    case Operation::ADD:
        if (rd == 15 && hd)
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
        if (rd == 15 && hd)
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
            instr_size = 2;
        }
        else
        {
            pc = alignWord(src);
            flushPipeWord();
            instr_size = 4;
        }
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

template<int rd>
void ARM::Thumb_LoadPCRelative(u16 instr)
{
    int offset = bits<0, 8>(instr);

    offset <<= 2;

    u32 addr = alignWord(pc + offset);

    regs[rd] = readWord(addr);

    idle();
}

template<int ro, int opcode>
void ARM::Thumb_LoadStoreRegisterOffset(u16 instr)
{
    enum class Operation
    {
        STR  = 0b00,
        STRB = 0b01,
        LDR  = 0b10,
        LDRB = 0b11
    };

    int rd = bits<0, 3>(instr);
    int rb = bits<3, 3>(instr);

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

template<int ro, int opcode>
void ARM::Thumb_LoadStoreByteHalf(u16 instr)
{
    enum class Operation
    {
        STRH  = 0b00,
        LDRSB = 0b01,
        LDRH  = 0b10,
        LDRSH = 0b11
    };

    int rd = bits<0, 3>(instr);
    int rb = bits<3, 3>(instr);

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

template<int offset, int opcode>
void ARM::Thumb_LoadStoreImmediateOffset(u16 instr)
{
    enum class Operation
    {
        STR  = 0b00,
        LDR  = 0b01,
        STRB = 0b10,
        LDRB = 0b11
    };

    int rd = bits<0, 3>(instr);
    int rb = bits<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + (offset << (~opcode & 0x2));

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

template<int offset, int load>
void ARM::Thumb_LoadStoreHalf(u16 instr)
{
    int rd = bits<0, 3>(instr);
    int rb = bits<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + (offset << 1);

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

template<int rd, int load>
void ARM::Thumb_LoadStoreSPRelative(u16 instr)
{
    int offset = bits<0, 8>(instr);

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

template<int rd, int use_sp>
void ARM::Thumb_LoadRelativeAddress(u16 instr)
{
    int offset = bits<0, 8>(instr);

    offset <<= 2;

    u32& dst = regs[rd];

    if (use_sp)
        dst = sp + offset;
    else
        dst = alignWord(pc + offset);
}

template<int sign>
void ARM::Thumb_AddOffsetSP(u16 instr)
{
    int offset = bits<0, 7>(instr);

    offset <<= 2;

    if (sign)
        sp -= offset;
    else
        sp += offset; 
}

template<int special, int pop>
void ARM::Thumb_PushPopRegisters(u16 instr)
{
    int rlist = bits< 0, 8>(instr);
     
    if (pop)
    {
        int beg = bitutil::scanForward(rlist);
        int end = bitutil::scanReverse(rlist);

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
        if (special)
        {
            sp -= 4;
            writeWord(sp, lr);
        }

        int beg = bitutil::scanReverse(rlist);
        int end = bitutil::scanForward(rlist);

        for (int x = beg; x >= end; --x)
        {
            if (~rlist & (1 << x))
                continue;

            sp -= 4;

            writeWord(sp, regs[x]);
        }
    }
}

template<int rb, int load>
void ARM::Thumb_LoadStoreMultiple(u16 instr)
{
    int rlist = bits<0, 8>(instr);

    u32 addr = regs[rb];
    u32 base = regs[rb];

    bool writeback = true;

    if (rlist != 0)
    {
        int beg = bitutil::scanForward(rlist);
        int end = bitutil::scanReverse(rlist);

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
                    writeWord(addr, base + 4 * bitutil::popcount(rlist));
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

template<int condition>
void ARM::Thumb_ConditionalBranch(u16 instr)
{
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

template<int second>
void ARM::Thumb_LongBranchLink(u16 instr)
{
    int offset = bits<0, 11>(instr);

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

std::array<void(ARM::*)(u16), 1024> ARM::instr_thumb =
{
    &ARM::Thumb_MoveShiftedRegister<0, 0>,
    &ARM::Thumb_MoveShiftedRegister<1, 0>,
    &ARM::Thumb_MoveShiftedRegister<2, 0>,
    &ARM::Thumb_MoveShiftedRegister<3, 0>,
    &ARM::Thumb_MoveShiftedRegister<4, 0>,
    &ARM::Thumb_MoveShiftedRegister<5, 0>,
    &ARM::Thumb_MoveShiftedRegister<6, 0>,
    &ARM::Thumb_MoveShiftedRegister<7, 0>,
    &ARM::Thumb_MoveShiftedRegister<8, 0>,
    &ARM::Thumb_MoveShiftedRegister<9, 0>,
    &ARM::Thumb_MoveShiftedRegister<10, 0>,
    &ARM::Thumb_MoveShiftedRegister<11, 0>,
    &ARM::Thumb_MoveShiftedRegister<12, 0>,
    &ARM::Thumb_MoveShiftedRegister<13, 0>,
    &ARM::Thumb_MoveShiftedRegister<14, 0>,
    &ARM::Thumb_MoveShiftedRegister<15, 0>,
    &ARM::Thumb_MoveShiftedRegister<16, 0>,
    &ARM::Thumb_MoveShiftedRegister<17, 0>,
    &ARM::Thumb_MoveShiftedRegister<18, 0>,
    &ARM::Thumb_MoveShiftedRegister<19, 0>,
    &ARM::Thumb_MoveShiftedRegister<20, 0>,
    &ARM::Thumb_MoveShiftedRegister<21, 0>,
    &ARM::Thumb_MoveShiftedRegister<22, 0>,
    &ARM::Thumb_MoveShiftedRegister<23, 0>,
    &ARM::Thumb_MoveShiftedRegister<24, 0>,
    &ARM::Thumb_MoveShiftedRegister<25, 0>,
    &ARM::Thumb_MoveShiftedRegister<26, 0>,
    &ARM::Thumb_MoveShiftedRegister<27, 0>,
    &ARM::Thumb_MoveShiftedRegister<28, 0>,
    &ARM::Thumb_MoveShiftedRegister<29, 0>,
    &ARM::Thumb_MoveShiftedRegister<30, 0>,
    &ARM::Thumb_MoveShiftedRegister<31, 0>,
    &ARM::Thumb_MoveShiftedRegister<0, 1>,
    &ARM::Thumb_MoveShiftedRegister<1, 1>,
    &ARM::Thumb_MoveShiftedRegister<2, 1>,
    &ARM::Thumb_MoveShiftedRegister<3, 1>,
    &ARM::Thumb_MoveShiftedRegister<4, 1>,
    &ARM::Thumb_MoveShiftedRegister<5, 1>,
    &ARM::Thumb_MoveShiftedRegister<6, 1>,
    &ARM::Thumb_MoveShiftedRegister<7, 1>,
    &ARM::Thumb_MoveShiftedRegister<8, 1>,
    &ARM::Thumb_MoveShiftedRegister<9, 1>,
    &ARM::Thumb_MoveShiftedRegister<10, 1>,
    &ARM::Thumb_MoveShiftedRegister<11, 1>,
    &ARM::Thumb_MoveShiftedRegister<12, 1>,
    &ARM::Thumb_MoveShiftedRegister<13, 1>,
    &ARM::Thumb_MoveShiftedRegister<14, 1>,
    &ARM::Thumb_MoveShiftedRegister<15, 1>,
    &ARM::Thumb_MoveShiftedRegister<16, 1>,
    &ARM::Thumb_MoveShiftedRegister<17, 1>,
    &ARM::Thumb_MoveShiftedRegister<18, 1>,
    &ARM::Thumb_MoveShiftedRegister<19, 1>,
    &ARM::Thumb_MoveShiftedRegister<20, 1>,
    &ARM::Thumb_MoveShiftedRegister<21, 1>,
    &ARM::Thumb_MoveShiftedRegister<22, 1>,
    &ARM::Thumb_MoveShiftedRegister<23, 1>,
    &ARM::Thumb_MoveShiftedRegister<24, 1>,
    &ARM::Thumb_MoveShiftedRegister<25, 1>,
    &ARM::Thumb_MoveShiftedRegister<26, 1>,
    &ARM::Thumb_MoveShiftedRegister<27, 1>,
    &ARM::Thumb_MoveShiftedRegister<28, 1>,
    &ARM::Thumb_MoveShiftedRegister<29, 1>,
    &ARM::Thumb_MoveShiftedRegister<30, 1>,
    &ARM::Thumb_MoveShiftedRegister<31, 1>,
    &ARM::Thumb_MoveShiftedRegister<0, 2>,
    &ARM::Thumb_MoveShiftedRegister<1, 2>,
    &ARM::Thumb_MoveShiftedRegister<2, 2>,
    &ARM::Thumb_MoveShiftedRegister<3, 2>,
    &ARM::Thumb_MoveShiftedRegister<4, 2>,
    &ARM::Thumb_MoveShiftedRegister<5, 2>,
    &ARM::Thumb_MoveShiftedRegister<6, 2>,
    &ARM::Thumb_MoveShiftedRegister<7, 2>,
    &ARM::Thumb_MoveShiftedRegister<8, 2>,
    &ARM::Thumb_MoveShiftedRegister<9, 2>,
    &ARM::Thumb_MoveShiftedRegister<10, 2>,
    &ARM::Thumb_MoveShiftedRegister<11, 2>,
    &ARM::Thumb_MoveShiftedRegister<12, 2>,
    &ARM::Thumb_MoveShiftedRegister<13, 2>,
    &ARM::Thumb_MoveShiftedRegister<14, 2>,
    &ARM::Thumb_MoveShiftedRegister<15, 2>,
    &ARM::Thumb_MoveShiftedRegister<16, 2>,
    &ARM::Thumb_MoveShiftedRegister<17, 2>,
    &ARM::Thumb_MoveShiftedRegister<18, 2>,
    &ARM::Thumb_MoveShiftedRegister<19, 2>,
    &ARM::Thumb_MoveShiftedRegister<20, 2>,
    &ARM::Thumb_MoveShiftedRegister<21, 2>,
    &ARM::Thumb_MoveShiftedRegister<22, 2>,
    &ARM::Thumb_MoveShiftedRegister<23, 2>,
    &ARM::Thumb_MoveShiftedRegister<24, 2>,
    &ARM::Thumb_MoveShiftedRegister<25, 2>,
    &ARM::Thumb_MoveShiftedRegister<26, 2>,
    &ARM::Thumb_MoveShiftedRegister<27, 2>,
    &ARM::Thumb_MoveShiftedRegister<28, 2>,
    &ARM::Thumb_MoveShiftedRegister<29, 2>,
    &ARM::Thumb_MoveShiftedRegister<30, 2>,
    &ARM::Thumb_MoveShiftedRegister<31, 2>,
    &ARM::Thumb_AddSubtract<0, 0>,
    &ARM::Thumb_AddSubtract<1, 0>,
    &ARM::Thumb_AddSubtract<2, 0>,
    &ARM::Thumb_AddSubtract<3, 0>,
    &ARM::Thumb_AddSubtract<4, 0>,
    &ARM::Thumb_AddSubtract<5, 0>,
    &ARM::Thumb_AddSubtract<6, 0>,
    &ARM::Thumb_AddSubtract<7, 0>,
    &ARM::Thumb_AddSubtract<0, 1>,
    &ARM::Thumb_AddSubtract<1, 1>,
    &ARM::Thumb_AddSubtract<2, 1>,
    &ARM::Thumb_AddSubtract<3, 1>,
    &ARM::Thumb_AddSubtract<4, 1>,
    &ARM::Thumb_AddSubtract<5, 1>,
    &ARM::Thumb_AddSubtract<6, 1>,
    &ARM::Thumb_AddSubtract<7, 1>,
    &ARM::Thumb_AddSubtract<0, 2>,
    &ARM::Thumb_AddSubtract<1, 2>,
    &ARM::Thumb_AddSubtract<2, 2>,
    &ARM::Thumb_AddSubtract<3, 2>,
    &ARM::Thumb_AddSubtract<4, 2>,
    &ARM::Thumb_AddSubtract<5, 2>,
    &ARM::Thumb_AddSubtract<6, 2>,
    &ARM::Thumb_AddSubtract<7, 2>,
    &ARM::Thumb_AddSubtract<0, 3>,
    &ARM::Thumb_AddSubtract<1, 3>,
    &ARM::Thumb_AddSubtract<2, 3>,
    &ARM::Thumb_AddSubtract<3, 3>,
    &ARM::Thumb_AddSubtract<4, 3>,
    &ARM::Thumb_AddSubtract<5, 3>,
    &ARM::Thumb_AddSubtract<6, 3>,
    &ARM::Thumb_AddSubtract<7, 3>,
    &ARM::Thumb_ImmediateOperations<0, 0>,
    &ARM::Thumb_ImmediateOperations<0, 0>,
    &ARM::Thumb_ImmediateOperations<0, 0>,
    &ARM::Thumb_ImmediateOperations<0, 0>,
    &ARM::Thumb_ImmediateOperations<1, 0>,
    &ARM::Thumb_ImmediateOperations<1, 0>,
    &ARM::Thumb_ImmediateOperations<1, 0>,
    &ARM::Thumb_ImmediateOperations<1, 0>,
    &ARM::Thumb_ImmediateOperations<2, 0>,
    &ARM::Thumb_ImmediateOperations<2, 0>,
    &ARM::Thumb_ImmediateOperations<2, 0>,
    &ARM::Thumb_ImmediateOperations<2, 0>,
    &ARM::Thumb_ImmediateOperations<3, 0>,
    &ARM::Thumb_ImmediateOperations<3, 0>,
    &ARM::Thumb_ImmediateOperations<3, 0>,
    &ARM::Thumb_ImmediateOperations<3, 0>,
    &ARM::Thumb_ImmediateOperations<4, 0>,
    &ARM::Thumb_ImmediateOperations<4, 0>,
    &ARM::Thumb_ImmediateOperations<4, 0>,
    &ARM::Thumb_ImmediateOperations<4, 0>,
    &ARM::Thumb_ImmediateOperations<5, 0>,
    &ARM::Thumb_ImmediateOperations<5, 0>,
    &ARM::Thumb_ImmediateOperations<5, 0>,
    &ARM::Thumb_ImmediateOperations<5, 0>,
    &ARM::Thumb_ImmediateOperations<6, 0>,
    &ARM::Thumb_ImmediateOperations<6, 0>,
    &ARM::Thumb_ImmediateOperations<6, 0>,
    &ARM::Thumb_ImmediateOperations<6, 0>,
    &ARM::Thumb_ImmediateOperations<7, 0>,
    &ARM::Thumb_ImmediateOperations<7, 0>,
    &ARM::Thumb_ImmediateOperations<7, 0>,
    &ARM::Thumb_ImmediateOperations<7, 0>,
    &ARM::Thumb_ImmediateOperations<0, 1>,
    &ARM::Thumb_ImmediateOperations<0, 1>,
    &ARM::Thumb_ImmediateOperations<0, 1>,
    &ARM::Thumb_ImmediateOperations<0, 1>,
    &ARM::Thumb_ImmediateOperations<1, 1>,
    &ARM::Thumb_ImmediateOperations<1, 1>,
    &ARM::Thumb_ImmediateOperations<1, 1>,
    &ARM::Thumb_ImmediateOperations<1, 1>,
    &ARM::Thumb_ImmediateOperations<2, 1>,
    &ARM::Thumb_ImmediateOperations<2, 1>,
    &ARM::Thumb_ImmediateOperations<2, 1>,
    &ARM::Thumb_ImmediateOperations<2, 1>,
    &ARM::Thumb_ImmediateOperations<3, 1>,
    &ARM::Thumb_ImmediateOperations<3, 1>,
    &ARM::Thumb_ImmediateOperations<3, 1>,
    &ARM::Thumb_ImmediateOperations<3, 1>,
    &ARM::Thumb_ImmediateOperations<4, 1>,
    &ARM::Thumb_ImmediateOperations<4, 1>,
    &ARM::Thumb_ImmediateOperations<4, 1>,
    &ARM::Thumb_ImmediateOperations<4, 1>,
    &ARM::Thumb_ImmediateOperations<5, 1>,
    &ARM::Thumb_ImmediateOperations<5, 1>,
    &ARM::Thumb_ImmediateOperations<5, 1>,
    &ARM::Thumb_ImmediateOperations<5, 1>,
    &ARM::Thumb_ImmediateOperations<6, 1>,
    &ARM::Thumb_ImmediateOperations<6, 1>,
    &ARM::Thumb_ImmediateOperations<6, 1>,
    &ARM::Thumb_ImmediateOperations<6, 1>,
    &ARM::Thumb_ImmediateOperations<7, 1>,
    &ARM::Thumb_ImmediateOperations<7, 1>,
    &ARM::Thumb_ImmediateOperations<7, 1>,
    &ARM::Thumb_ImmediateOperations<7, 1>,
    &ARM::Thumb_ImmediateOperations<0, 2>,
    &ARM::Thumb_ImmediateOperations<0, 2>,
    &ARM::Thumb_ImmediateOperations<0, 2>,
    &ARM::Thumb_ImmediateOperations<0, 2>,
    &ARM::Thumb_ImmediateOperations<1, 2>,
    &ARM::Thumb_ImmediateOperations<1, 2>,
    &ARM::Thumb_ImmediateOperations<1, 2>,
    &ARM::Thumb_ImmediateOperations<1, 2>,
    &ARM::Thumb_ImmediateOperations<2, 2>,
    &ARM::Thumb_ImmediateOperations<2, 2>,
    &ARM::Thumb_ImmediateOperations<2, 2>,
    &ARM::Thumb_ImmediateOperations<2, 2>,
    &ARM::Thumb_ImmediateOperations<3, 2>,
    &ARM::Thumb_ImmediateOperations<3, 2>,
    &ARM::Thumb_ImmediateOperations<3, 2>,
    &ARM::Thumb_ImmediateOperations<3, 2>,
    &ARM::Thumb_ImmediateOperations<4, 2>,
    &ARM::Thumb_ImmediateOperations<4, 2>,
    &ARM::Thumb_ImmediateOperations<4, 2>,
    &ARM::Thumb_ImmediateOperations<4, 2>,
    &ARM::Thumb_ImmediateOperations<5, 2>,
    &ARM::Thumb_ImmediateOperations<5, 2>,
    &ARM::Thumb_ImmediateOperations<5, 2>,
    &ARM::Thumb_ImmediateOperations<5, 2>,
    &ARM::Thumb_ImmediateOperations<6, 2>,
    &ARM::Thumb_ImmediateOperations<6, 2>,
    &ARM::Thumb_ImmediateOperations<6, 2>,
    &ARM::Thumb_ImmediateOperations<6, 2>,
    &ARM::Thumb_ImmediateOperations<7, 2>,
    &ARM::Thumb_ImmediateOperations<7, 2>,
    &ARM::Thumb_ImmediateOperations<7, 2>,
    &ARM::Thumb_ImmediateOperations<7, 2>,
    &ARM::Thumb_ImmediateOperations<0, 3>,
    &ARM::Thumb_ImmediateOperations<0, 3>,
    &ARM::Thumb_ImmediateOperations<0, 3>,
    &ARM::Thumb_ImmediateOperations<0, 3>,
    &ARM::Thumb_ImmediateOperations<1, 3>,
    &ARM::Thumb_ImmediateOperations<1, 3>,
    &ARM::Thumb_ImmediateOperations<1, 3>,
    &ARM::Thumb_ImmediateOperations<1, 3>,
    &ARM::Thumb_ImmediateOperations<2, 3>,
    &ARM::Thumb_ImmediateOperations<2, 3>,
    &ARM::Thumb_ImmediateOperations<2, 3>,
    &ARM::Thumb_ImmediateOperations<2, 3>,
    &ARM::Thumb_ImmediateOperations<3, 3>,
    &ARM::Thumb_ImmediateOperations<3, 3>,
    &ARM::Thumb_ImmediateOperations<3, 3>,
    &ARM::Thumb_ImmediateOperations<3, 3>,
    &ARM::Thumb_ImmediateOperations<4, 3>,
    &ARM::Thumb_ImmediateOperations<4, 3>,
    &ARM::Thumb_ImmediateOperations<4, 3>,
    &ARM::Thumb_ImmediateOperations<4, 3>,
    &ARM::Thumb_ImmediateOperations<5, 3>,
    &ARM::Thumb_ImmediateOperations<5, 3>,
    &ARM::Thumb_ImmediateOperations<5, 3>,
    &ARM::Thumb_ImmediateOperations<5, 3>,
    &ARM::Thumb_ImmediateOperations<6, 3>,
    &ARM::Thumb_ImmediateOperations<6, 3>,
    &ARM::Thumb_ImmediateOperations<6, 3>,
    &ARM::Thumb_ImmediateOperations<6, 3>,
    &ARM::Thumb_ImmediateOperations<7, 3>,
    &ARM::Thumb_ImmediateOperations<7, 3>,
    &ARM::Thumb_ImmediateOperations<7, 3>,
    &ARM::Thumb_ImmediateOperations<7, 3>,
    &ARM::Thumb_ALUOperations<0>,
    &ARM::Thumb_ALUOperations<1>,
    &ARM::Thumb_ALUOperations<2>,
    &ARM::Thumb_ALUOperations<3>,
    &ARM::Thumb_ALUOperations<4>,
    &ARM::Thumb_ALUOperations<5>,
    &ARM::Thumb_ALUOperations<6>,
    &ARM::Thumb_ALUOperations<7>,
    &ARM::Thumb_ALUOperations<8>,
    &ARM::Thumb_ALUOperations<9>,
    &ARM::Thumb_ALUOperations<10>,
    &ARM::Thumb_ALUOperations<11>,
    &ARM::Thumb_ALUOperations<12>,
    &ARM::Thumb_ALUOperations<13>,
    &ARM::Thumb_ALUOperations<14>,
    &ARM::Thumb_ALUOperations<15>,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_HighRegisterOperations<1, 0, 0>,
    &ARM::Thumb_HighRegisterOperations<0, 1, 0>,
    &ARM::Thumb_HighRegisterOperations<1, 1, 0>,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_HighRegisterOperations<1, 0, 1>,
    &ARM::Thumb_HighRegisterOperations<0, 1, 1>,
    &ARM::Thumb_HighRegisterOperations<1, 1, 1>,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_HighRegisterOperations<1, 0, 2>,
    &ARM::Thumb_HighRegisterOperations<0, 1, 2>,
    &ARM::Thumb_HighRegisterOperations<1, 1, 2>,
    &ARM::Thumb_HighRegisterOperations<0, 0, 3>,
    &ARM::Thumb_HighRegisterOperations<1, 0, 3>,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_LoadPCRelative<0>,
    &ARM::Thumb_LoadPCRelative<0>,
    &ARM::Thumb_LoadPCRelative<0>,
    &ARM::Thumb_LoadPCRelative<0>,
    &ARM::Thumb_LoadPCRelative<1>,
    &ARM::Thumb_LoadPCRelative<1>,
    &ARM::Thumb_LoadPCRelative<1>,
    &ARM::Thumb_LoadPCRelative<1>,
    &ARM::Thumb_LoadPCRelative<2>,
    &ARM::Thumb_LoadPCRelative<2>,
    &ARM::Thumb_LoadPCRelative<2>,
    &ARM::Thumb_LoadPCRelative<2>,
    &ARM::Thumb_LoadPCRelative<3>,
    &ARM::Thumb_LoadPCRelative<3>,
    &ARM::Thumb_LoadPCRelative<3>,
    &ARM::Thumb_LoadPCRelative<3>,
    &ARM::Thumb_LoadPCRelative<4>,
    &ARM::Thumb_LoadPCRelative<4>,
    &ARM::Thumb_LoadPCRelative<4>,
    &ARM::Thumb_LoadPCRelative<4>,
    &ARM::Thumb_LoadPCRelative<5>,
    &ARM::Thumb_LoadPCRelative<5>,
    &ARM::Thumb_LoadPCRelative<5>,
    &ARM::Thumb_LoadPCRelative<5>,
    &ARM::Thumb_LoadPCRelative<6>,
    &ARM::Thumb_LoadPCRelative<6>,
    &ARM::Thumb_LoadPCRelative<6>,
    &ARM::Thumb_LoadPCRelative<6>,
    &ARM::Thumb_LoadPCRelative<7>,
    &ARM::Thumb_LoadPCRelative<7>,
    &ARM::Thumb_LoadPCRelative<7>,
    &ARM::Thumb_LoadPCRelative<7>,
    &ARM::Thumb_LoadStoreRegisterOffset<0, 0>,
    &ARM::Thumb_LoadStoreRegisterOffset<1, 0>,
    &ARM::Thumb_LoadStoreRegisterOffset<2, 0>,
    &ARM::Thumb_LoadStoreRegisterOffset<3, 0>,
    &ARM::Thumb_LoadStoreRegisterOffset<4, 0>,
    &ARM::Thumb_LoadStoreRegisterOffset<5, 0>,
    &ARM::Thumb_LoadStoreRegisterOffset<6, 0>,
    &ARM::Thumb_LoadStoreRegisterOffset<7, 0>,
    &ARM::Thumb_LoadStoreByteHalf<0, 0>,
    &ARM::Thumb_LoadStoreByteHalf<1, 0>,
    &ARM::Thumb_LoadStoreByteHalf<2, 0>,
    &ARM::Thumb_LoadStoreByteHalf<3, 0>,
    &ARM::Thumb_LoadStoreByteHalf<4, 0>,
    &ARM::Thumb_LoadStoreByteHalf<5, 0>,
    &ARM::Thumb_LoadStoreByteHalf<6, 0>,
    &ARM::Thumb_LoadStoreByteHalf<7, 0>,
    &ARM::Thumb_LoadStoreRegisterOffset<0, 1>,
    &ARM::Thumb_LoadStoreRegisterOffset<1, 1>,
    &ARM::Thumb_LoadStoreRegisterOffset<2, 1>,
    &ARM::Thumb_LoadStoreRegisterOffset<3, 1>,
    &ARM::Thumb_LoadStoreRegisterOffset<4, 1>,
    &ARM::Thumb_LoadStoreRegisterOffset<5, 1>,
    &ARM::Thumb_LoadStoreRegisterOffset<6, 1>,
    &ARM::Thumb_LoadStoreRegisterOffset<7, 1>,
    &ARM::Thumb_LoadStoreByteHalf<0, 1>,
    &ARM::Thumb_LoadStoreByteHalf<1, 1>,
    &ARM::Thumb_LoadStoreByteHalf<2, 1>,
    &ARM::Thumb_LoadStoreByteHalf<3, 1>,
    &ARM::Thumb_LoadStoreByteHalf<4, 1>,
    &ARM::Thumb_LoadStoreByteHalf<5, 1>,
    &ARM::Thumb_LoadStoreByteHalf<6, 1>,
    &ARM::Thumb_LoadStoreByteHalf<7, 1>,
    &ARM::Thumb_LoadStoreRegisterOffset<0, 2>,
    &ARM::Thumb_LoadStoreRegisterOffset<1, 2>,
    &ARM::Thumb_LoadStoreRegisterOffset<2, 2>,
    &ARM::Thumb_LoadStoreRegisterOffset<3, 2>,
    &ARM::Thumb_LoadStoreRegisterOffset<4, 2>,
    &ARM::Thumb_LoadStoreRegisterOffset<5, 2>,
    &ARM::Thumb_LoadStoreRegisterOffset<6, 2>,
    &ARM::Thumb_LoadStoreRegisterOffset<7, 2>,
    &ARM::Thumb_LoadStoreByteHalf<0, 2>,
    &ARM::Thumb_LoadStoreByteHalf<1, 2>,
    &ARM::Thumb_LoadStoreByteHalf<2, 2>,
    &ARM::Thumb_LoadStoreByteHalf<3, 2>,
    &ARM::Thumb_LoadStoreByteHalf<4, 2>,
    &ARM::Thumb_LoadStoreByteHalf<5, 2>,
    &ARM::Thumb_LoadStoreByteHalf<6, 2>,
    &ARM::Thumb_LoadStoreByteHalf<7, 2>,
    &ARM::Thumb_LoadStoreRegisterOffset<0, 3>,
    &ARM::Thumb_LoadStoreRegisterOffset<1, 3>,
    &ARM::Thumb_LoadStoreRegisterOffset<2, 3>,
    &ARM::Thumb_LoadStoreRegisterOffset<3, 3>,
    &ARM::Thumb_LoadStoreRegisterOffset<4, 3>,
    &ARM::Thumb_LoadStoreRegisterOffset<5, 3>,
    &ARM::Thumb_LoadStoreRegisterOffset<6, 3>,
    &ARM::Thumb_LoadStoreRegisterOffset<7, 3>,
    &ARM::Thumb_LoadStoreByteHalf<0, 3>,
    &ARM::Thumb_LoadStoreByteHalf<1, 3>,
    &ARM::Thumb_LoadStoreByteHalf<2, 3>,
    &ARM::Thumb_LoadStoreByteHalf<3, 3>,
    &ARM::Thumb_LoadStoreByteHalf<4, 3>,
    &ARM::Thumb_LoadStoreByteHalf<5, 3>,
    &ARM::Thumb_LoadStoreByteHalf<6, 3>,
    &ARM::Thumb_LoadStoreByteHalf<7, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<0, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<1, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<2, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<3, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<4, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<5, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<6, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<7, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<8, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<9, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<10, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<11, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<12, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<13, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<14, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<15, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<16, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<17, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<18, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<19, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<20, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<21, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<22, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<23, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<24, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<25, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<26, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<27, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<28, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<29, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<30, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<31, 0>,
    &ARM::Thumb_LoadStoreImmediateOffset<0, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<1, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<2, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<3, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<4, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<5, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<6, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<7, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<8, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<9, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<10, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<11, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<12, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<13, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<14, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<15, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<16, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<17, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<18, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<19, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<20, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<21, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<22, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<23, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<24, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<25, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<26, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<27, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<28, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<29, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<30, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<31, 1>,
    &ARM::Thumb_LoadStoreImmediateOffset<0, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<1, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<2, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<3, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<4, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<5, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<6, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<7, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<8, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<9, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<10, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<11, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<12, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<13, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<14, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<15, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<16, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<17, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<18, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<19, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<20, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<21, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<22, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<23, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<24, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<25, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<26, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<27, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<28, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<29, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<30, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<31, 2>,
    &ARM::Thumb_LoadStoreImmediateOffset<0, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<1, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<2, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<3, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<4, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<5, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<6, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<7, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<8, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<9, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<10, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<11, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<12, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<13, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<14, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<15, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<16, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<17, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<18, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<19, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<20, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<21, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<22, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<23, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<24, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<25, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<26, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<27, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<28, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<29, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<30, 3>,
    &ARM::Thumb_LoadStoreImmediateOffset<31, 3>,
    &ARM::Thumb_LoadStoreHalf<0, 0>,
    &ARM::Thumb_LoadStoreHalf<1, 0>,
    &ARM::Thumb_LoadStoreHalf<2, 0>,
    &ARM::Thumb_LoadStoreHalf<3, 0>,
    &ARM::Thumb_LoadStoreHalf<4, 0>,
    &ARM::Thumb_LoadStoreHalf<5, 0>,
    &ARM::Thumb_LoadStoreHalf<6, 0>,
    &ARM::Thumb_LoadStoreHalf<7, 0>,
    &ARM::Thumb_LoadStoreHalf<8, 0>,
    &ARM::Thumb_LoadStoreHalf<9, 0>,
    &ARM::Thumb_LoadStoreHalf<10, 0>,
    &ARM::Thumb_LoadStoreHalf<11, 0>,
    &ARM::Thumb_LoadStoreHalf<12, 0>,
    &ARM::Thumb_LoadStoreHalf<13, 0>,
    &ARM::Thumb_LoadStoreHalf<14, 0>,
    &ARM::Thumb_LoadStoreHalf<15, 0>,
    &ARM::Thumb_LoadStoreHalf<16, 0>,
    &ARM::Thumb_LoadStoreHalf<17, 0>,
    &ARM::Thumb_LoadStoreHalf<18, 0>,
    &ARM::Thumb_LoadStoreHalf<19, 0>,
    &ARM::Thumb_LoadStoreHalf<20, 0>,
    &ARM::Thumb_LoadStoreHalf<21, 0>,
    &ARM::Thumb_LoadStoreHalf<22, 0>,
    &ARM::Thumb_LoadStoreHalf<23, 0>,
    &ARM::Thumb_LoadStoreHalf<24, 0>,
    &ARM::Thumb_LoadStoreHalf<25, 0>,
    &ARM::Thumb_LoadStoreHalf<26, 0>,
    &ARM::Thumb_LoadStoreHalf<27, 0>,
    &ARM::Thumb_LoadStoreHalf<28, 0>,
    &ARM::Thumb_LoadStoreHalf<29, 0>,
    &ARM::Thumb_LoadStoreHalf<30, 0>,
    &ARM::Thumb_LoadStoreHalf<31, 0>,
    &ARM::Thumb_LoadStoreHalf<0, 1>,
    &ARM::Thumb_LoadStoreHalf<1, 1>,
    &ARM::Thumb_LoadStoreHalf<2, 1>,
    &ARM::Thumb_LoadStoreHalf<3, 1>,
    &ARM::Thumb_LoadStoreHalf<4, 1>,
    &ARM::Thumb_LoadStoreHalf<5, 1>,
    &ARM::Thumb_LoadStoreHalf<6, 1>,
    &ARM::Thumb_LoadStoreHalf<7, 1>,
    &ARM::Thumb_LoadStoreHalf<8, 1>,
    &ARM::Thumb_LoadStoreHalf<9, 1>,
    &ARM::Thumb_LoadStoreHalf<10, 1>,
    &ARM::Thumb_LoadStoreHalf<11, 1>,
    &ARM::Thumb_LoadStoreHalf<12, 1>,
    &ARM::Thumb_LoadStoreHalf<13, 1>,
    &ARM::Thumb_LoadStoreHalf<14, 1>,
    &ARM::Thumb_LoadStoreHalf<15, 1>,
    &ARM::Thumb_LoadStoreHalf<16, 1>,
    &ARM::Thumb_LoadStoreHalf<17, 1>,
    &ARM::Thumb_LoadStoreHalf<18, 1>,
    &ARM::Thumb_LoadStoreHalf<19, 1>,
    &ARM::Thumb_LoadStoreHalf<20, 1>,
    &ARM::Thumb_LoadStoreHalf<21, 1>,
    &ARM::Thumb_LoadStoreHalf<22, 1>,
    &ARM::Thumb_LoadStoreHalf<23, 1>,
    &ARM::Thumb_LoadStoreHalf<24, 1>,
    &ARM::Thumb_LoadStoreHalf<25, 1>,
    &ARM::Thumb_LoadStoreHalf<26, 1>,
    &ARM::Thumb_LoadStoreHalf<27, 1>,
    &ARM::Thumb_LoadStoreHalf<28, 1>,
    &ARM::Thumb_LoadStoreHalf<29, 1>,
    &ARM::Thumb_LoadStoreHalf<30, 1>,
    &ARM::Thumb_LoadStoreHalf<31, 1>,
    &ARM::Thumb_LoadStoreSPRelative<0, 0>,
    &ARM::Thumb_LoadStoreSPRelative<0, 0>,
    &ARM::Thumb_LoadStoreSPRelative<0, 0>,
    &ARM::Thumb_LoadStoreSPRelative<0, 0>,
    &ARM::Thumb_LoadStoreSPRelative<1, 0>,
    &ARM::Thumb_LoadStoreSPRelative<1, 0>,
    &ARM::Thumb_LoadStoreSPRelative<1, 0>,
    &ARM::Thumb_LoadStoreSPRelative<1, 0>,
    &ARM::Thumb_LoadStoreSPRelative<2, 0>,
    &ARM::Thumb_LoadStoreSPRelative<2, 0>,
    &ARM::Thumb_LoadStoreSPRelative<2, 0>,
    &ARM::Thumb_LoadStoreSPRelative<2, 0>,
    &ARM::Thumb_LoadStoreSPRelative<3, 0>,
    &ARM::Thumb_LoadStoreSPRelative<3, 0>,
    &ARM::Thumb_LoadStoreSPRelative<3, 0>,
    &ARM::Thumb_LoadStoreSPRelative<3, 0>,
    &ARM::Thumb_LoadStoreSPRelative<4, 0>,
    &ARM::Thumb_LoadStoreSPRelative<4, 0>,
    &ARM::Thumb_LoadStoreSPRelative<4, 0>,
    &ARM::Thumb_LoadStoreSPRelative<4, 0>,
    &ARM::Thumb_LoadStoreSPRelative<5, 0>,
    &ARM::Thumb_LoadStoreSPRelative<5, 0>,
    &ARM::Thumb_LoadStoreSPRelative<5, 0>,
    &ARM::Thumb_LoadStoreSPRelative<5, 0>,
    &ARM::Thumb_LoadStoreSPRelative<6, 0>,
    &ARM::Thumb_LoadStoreSPRelative<6, 0>,
    &ARM::Thumb_LoadStoreSPRelative<6, 0>,
    &ARM::Thumb_LoadStoreSPRelative<6, 0>,
    &ARM::Thumb_LoadStoreSPRelative<7, 0>,
    &ARM::Thumb_LoadStoreSPRelative<7, 0>,
    &ARM::Thumb_LoadStoreSPRelative<7, 0>,
    &ARM::Thumb_LoadStoreSPRelative<7, 0>,
    &ARM::Thumb_LoadStoreSPRelative<0, 1>,
    &ARM::Thumb_LoadStoreSPRelative<0, 1>,
    &ARM::Thumb_LoadStoreSPRelative<0, 1>,
    &ARM::Thumb_LoadStoreSPRelative<0, 1>,
    &ARM::Thumb_LoadStoreSPRelative<1, 1>,
    &ARM::Thumb_LoadStoreSPRelative<1, 1>,
    &ARM::Thumb_LoadStoreSPRelative<1, 1>,
    &ARM::Thumb_LoadStoreSPRelative<1, 1>,
    &ARM::Thumb_LoadStoreSPRelative<2, 1>,
    &ARM::Thumb_LoadStoreSPRelative<2, 1>,
    &ARM::Thumb_LoadStoreSPRelative<2, 1>,
    &ARM::Thumb_LoadStoreSPRelative<2, 1>,
    &ARM::Thumb_LoadStoreSPRelative<3, 1>,
    &ARM::Thumb_LoadStoreSPRelative<3, 1>,
    &ARM::Thumb_LoadStoreSPRelative<3, 1>,
    &ARM::Thumb_LoadStoreSPRelative<3, 1>,
    &ARM::Thumb_LoadStoreSPRelative<4, 1>,
    &ARM::Thumb_LoadStoreSPRelative<4, 1>,
    &ARM::Thumb_LoadStoreSPRelative<4, 1>,
    &ARM::Thumb_LoadStoreSPRelative<4, 1>,
    &ARM::Thumb_LoadStoreSPRelative<5, 1>,
    &ARM::Thumb_LoadStoreSPRelative<5, 1>,
    &ARM::Thumb_LoadStoreSPRelative<5, 1>,
    &ARM::Thumb_LoadStoreSPRelative<5, 1>,
    &ARM::Thumb_LoadStoreSPRelative<6, 1>,
    &ARM::Thumb_LoadStoreSPRelative<6, 1>,
    &ARM::Thumb_LoadStoreSPRelative<6, 1>,
    &ARM::Thumb_LoadStoreSPRelative<6, 1>,
    &ARM::Thumb_LoadStoreSPRelative<7, 1>,
    &ARM::Thumb_LoadStoreSPRelative<7, 1>,
    &ARM::Thumb_LoadStoreSPRelative<7, 1>,
    &ARM::Thumb_LoadStoreSPRelative<7, 1>,
    &ARM::Thumb_LoadRelativeAddress<0, 0>,
    &ARM::Thumb_LoadRelativeAddress<0, 0>,
    &ARM::Thumb_LoadRelativeAddress<0, 0>,
    &ARM::Thumb_LoadRelativeAddress<0, 0>,
    &ARM::Thumb_LoadRelativeAddress<1, 0>,
    &ARM::Thumb_LoadRelativeAddress<1, 0>,
    &ARM::Thumb_LoadRelativeAddress<1, 0>,
    &ARM::Thumb_LoadRelativeAddress<1, 0>,
    &ARM::Thumb_LoadRelativeAddress<2, 0>,
    &ARM::Thumb_LoadRelativeAddress<2, 0>,
    &ARM::Thumb_LoadRelativeAddress<2, 0>,
    &ARM::Thumb_LoadRelativeAddress<2, 0>,
    &ARM::Thumb_LoadRelativeAddress<3, 0>,
    &ARM::Thumb_LoadRelativeAddress<3, 0>,
    &ARM::Thumb_LoadRelativeAddress<3, 0>,
    &ARM::Thumb_LoadRelativeAddress<3, 0>,
    &ARM::Thumb_LoadRelativeAddress<4, 0>,
    &ARM::Thumb_LoadRelativeAddress<4, 0>,
    &ARM::Thumb_LoadRelativeAddress<4, 0>,
    &ARM::Thumb_LoadRelativeAddress<4, 0>,
    &ARM::Thumb_LoadRelativeAddress<5, 0>,
    &ARM::Thumb_LoadRelativeAddress<5, 0>,
    &ARM::Thumb_LoadRelativeAddress<5, 0>,
    &ARM::Thumb_LoadRelativeAddress<5, 0>,
    &ARM::Thumb_LoadRelativeAddress<6, 0>,
    &ARM::Thumb_LoadRelativeAddress<6, 0>,
    &ARM::Thumb_LoadRelativeAddress<6, 0>,
    &ARM::Thumb_LoadRelativeAddress<6, 0>,
    &ARM::Thumb_LoadRelativeAddress<7, 0>,
    &ARM::Thumb_LoadRelativeAddress<7, 0>,
    &ARM::Thumb_LoadRelativeAddress<7, 0>,
    &ARM::Thumb_LoadRelativeAddress<7, 0>,
    &ARM::Thumb_LoadRelativeAddress<0, 1>,
    &ARM::Thumb_LoadRelativeAddress<0, 1>,
    &ARM::Thumb_LoadRelativeAddress<0, 1>,
    &ARM::Thumb_LoadRelativeAddress<0, 1>,
    &ARM::Thumb_LoadRelativeAddress<1, 1>,
    &ARM::Thumb_LoadRelativeAddress<1, 1>,
    &ARM::Thumb_LoadRelativeAddress<1, 1>,
    &ARM::Thumb_LoadRelativeAddress<1, 1>,
    &ARM::Thumb_LoadRelativeAddress<2, 1>,
    &ARM::Thumb_LoadRelativeAddress<2, 1>,
    &ARM::Thumb_LoadRelativeAddress<2, 1>,
    &ARM::Thumb_LoadRelativeAddress<2, 1>,
    &ARM::Thumb_LoadRelativeAddress<3, 1>,
    &ARM::Thumb_LoadRelativeAddress<3, 1>,
    &ARM::Thumb_LoadRelativeAddress<3, 1>,
    &ARM::Thumb_LoadRelativeAddress<3, 1>,
    &ARM::Thumb_LoadRelativeAddress<4, 1>,
    &ARM::Thumb_LoadRelativeAddress<4, 1>,
    &ARM::Thumb_LoadRelativeAddress<4, 1>,
    &ARM::Thumb_LoadRelativeAddress<4, 1>,
    &ARM::Thumb_LoadRelativeAddress<5, 1>,
    &ARM::Thumb_LoadRelativeAddress<5, 1>,
    &ARM::Thumb_LoadRelativeAddress<5, 1>,
    &ARM::Thumb_LoadRelativeAddress<5, 1>,
    &ARM::Thumb_LoadRelativeAddress<6, 1>,
    &ARM::Thumb_LoadRelativeAddress<6, 1>,
    &ARM::Thumb_LoadRelativeAddress<6, 1>,
    &ARM::Thumb_LoadRelativeAddress<6, 1>,
    &ARM::Thumb_LoadRelativeAddress<7, 1>,
    &ARM::Thumb_LoadRelativeAddress<7, 1>,
    &ARM::Thumb_LoadRelativeAddress<7, 1>,
    &ARM::Thumb_LoadRelativeAddress<7, 1>,
    &ARM::Thumb_AddOffsetSP<0>,
    &ARM::Thumb_AddOffsetSP<0>,
    &ARM::Thumb_AddOffsetSP<1>,
    &ARM::Thumb_AddOffsetSP<1>,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_PushPopRegisters<0, 0>,
    &ARM::Thumb_PushPopRegisters<0, 0>,
    &ARM::Thumb_PushPopRegisters<0, 0>,
    &ARM::Thumb_PushPopRegisters<0, 0>,
    &ARM::Thumb_PushPopRegisters<1, 0>,
    &ARM::Thumb_PushPopRegisters<1, 0>,
    &ARM::Thumb_PushPopRegisters<1, 0>,
    &ARM::Thumb_PushPopRegisters<1, 0>,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_PushPopRegisters<0, 1>,
    &ARM::Thumb_PushPopRegisters<0, 1>,
    &ARM::Thumb_PushPopRegisters<0, 1>,
    &ARM::Thumb_PushPopRegisters<0, 1>,
    &ARM::Thumb_PushPopRegisters<1, 1>,
    &ARM::Thumb_PushPopRegisters<1, 1>,
    &ARM::Thumb_PushPopRegisters<1, 1>,
    &ARM::Thumb_PushPopRegisters<1, 1>,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_LoadStoreMultiple<0, 0>,
    &ARM::Thumb_LoadStoreMultiple<0, 0>,
    &ARM::Thumb_LoadStoreMultiple<0, 0>,
    &ARM::Thumb_LoadStoreMultiple<0, 0>,
    &ARM::Thumb_LoadStoreMultiple<1, 0>,
    &ARM::Thumb_LoadStoreMultiple<1, 0>,
    &ARM::Thumb_LoadStoreMultiple<1, 0>,
    &ARM::Thumb_LoadStoreMultiple<1, 0>,
    &ARM::Thumb_LoadStoreMultiple<2, 0>,
    &ARM::Thumb_LoadStoreMultiple<2, 0>,
    &ARM::Thumb_LoadStoreMultiple<2, 0>,
    &ARM::Thumb_LoadStoreMultiple<2, 0>,
    &ARM::Thumb_LoadStoreMultiple<3, 0>,
    &ARM::Thumb_LoadStoreMultiple<3, 0>,
    &ARM::Thumb_LoadStoreMultiple<3, 0>,
    &ARM::Thumb_LoadStoreMultiple<3, 0>,
    &ARM::Thumb_LoadStoreMultiple<4, 0>,
    &ARM::Thumb_LoadStoreMultiple<4, 0>,
    &ARM::Thumb_LoadStoreMultiple<4, 0>,
    &ARM::Thumb_LoadStoreMultiple<4, 0>,
    &ARM::Thumb_LoadStoreMultiple<5, 0>,
    &ARM::Thumb_LoadStoreMultiple<5, 0>,
    &ARM::Thumb_LoadStoreMultiple<5, 0>,
    &ARM::Thumb_LoadStoreMultiple<5, 0>,
    &ARM::Thumb_LoadStoreMultiple<6, 0>,
    &ARM::Thumb_LoadStoreMultiple<6, 0>,
    &ARM::Thumb_LoadStoreMultiple<6, 0>,
    &ARM::Thumb_LoadStoreMultiple<6, 0>,
    &ARM::Thumb_LoadStoreMultiple<7, 0>,
    &ARM::Thumb_LoadStoreMultiple<7, 0>,
    &ARM::Thumb_LoadStoreMultiple<7, 0>,
    &ARM::Thumb_LoadStoreMultiple<7, 0>,
    &ARM::Thumb_LoadStoreMultiple<0, 1>,
    &ARM::Thumb_LoadStoreMultiple<0, 1>,
    &ARM::Thumb_LoadStoreMultiple<0, 1>,
    &ARM::Thumb_LoadStoreMultiple<0, 1>,
    &ARM::Thumb_LoadStoreMultiple<1, 1>,
    &ARM::Thumb_LoadStoreMultiple<1, 1>,
    &ARM::Thumb_LoadStoreMultiple<1, 1>,
    &ARM::Thumb_LoadStoreMultiple<1, 1>,
    &ARM::Thumb_LoadStoreMultiple<2, 1>,
    &ARM::Thumb_LoadStoreMultiple<2, 1>,
    &ARM::Thumb_LoadStoreMultiple<2, 1>,
    &ARM::Thumb_LoadStoreMultiple<2, 1>,
    &ARM::Thumb_LoadStoreMultiple<3, 1>,
    &ARM::Thumb_LoadStoreMultiple<3, 1>,
    &ARM::Thumb_LoadStoreMultiple<3, 1>,
    &ARM::Thumb_LoadStoreMultiple<3, 1>,
    &ARM::Thumb_LoadStoreMultiple<4, 1>,
    &ARM::Thumb_LoadStoreMultiple<4, 1>,
    &ARM::Thumb_LoadStoreMultiple<4, 1>,
    &ARM::Thumb_LoadStoreMultiple<4, 1>,
    &ARM::Thumb_LoadStoreMultiple<5, 1>,
    &ARM::Thumb_LoadStoreMultiple<5, 1>,
    &ARM::Thumb_LoadStoreMultiple<5, 1>,
    &ARM::Thumb_LoadStoreMultiple<5, 1>,
    &ARM::Thumb_LoadStoreMultiple<6, 1>,
    &ARM::Thumb_LoadStoreMultiple<6, 1>,
    &ARM::Thumb_LoadStoreMultiple<6, 1>,
    &ARM::Thumb_LoadStoreMultiple<6, 1>,
    &ARM::Thumb_LoadStoreMultiple<7, 1>,
    &ARM::Thumb_LoadStoreMultiple<7, 1>,
    &ARM::Thumb_LoadStoreMultiple<7, 1>,
    &ARM::Thumb_LoadStoreMultiple<7, 1>,
    &ARM::Thumb_ConditionalBranch<0>,
    &ARM::Thumb_ConditionalBranch<0>,
    &ARM::Thumb_ConditionalBranch<0>,
    &ARM::Thumb_ConditionalBranch<0>,
    &ARM::Thumb_ConditionalBranch<1>,
    &ARM::Thumb_ConditionalBranch<1>,
    &ARM::Thumb_ConditionalBranch<1>,
    &ARM::Thumb_ConditionalBranch<1>,
    &ARM::Thumb_ConditionalBranch<2>,
    &ARM::Thumb_ConditionalBranch<2>,
    &ARM::Thumb_ConditionalBranch<2>,
    &ARM::Thumb_ConditionalBranch<2>,
    &ARM::Thumb_ConditionalBranch<3>,
    &ARM::Thumb_ConditionalBranch<3>,
    &ARM::Thumb_ConditionalBranch<3>,
    &ARM::Thumb_ConditionalBranch<3>,
    &ARM::Thumb_ConditionalBranch<4>,
    &ARM::Thumb_ConditionalBranch<4>,
    &ARM::Thumb_ConditionalBranch<4>,
    &ARM::Thumb_ConditionalBranch<4>,
    &ARM::Thumb_ConditionalBranch<5>,
    &ARM::Thumb_ConditionalBranch<5>,
    &ARM::Thumb_ConditionalBranch<5>,
    &ARM::Thumb_ConditionalBranch<5>,
    &ARM::Thumb_ConditionalBranch<6>,
    &ARM::Thumb_ConditionalBranch<6>,
    &ARM::Thumb_ConditionalBranch<6>,
    &ARM::Thumb_ConditionalBranch<6>,
    &ARM::Thumb_ConditionalBranch<7>,
    &ARM::Thumb_ConditionalBranch<7>,
    &ARM::Thumb_ConditionalBranch<7>,
    &ARM::Thumb_ConditionalBranch<7>,
    &ARM::Thumb_ConditionalBranch<8>,
    &ARM::Thumb_ConditionalBranch<8>,
    &ARM::Thumb_ConditionalBranch<8>,
    &ARM::Thumb_ConditionalBranch<8>,
    &ARM::Thumb_ConditionalBranch<9>,
    &ARM::Thumb_ConditionalBranch<9>,
    &ARM::Thumb_ConditionalBranch<9>,
    &ARM::Thumb_ConditionalBranch<9>,
    &ARM::Thumb_ConditionalBranch<10>,
    &ARM::Thumb_ConditionalBranch<10>,
    &ARM::Thumb_ConditionalBranch<10>,
    &ARM::Thumb_ConditionalBranch<10>,
    &ARM::Thumb_ConditionalBranch<11>,
    &ARM::Thumb_ConditionalBranch<11>,
    &ARM::Thumb_ConditionalBranch<11>,
    &ARM::Thumb_ConditionalBranch<11>,
    &ARM::Thumb_ConditionalBranch<12>,
    &ARM::Thumb_ConditionalBranch<12>,
    &ARM::Thumb_ConditionalBranch<12>,
    &ARM::Thumb_ConditionalBranch<12>,
    &ARM::Thumb_ConditionalBranch<13>,
    &ARM::Thumb_ConditionalBranch<13>,
    &ARM::Thumb_ConditionalBranch<13>,
    &ARM::Thumb_ConditionalBranch<13>,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_SoftwareInterrupt,
    &ARM::Thumb_SoftwareInterrupt,
    &ARM::Thumb_SoftwareInterrupt,
    &ARM::Thumb_SoftwareInterrupt,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_UnconditionalBranch,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_Undefined,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<0>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>,
    &ARM::Thumb_LongBranchLink<1>
};
