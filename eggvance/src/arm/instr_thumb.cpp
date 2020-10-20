#include "arm.h"

#include "arm/decode.h"

template<uint Instr>
void Arm::Thumb_MoveShiftedRegister(u16 instr)
{
    constexpr uint kAmount = bit::seq< 6, 5>(Instr);
    constexpr uint kOpcode = bit::seq<11, 2>(Instr);

    static_assert(kOpcode != kShiftRor);

    uint rd = bit::seq<0, 3>(instr);
    uint rs = bit::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (kOpcode)
    {
    case kShiftLsl: dst = log(lsl<true>(src, kAmount)); break;
    case kShiftLsr: dst = log(lsr<true>(src, kAmount)); break;
    case kShiftAsr: dst = log(asr<true>(src, kAmount)); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<uint Instr>
void Arm::Thumb_AddSubtract(u16 instr)
{
    enum Opcode
    {
        kOpcodeAddReg,
        kOpcodeSubReg,
        kOpcodeAddImm,
        kOpcodeSubImm
    };

    constexpr uint kRn     = bit::seq<6, 3>(Instr);
    constexpr uint kOpcode = bit::seq<9, 2>(Instr);

    uint rd = bit::seq<0, 3>(instr);
    uint rs = bit::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (kOpcode)
    {
    case kOpcodeAddReg: dst = add(src, regs[kRn]); break;
    case kOpcodeSubReg: dst = sub(src, regs[kRn]); break;
    case kOpcodeAddImm: dst = add(src,      kRn ); break;
    case kOpcodeSubImm: dst = sub(src,      kRn ); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<uint Instr>
void Arm::Thumb_ImmediateOperations(u16 instr)
{
    enum Opcode
    {
        kOpcodeMov,
        kOpcodeCmp,
        kOpcodeAdd,
        kOpcodeSub
    };

    constexpr uint kRd     = bit::seq< 8, 3>(Instr);
    constexpr uint kOpcode = bit::seq<11, 2>(Instr);

    uint amount = bit::seq<0, 8>(instr);

    u32& dst = regs[kRd];
    u32  src = regs[kRd];

    switch (kOpcode)
    {
    case kOpcodeMov: dst = log(     amount); break;
    case kOpcodeCmp:       sub(src, amount); break;
    case kOpcodeAdd: dst = add(src, amount); break;
    case kOpcodeSub: dst = sub(src, amount); break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<uint Instr>
void Arm::Thumb_AluOperations(u16 instr)
{
    enum Opcode
    {
        kOpcodeAnd,
        kOpcodeEor,
        kOpcodeLsl,
        kOpcodeLsr,
        kOpcodeAsr,
        kOpcodeAdc,
        kOpcodeSbc,
        kOpcodeRor,
        kOpcodeTst,
        kOpcodeNeg,
        kOpcodeCmp,
        kOpcodeCmn,
        kOpcodeOrr,
        kOpcodeMul,
        kOpcodeBic,
        kOpcodeMvn
    };

    constexpr uint kOpcode = bit::seq<6, 4>(Instr);

    uint rd = bit::seq<0, 3>(instr);
    uint rs = bit::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (kOpcode)
    {
    case kOpcodeLsl: dst = log(lsl<false>(dst, src)); idle(); break;
    case kOpcodeLsr: dst = log(lsr<false>(dst, src)); idle(); break;
    case kOpcodeAsr: dst = log(asr<false>(dst, src)); idle(); break;
    case kOpcodeRor: dst = log(ror<false>(dst, src)); idle(); break;
    case kOpcodeAnd: dst = log(dst &  src); break;
    case kOpcodeEor: dst = log(dst ^  src); break;
    case kOpcodeOrr: dst = log(dst |  src); break;
    case kOpcodeBic: dst = log(dst & ~src); break;
    case kOpcodeMvn: dst = log(      ~src); break;
    case kOpcodeTst:       log(dst &  src); break;
    case kOpcodeCmn:       add(dst,   src); break;
    case kOpcodeCmp:       sub(dst,   src); break;
    case kOpcodeAdc: dst = adc(dst,   src); break;
    case kOpcodeSbc: dst = sbc(dst,   src); break;
    case kOpcodeNeg: dst = sub(  0,   src); break;
    case kOpcodeMul:
        booth<true>(dst);
        dst = log(dst * src);
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<uint Instr>
void Arm::Thumb_HighRegisterOperations(u16 instr)
{
    enum Opcode
    {
        kOpcodeAdd,
        kOpcodeCmp,
        kOpcodeMov,
        kOpcodeBx
    };

    constexpr uint kHs     = bit::seq<6, 1>(Instr);
    constexpr uint kHd     = bit::seq<7, 1>(Instr);
    constexpr uint kOpcode = bit::seq<8, 2>(Instr);

    uint rd = bit::seq<0, 3>(instr);
    uint rs = bit::seq<3, 3>(instr);

    rs |= kHs << 3;
    rd |= kHd << 3;

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (kOpcode)
    {
    case kOpcodeAdd:
        dst += src;
        if (rd == 15 && kHd)
            flushHalf();
        break;

    case kOpcodeMov:
        dst = src;
        if (rd == 15 && kHd)
            flushHalf();
        break;

    case kOpcodeCmp:
        sub(dst, src);
        break;

    case kOpcodeBx:
        pc = src;
        if ((cpsr.t = src & 0x1))
        {
            flushHalf();
        }
        else
        {
            flushWord();
            state &= ~kStateThumb;
        }
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<uint Instr>
void Arm::Thumb_LoadPcRelative(u16 instr)
{
    constexpr uint kRd = bit::seq<8, 3>(Instr);

    uint offset = bit::seq<0, 8>(instr);

    regs[kRd] = readWord((pc & ~0x3) + (offset << 2));

    idle();
}

template<uint Instr>
void Arm::Thumb_LoadStoreRegisterOffset(u16 instr)
{
    enum Opcode
    {
        kOpcodeStr,
        kOpcodeStrb,
        kOpcodeLdr,
        kOpcodeLdrb
    };

    constexpr uint kRo     = bit::seq< 6, 3>(Instr);
    constexpr uint kOpcode = bit::seq<10, 2>(Instr);

    uint rd = bit::seq<0, 3>(instr);
    uint rb = bit::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[kRo];

    switch (kOpcode)
    {
    case kOpcodeStr:
        writeWord(addr, dst);
        break;

    case kOpcodeStrb:
        writeByte(addr, dst);
        break;

    case kOpcodeLdr:
        dst = readWordRotate(addr);
        idle();
        break;

    case kOpcodeLdrb:
        dst = readByte(addr);
        idle();
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<uint Instr>
void Arm::Thumb_LoadStoreByteHalf(u16 instr)
{
    enum Opcode
    {
        kOpcodeStrh,
        kOpcodeLdrsb,
        kOpcodeLdrh,
        kOpcodeLdrsh
    };

    constexpr uint kRo     = bit::seq< 6, 3>(Instr);
    constexpr uint kOpcode = bit::seq<10, 2>(Instr);

    uint rd = bit::seq<0, 3>(instr);
    uint rb = bit::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[kRo];

    switch (kOpcode)
    {
    case kOpcodeStrh:
        writeHalf(addr, dst);
        break;

    case kOpcodeLdrsb:
        dst = readByte(addr);
        dst = bit::signEx<8>(dst);
        idle();
        break;

    case kOpcodeLdrh:
        dst = readHalfRotate(addr);
        idle();
        break;

    case kOpcodeLdrsh:
        dst = readHalfSignEx(addr);
        idle();
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<uint Instr>
void Arm::Thumb_LoadStoreImmediateOffset(u16 instr)
{
    enum Opcode
    {
        kOpcodeStr,
        kOpcodeLdr,
        kOpcodeStrb,
        kOpcodeLdrb
    };

    constexpr uint kOffset = bit::seq< 6, 5>(Instr);
    constexpr uint kOpcode = bit::seq<11, 2>(Instr);

    uint rd = bit::seq<0, 3>(instr);
    uint rb = bit::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + (kOffset << (~kOpcode & 0x2));

    switch (kOpcode)
    {
    case kOpcodeStr:
        writeWord(addr, dst);
        break;

    case kOpcodeStrb:
        writeByte(addr, dst);
        break;

    case kOpcodeLdr:
        dst = readWordRotate(addr);
        idle();
        break;

    case kOpcodeLdrb:
        dst = readByte(addr);
        idle();
        break;

    default:
        SHELL_UNREACHABLE;
        break;
    }
}

template<uint Instr>
void Arm::Thumb_LoadStoreHalf(u16 instr)
{
    constexpr uint kOffset = bit::seq< 6, 5>(Instr);
    constexpr uint kLoad   = bit::seq<11, 1>(Instr);

    uint rd = bit::seq<0, 3>(instr);
    uint rb = bit::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + (kOffset << 1);

    if (kLoad)
    {
        dst = readHalfRotate(addr);
        idle();
    }
    else
    {
        writeHalf(addr, dst);
    }
}

template<uint Instr>
void Arm::Thumb_LoadStoreSpRelative(u16 instr)
{
    constexpr uint kRd   = bit::seq< 8, 3>(Instr);
    constexpr uint kLoad = bit::seq<11, 1>(Instr);

    uint offset = bit::seq<0, 8>(instr);

    u32& dst = regs[kRd];
    u32 addr = sp + (offset << 2);

    if (kLoad)
    {
        dst = readWordRotate(addr);
        idle();
    }
    else
    {
        writeWord(addr, dst);
    }
}

template<uint Instr>
void Arm::Thumb_LoadRelativeAddress(u16 instr)
{
    constexpr uint kRd = bit::seq< 8, 3>(Instr);
    constexpr uint kSp = bit::seq<11, 1>(Instr);

    uint offset = bit::seq<0, 8>(instr);

    offset <<= 2;

    regs[kRd] = (kSp ? (sp & ~0x0) : (pc & ~0x2)) + offset;
}

template<uint Instr>
void Arm::Thumb_AddOffsetSp(u16 instr)
{
    constexpr uint kSign = bit::seq<7, 1>(Instr);

    uint offset = bit::seq<0, 7>(instr);

    offset <<= 2;

    if (kSign)
        sp -= offset;
    else
        sp += offset; 
}

template<uint Instr>
void Arm::Thumb_PushPopRegisters(u16 instr)
{
    constexpr uint kRbit = bit::seq< 8, 1>(Instr);
    constexpr uint kPop  = bit::seq<11, 1>(Instr);

    uint rlist = bit::seq<0, 8>(instr);

    rlist |= kRbit << (kPop ? 15 : 14);

    if (kPop)
    {
        for (uint x : bit::iterateBits(rlist))
        {
            regs[x] = readWord(sp);
            sp += 4;
        }

        if (kRbit)
            flushHalf();

        idle();
    }
    else
    {
        sp -= 4 * bit::popcnt(rlist);

        u32 addr = sp;

        for (uint x : bit::iterateBits(rlist))
        {
            writeWord(addr, regs[x]);
            addr += 4;
        }
    }
}

template<uint Instr>
void Arm::Thumb_LoadStoreMultiple(u16 instr)
{
    constexpr uint kRb   = bit::seq< 8, 3>(Instr);
    constexpr uint kLoad = bit::seq<11, 1>(Instr);

    uint rlist = bit::seq<0, 8>(instr);

    u32 addr = regs[kRb];
    u32 base = regs[kRb];

    bool writeback = true;

    if (rlist != 0)
    {
        if (kLoad)
        {
            if (rlist & (1 << kRb))
                writeback = false;

            for (uint x : bit::iterateBits(rlist))
            {
                regs[x] = readWord(addr);
                addr += 4;
            }
            idle();
        }
        else
        {
            for (uint x : bit::iterateBits(rlist))
            {
                u32 value = x != kRb
                    ? regs[x]
                    : x == bit::ctz(rlist)
                        ? base
                        : base + 4 * bit::popcnt(rlist);

                writeWord(addr, value);
                addr += 4;
            }
        }
    }
    else
    {
        if (kLoad)
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
        regs[kRb] = addr;
}

template<uint Instr>
void Arm::Thumb_ConditionalBranch(u16 instr)
{
    constexpr uint kCondition = bit::seq<8, 4>(Instr);

    if (cpsr.check(kCondition))
    {
        uint offset = bit::seq<0, 8>(instr);

        offset = bit::signEx<8>(offset);
        offset <<= 1;

        pc += offset;
        flushHalf();
    }
}

template<uint Instr>
void Arm::Thumb_SoftwareInterrupt(u16 instr)
{
    interruptSw();
}

template<uint Instr>
void Arm::Thumb_UnconditionalBranch(u16 instr)
{
    uint offset = bit::seq<0, 11>(instr);

    offset = bit::signEx<11>(offset);
    offset <<= 1;

    pc += offset;
    flushHalf();
}

template<uint Instr>
void Arm::Thumb_LongBranchLink(u16 instr)
{
    constexpr uint kSecond = bit::seq<11, 1>(Instr);

    uint offset = bit::seq<0, 11>(instr);

    if (kSecond)
    {
        offset <<= 1;

        u32 next = (pc - 2) | 0x1;
        pc = lr + offset;
        lr = next;

        flushHalf();
    }
    else
    {
        offset = bit::signEx<11>(offset);
        offset <<= 12;

        lr = pc + offset;
    }
}

template<uint Instr>
void Arm::Thumb_Undefined(u16 instr)
{
    SHELL_ASSERT(false, SHELL_FUNCTION);
}

template<uint Hash>
constexpr Arm::Instruction16 Arm::Thumb_Decode()
{
    constexpr auto kDehash = dehashThumb(Hash);
    constexpr auto kDecode = decodeThumb(Hash);

    if constexpr (kDecode == InstructionThumb::MoveShiftedRegister)      return &Arm::Thumb_MoveShiftedRegister<kDehash>;
    if constexpr (kDecode == InstructionThumb::AddSubtract)              return &Arm::Thumb_AddSubtract<kDehash>;
    if constexpr (kDecode == InstructionThumb::ImmediateOperations)      return &Arm::Thumb_ImmediateOperations<kDehash>;
    if constexpr (kDecode == InstructionThumb::AluOperations)            return &Arm::Thumb_AluOperations<kDehash>;
    if constexpr (kDecode == InstructionThumb::HighRegisterOperations)   return &Arm::Thumb_HighRegisterOperations<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadPcRelative)           return &Arm::Thumb_LoadPcRelative<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreRegisterOffset)  return &Arm::Thumb_LoadStoreRegisterOffset<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreByteHalf)        return &Arm::Thumb_LoadStoreByteHalf<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreImmediateOffset) return &Arm::Thumb_LoadStoreImmediateOffset<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreHalf)            return &Arm::Thumb_LoadStoreHalf<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreSpRelative)      return &Arm::Thumb_LoadStoreSpRelative<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadRelativeAddress)      return &Arm::Thumb_LoadRelativeAddress<kDehash>;
    if constexpr (kDecode == InstructionThumb::AddOffsetSp)              return &Arm::Thumb_AddOffsetSp<kDehash>;
    if constexpr (kDecode == InstructionThumb::PushPopRegisters)         return &Arm::Thumb_PushPopRegisters<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreMultiple)        return &Arm::Thumb_LoadStoreMultiple<kDehash>;
    if constexpr (kDecode == InstructionThumb::ConditionalBranch)        return &Arm::Thumb_ConditionalBranch<kDehash>;
    if constexpr (kDecode == InstructionThumb::SoftwareInterrupt)        return &Arm::Thumb_SoftwareInterrupt<kDehash>;
    if constexpr (kDecode == InstructionThumb::UnconditionalBranch)      return &Arm::Thumb_UnconditionalBranch<kDehash>;
    if constexpr (kDecode == InstructionThumb::LongBranchLink)           return &Arm::Thumb_LongBranchLink<kDehash>;
    if constexpr (kDecode == InstructionThumb::Undefined)                return &Arm::Thumb_Undefined<kDehash>;
}

#define DECODE0001(hash) Thumb_Decode<hash>(),
#define DECODE0004(hash) DECODE0001(hash + 0 *   1) DECODE0001(hash + 1 *   1) DECODE0001(hash + 2 *   1) DECODE0001(hash + 3 *   1)
#define DECODE0016(hash) DECODE0004(hash + 0 *   4) DECODE0004(hash + 1 *   4) DECODE0004(hash + 2 *   4) DECODE0004(hash + 3 *   4)
#define DECODE0064(hash) DECODE0016(hash + 0 *  16) DECODE0016(hash + 1 *  16) DECODE0016(hash + 2 *  16) DECODE0016(hash + 3 *  16)
#define DECODE0256(hash) DECODE0064(hash + 0 *  64) DECODE0064(hash + 1 *  64) DECODE0064(hash + 2 *  64) DECODE0064(hash + 3 *  64)
#define DECODE1024(hash) DECODE0256(hash + 0 * 256) DECODE0256(hash + 1 * 256) DECODE0256(hash + 2 * 256) DECODE0256(hash + 3 * 256)

const std::array<Arm::Instruction16, 1024> Arm::instr_thumb = { DECODE1024(0) };

#undef DECODE0001
#undef DECODE0004
#undef DECODE0016
#undef DECODE0064
#undef DECODE0256
#undef DECODE1024
