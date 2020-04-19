#include "arm.h"

#include "util.h"

template<uint amount, uint opcode>
void ARM::Thumb_MoveShiftedRegister(u16 instr)
{
    static_assert(static_cast<Shift>(opcode) != Shift::Ror);

    uint rd = bits::seq<0, 3>(instr);
    uint rs = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (static_cast<Shift>(opcode))
    {
    case Shift::Lsl: dst = util::log(util::lslThumb<amount>(src, cpsr), cpsr); break;
    case Shift::Lsr: dst = util::log(util::lsrThumb<amount>(src, cpsr), cpsr); break;
    case Shift::Asr: dst = util::log(util::asrThumb<amount>(src, cpsr), cpsr); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint rn, uint opcode>
void ARM::Thumb_AddSubtract(u16 instr)
{
    enum class Opcode
    {
        AddReg = 0b00,
        SubReg = 0b01,
        AddImm = 0b10,
        SubImm = 0b11
    };

    uint rd = bits::seq<0, 3>(instr);
    uint rs = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (static_cast<Opcode>(opcode))
    {
    case Opcode::AddImm: dst = util::add(src, rn, cpsr); break;
    case Opcode::SubImm: dst = util::sub(src, rn, cpsr); break;
    case Opcode::AddReg: dst = util::add(src, regs[rn], cpsr); break;
    case Opcode::SubReg: dst = util::sub(src, regs[rn], cpsr); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint rd, uint opcode>
void ARM::Thumb_ImmediateOperations(u16 instr)
{
    enum class Opcode
    {
        Mov = 0b00,
        Cmp = 0b01,
        Add = 0b10,
        Sub = 0b11
    };

    uint offset = bits::seq<0, 8>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rd];

    switch (static_cast<Opcode>(opcode))
    {
    case Opcode::Mov: dst = util::log(     offset, cpsr); break;
    case Opcode::Cmp:       util::sub(src, offset, cpsr); break;
    case Opcode::Add: dst = util::add(src, offset, cpsr); break;
    case Opcode::Sub: dst = util::sub(src, offset, cpsr); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint opcode>
void ARM::Thumb_AluOperations(u16 instr)
{
    enum class Opcode
    {
        And = 0b0000,
        Eor = 0b0001,
        Lsl = 0b0010,
        Lsr = 0b0011,
        Asr = 0b0100,
        Adc = 0b0101,
        Sbc = 0b0110,
        Ror = 0b0111,
        Tst = 0b1000,
        Neg = 0b1001,
        Cmp = 0b1010,
        Cmn = 0b1011,
        Orr = 0b1100,
        Mul = 0b1101,
        Bic = 0b1110,
        Mvn = 0b1111
    };

    uint rd = bits::seq<0, 3>(instr);
    uint rs = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (static_cast<Opcode>(opcode))
    {
    case Opcode::Lsl: dst = util::log(util::lslThumb(dst, src, cpsr), cpsr); idle(); break;
    case Opcode::Lsr: dst = util::log(util::lsrThumb(dst, src, cpsr), cpsr); idle(); break;
    case Opcode::Asr: dst = util::log(util::asrThumb(dst, src, cpsr), cpsr); idle(); break;
    case Opcode::Ror: dst = util::log(util::rorThumb(dst, src, cpsr), cpsr); idle(); break;
    case Opcode::And: dst = util::log(dst &  src, cpsr); break;
    case Opcode::Eor: dst = util::log(dst ^  src, cpsr); break;
    case Opcode::Orr: dst = util::log(dst |  src, cpsr); break;
    case Opcode::Bic: dst = util::log(dst & ~src, cpsr); break;
    case Opcode::Mvn: dst = util::log(      ~src, cpsr); break;
    case Opcode::Tst:       util::log(dst &  src, cpsr); break;
    case Opcode::Cmn:       util::add(dst,   src, cpsr); break;
    case Opcode::Cmp:       util::sub(dst,   src, cpsr); break;
    case Opcode::Adc: dst = util::adc(dst,   src, cpsr); break;
    case Opcode::Sbc: dst = util::sbc(dst,   src, cpsr); break;
    case Opcode::Neg: dst = util::sub(  0,   src, cpsr); break;
    case Opcode::Mul:
        booth(dst, true);
        dst = util::log(dst * src, cpsr);
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint hs, uint hd, uint opcode>
void ARM::Thumb_HighRegisterOperations(u16 instr)
{
    enum class Opcode
    {
        Add = 0b00,
        Cmp = 0b01,
        Mov = 0b10,
        Bx  = 0b11
    };

    uint rd = bits::seq<0, 3>(instr);
    uint rs = bits::seq<3, 3>(instr);

    rs |= hs << 3;
    rd |= hd << 3;

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (static_cast<Opcode>(opcode))
    {
    case Opcode::Add:
        dst += src;
        if (rd == 15 && hd)
            flushHalf();
        break;

    case Opcode::Mov:
        dst = src;
        if (rd == 15 && hd)
            flushHalf();
        break;

    case Opcode::Cmp:
        util::sub(dst, src, cpsr);
        break;

    case Opcode::Bx:
        pc = src;
        if (cpsr.t = src & 0x1)
        {
            flushHalf();
        }
        else
        {
            flushWord();
            state &= ~STATE_THUMB;
        }
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint rd>
void ARM::Thumb_LoadPcRelative(u16 instr)
{
    uint offset = bits::seq<0, 8>(instr);

    regs[rd] = readWord((pc & ~0x3) + (offset << 2));

    idle();
}

template<uint ro, uint opcode>
void ARM::Thumb_LoadStoreRegisterOffset(u16 instr)
{
    enum class Opcode
    {
        Str  = 0b00,
        Strb = 0b01,
        Ldr  = 0b10,
        Ldrb = 0b11
    };

    uint rd = bits::seq<0, 3>(instr);
    uint rb = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[ro];

    switch (static_cast<Opcode>(opcode))
    {
    case Opcode::Str:
        writeWord(addr, dst);
        break;

    case Opcode::Strb:
        writeByte(addr, dst);
        break;

    case Opcode::Ldr:
        dst = readWordRotated(addr);
        idle();
        break;

    case Opcode::Ldrb:
        dst = readByte(addr);
        idle();
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint ro, uint opcode>
void ARM::Thumb_LoadStoreByteHalf(u16 instr)
{
    enum class Opcode
    {
        Strh  = 0b00,
        Ldrsb = 0b01,
        Ldrh  = 0b10,
        Ldrsh = 0b11
    };

    uint rd = bits::seq<0, 3>(instr);
    uint rb = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[ro];

    switch (static_cast<Opcode>(opcode))
    {
    case Opcode::Strh:
        writeHalf(addr, dst);
        break;

    case Opcode::Ldrsb:
        dst = readByte(addr);
        dst = bits::sx<8>(dst);
        idle();
        break;

    case Opcode::Ldrh:
        dst = readHalfRotated(addr);
        idle();
        break;

    case Opcode::Ldrsh:
        dst = readHalfSigned(addr);
        idle();
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint amount, uint opcode>
void ARM::Thumb_LoadStoreImmediateOffset(u16 instr)
{
    enum class Opcode
    {
        Str  = 0b00,
        Ldr  = 0b01,
        Strb = 0b10,
        Ldrb = 0b11
    };

    uint rd = bits::seq<0, 3>(instr);
    uint rb = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + (amount << (~opcode & 0x2));

    switch (static_cast<Opcode>(opcode))
    {
    case Opcode::Str:
        writeWord(addr, dst);
        break;

    case Opcode::Strb:
        writeByte(addr, dst);
        break;

    case Opcode::Ldr:
        dst = readWordRotated(addr);
        idle();
        break;

    case Opcode::Ldrb:
        dst = readByte(addr);
        idle();
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint amount, uint load>
void ARM::Thumb_LoadStoreHalf(u16 instr)
{
    uint rd = bits::seq<0, 3>(instr);
    uint rb = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + (amount << 1);

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

template<uint rd, uint load>
void ARM::Thumb_LoadStoreSpRelative(u16 instr)
{
    uint offset = bits::seq<0, 8>(instr);

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

template<uint rd, uint use_sp>
void ARM::Thumb_LoadRelativeAddress(u16 instr)
{
    uint offset = bits::seq<0, 8>(instr);

    offset <<= 2;

    u32& dst = regs[rd];

    if (use_sp)
        dst = (sp & ~0x0) + offset;
    else
        dst = (pc & ~0x3) + offset;
}

template<uint sign>
void ARM::Thumb_AddOffsetSp(u16 instr)
{
    uint offset = bits::seq<0, 7>(instr);

    offset <<= 2;

    if (sign)
        sp -= offset;
    else
        sp += offset; 
}

template<uint rbit, uint pop>
void ARM::Thumb_PushPopRegisters(u16 instr)
{
    uint rlist = bits::seq<0, 8>(instr);

    rlist |= rbit << (pop ? 15 : 14);

    if (pop)
    {
        for (uint x : bits::iter(rlist))
        {
            regs[x] = readWord(sp);
            sp += 4;
        }

        if (rbit)
            flushHalf();

        idle();
    }
    else
    {
        sp -= 4 * bits::popcnt(rlist);

        u32 addr = sp;

        for (uint x : bits::iter(rlist))
        {
            writeWord(addr, regs[x]);
            addr += 4;
        }
    }
}

template<uint rb, uint load>
void ARM::Thumb_LoadStoreMultiple(u16 instr)
{
    uint rlist = bits::seq<0, 8>(instr);

    u32 addr = regs[rb];
    u32 base = regs[rb];

    bool writeback = true;

    if (rlist != 0)
    {
        if (load)
        {
            if (rlist & (1 << rb))
                writeback = false;

            for (uint x : bits::iter(rlist))
            {
                regs[x] = readWord(addr);
                addr += 4;
            }
            idle();
        }
        else
        {
            bool begin = true;

            for (uint x : bits::iter(rlist))
            {
                u32 value = x != rb
                    ? regs[x]
                    : begin
                        ? base
                        : base + 4 * bits::popcnt(rlist);

                writeWord(addr, value);
                addr += 4;

                begin = false;
            }
        }
    }
    else
    {
        if (load)
        {
            pc = readWord(addr);
            flushHalf();
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

template<uint condition>
void ARM::Thumb_ConditionalBranch(u16 instr)
{
    if (cpsr.check(static_cast<PSR::Condition>(condition)))
    {
        uint offset = bits::seq<0, 8>(instr);

        offset = bits::sx<8>(offset);
        offset <<= 1;

        pc += offset;
        flushHalf();
    }
}

void ARM::Thumb_SoftwareInterrupt(u16 instr)
{
    interruptSW();
}

void ARM::Thumb_UnconditionalBranch(u16 instr)
{
    uint offset = bits::seq<0, 11>(instr);

    offset = bits::sx<11>(offset);
    offset <<= 1;

    pc += offset;
    flushHalf();
}

template<uint second>
void ARM::Thumb_LongBranchLink(u16 instr)
{
    uint offset = bits::seq<0, 11>(instr);

    if (second)
    {
        offset <<= 1;

        u32 next = (pc - 2) | 0x1;
        pc = lr + offset;
        lr = next;

        flushHalf();
    }
    else
    {
        offset = bits::sx<11>(offset);
        offset <<= 12;

        lr = pc + offset;
    }
}

void ARM::Thumb_Undefined(u16 instr)
{
    ASSERT(false, __FUNCTION__);
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
    &ARM::Thumb_AluOperations<0>,
    &ARM::Thumb_AluOperations<1>,
    &ARM::Thumb_AluOperations<2>,
    &ARM::Thumb_AluOperations<3>,
    &ARM::Thumb_AluOperations<4>,
    &ARM::Thumb_AluOperations<5>,
    &ARM::Thumb_AluOperations<6>,
    &ARM::Thumb_AluOperations<7>,
    &ARM::Thumb_AluOperations<8>,
    &ARM::Thumb_AluOperations<9>,
    &ARM::Thumb_AluOperations<10>,
    &ARM::Thumb_AluOperations<11>,
    &ARM::Thumb_AluOperations<12>,
    &ARM::Thumb_AluOperations<13>,
    &ARM::Thumb_AluOperations<14>,
    &ARM::Thumb_AluOperations<15>,
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
    &ARM::Thumb_LoadPcRelative<0>,
    &ARM::Thumb_LoadPcRelative<0>,
    &ARM::Thumb_LoadPcRelative<0>,
    &ARM::Thumb_LoadPcRelative<0>,
    &ARM::Thumb_LoadPcRelative<1>,
    &ARM::Thumb_LoadPcRelative<1>,
    &ARM::Thumb_LoadPcRelative<1>,
    &ARM::Thumb_LoadPcRelative<1>,
    &ARM::Thumb_LoadPcRelative<2>,
    &ARM::Thumb_LoadPcRelative<2>,
    &ARM::Thumb_LoadPcRelative<2>,
    &ARM::Thumb_LoadPcRelative<2>,
    &ARM::Thumb_LoadPcRelative<3>,
    &ARM::Thumb_LoadPcRelative<3>,
    &ARM::Thumb_LoadPcRelative<3>,
    &ARM::Thumb_LoadPcRelative<3>,
    &ARM::Thumb_LoadPcRelative<4>,
    &ARM::Thumb_LoadPcRelative<4>,
    &ARM::Thumb_LoadPcRelative<4>,
    &ARM::Thumb_LoadPcRelative<4>,
    &ARM::Thumb_LoadPcRelative<5>,
    &ARM::Thumb_LoadPcRelative<5>,
    &ARM::Thumb_LoadPcRelative<5>,
    &ARM::Thumb_LoadPcRelative<5>,
    &ARM::Thumb_LoadPcRelative<6>,
    &ARM::Thumb_LoadPcRelative<6>,
    &ARM::Thumb_LoadPcRelative<6>,
    &ARM::Thumb_LoadPcRelative<6>,
    &ARM::Thumb_LoadPcRelative<7>,
    &ARM::Thumb_LoadPcRelative<7>,
    &ARM::Thumb_LoadPcRelative<7>,
    &ARM::Thumb_LoadPcRelative<7>,
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
    &ARM::Thumb_LoadStoreSpRelative<0, 0>,
    &ARM::Thumb_LoadStoreSpRelative<0, 0>,
    &ARM::Thumb_LoadStoreSpRelative<0, 0>,
    &ARM::Thumb_LoadStoreSpRelative<0, 0>,
    &ARM::Thumb_LoadStoreSpRelative<1, 0>,
    &ARM::Thumb_LoadStoreSpRelative<1, 0>,
    &ARM::Thumb_LoadStoreSpRelative<1, 0>,
    &ARM::Thumb_LoadStoreSpRelative<1, 0>,
    &ARM::Thumb_LoadStoreSpRelative<2, 0>,
    &ARM::Thumb_LoadStoreSpRelative<2, 0>,
    &ARM::Thumb_LoadStoreSpRelative<2, 0>,
    &ARM::Thumb_LoadStoreSpRelative<2, 0>,
    &ARM::Thumb_LoadStoreSpRelative<3, 0>,
    &ARM::Thumb_LoadStoreSpRelative<3, 0>,
    &ARM::Thumb_LoadStoreSpRelative<3, 0>,
    &ARM::Thumb_LoadStoreSpRelative<3, 0>,
    &ARM::Thumb_LoadStoreSpRelative<4, 0>,
    &ARM::Thumb_LoadStoreSpRelative<4, 0>,
    &ARM::Thumb_LoadStoreSpRelative<4, 0>,
    &ARM::Thumb_LoadStoreSpRelative<4, 0>,
    &ARM::Thumb_LoadStoreSpRelative<5, 0>,
    &ARM::Thumb_LoadStoreSpRelative<5, 0>,
    &ARM::Thumb_LoadStoreSpRelative<5, 0>,
    &ARM::Thumb_LoadStoreSpRelative<5, 0>,
    &ARM::Thumb_LoadStoreSpRelative<6, 0>,
    &ARM::Thumb_LoadStoreSpRelative<6, 0>,
    &ARM::Thumb_LoadStoreSpRelative<6, 0>,
    &ARM::Thumb_LoadStoreSpRelative<6, 0>,
    &ARM::Thumb_LoadStoreSpRelative<7, 0>,
    &ARM::Thumb_LoadStoreSpRelative<7, 0>,
    &ARM::Thumb_LoadStoreSpRelative<7, 0>,
    &ARM::Thumb_LoadStoreSpRelative<7, 0>,
    &ARM::Thumb_LoadStoreSpRelative<0, 1>,
    &ARM::Thumb_LoadStoreSpRelative<0, 1>,
    &ARM::Thumb_LoadStoreSpRelative<0, 1>,
    &ARM::Thumb_LoadStoreSpRelative<0, 1>,
    &ARM::Thumb_LoadStoreSpRelative<1, 1>,
    &ARM::Thumb_LoadStoreSpRelative<1, 1>,
    &ARM::Thumb_LoadStoreSpRelative<1, 1>,
    &ARM::Thumb_LoadStoreSpRelative<1, 1>,
    &ARM::Thumb_LoadStoreSpRelative<2, 1>,
    &ARM::Thumb_LoadStoreSpRelative<2, 1>,
    &ARM::Thumb_LoadStoreSpRelative<2, 1>,
    &ARM::Thumb_LoadStoreSpRelative<2, 1>,
    &ARM::Thumb_LoadStoreSpRelative<3, 1>,
    &ARM::Thumb_LoadStoreSpRelative<3, 1>,
    &ARM::Thumb_LoadStoreSpRelative<3, 1>,
    &ARM::Thumb_LoadStoreSpRelative<3, 1>,
    &ARM::Thumb_LoadStoreSpRelative<4, 1>,
    &ARM::Thumb_LoadStoreSpRelative<4, 1>,
    &ARM::Thumb_LoadStoreSpRelative<4, 1>,
    &ARM::Thumb_LoadStoreSpRelative<4, 1>,
    &ARM::Thumb_LoadStoreSpRelative<5, 1>,
    &ARM::Thumb_LoadStoreSpRelative<5, 1>,
    &ARM::Thumb_LoadStoreSpRelative<5, 1>,
    &ARM::Thumb_LoadStoreSpRelative<5, 1>,
    &ARM::Thumb_LoadStoreSpRelative<6, 1>,
    &ARM::Thumb_LoadStoreSpRelative<6, 1>,
    &ARM::Thumb_LoadStoreSpRelative<6, 1>,
    &ARM::Thumb_LoadStoreSpRelative<6, 1>,
    &ARM::Thumb_LoadStoreSpRelative<7, 1>,
    &ARM::Thumb_LoadStoreSpRelative<7, 1>,
    &ARM::Thumb_LoadStoreSpRelative<7, 1>,
    &ARM::Thumb_LoadStoreSpRelative<7, 1>,
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
    &ARM::Thumb_AddOffsetSp<0>,
    &ARM::Thumb_AddOffsetSp<0>,
    &ARM::Thumb_AddOffsetSp<1>,
    &ARM::Thumb_AddOffsetSp<1>,
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
