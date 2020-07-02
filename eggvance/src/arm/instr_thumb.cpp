#include "arm.h"

#include "arm/decode.h"

template<uint Instr>
void ARM::Thumb_MoveShiftedRegister(u16 instr)
{
    static constexpr uint kAmount = bits::seq< 6, 5>(Instr);
    static constexpr uint kOpcode = bits::seq<11, 2>(Instr);

    uint rd = bits::seq<0, 3>(instr);
    uint rs = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    static_assert(kOpcode != kShiftRor);

    switch (kOpcode)
    {
    case kShiftLsl: dst = log(lsl      (src, kAmount)); break;
    case kShiftLsr: dst = log(lsr<true>(src, kAmount)); break;
    case kShiftAsr: dst = log(asr<true>(src, kAmount)); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint Instr>
void ARM::Thumb_AddSubtract(u16 instr)
{
    enum Opcode
    {
        kOpcodeAddReg,
        kOpcodeSubReg,
        kOpcodeAddImm,
        kOpcodeSubImm
    };

    static constexpr uint kRn     = bits::seq<6, 3>(Instr);
    static constexpr uint kOpcode = bits::seq<9, 2>(Instr);

    uint rd = bits::seq<0, 3>(instr);
    uint rs = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (kOpcode)
    {
    case kOpcodeAddImm: dst = add(src,       kRn); break;
    case kOpcodeSubImm: dst = sub(src,       kRn); break;
    case kOpcodeAddReg: dst = add(src, regs[kRn]); break;
    case kOpcodeSubReg: dst = sub(src, regs[kRn]); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint Instr>
void ARM::Thumb_ImmediateOperations(u16 instr)
{
    enum Opcode
    {
        kOpcodeMov,
        kOpcodeCmp,
        kOpcodeAdd,
        kOpcodeSub
    };

    static constexpr uint kRd     = bits::seq< 8, 3>(Instr);
    static constexpr uint kOpcode = bits::seq<11, 2>(Instr);

    uint offset = bits::seq<0, 8>(instr);

    u32& dst = regs[kRd];
    u32  src = regs[kRd];

    switch (kOpcode)
    {
    case kOpcodeMov: dst = log(     offset); break;
    case kOpcodeCmp:       sub(src, offset); break;
    case kOpcodeAdd: dst = add(src, offset); break;
    case kOpcodeSub: dst = sub(src, offset); break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint Instr>
void ARM::Thumb_AluOperations(u16 instr)
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

    static constexpr uint kOpcode = bits::seq<6, 4>(Instr);

    uint rd = bits::seq<0, 3>(instr);
    uint rs = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32  src = regs[rs];

    switch (kOpcode)
    {
    case kOpcodeLsl: dst = log(lsl       (dst, src)); idle(); break;
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
        booth(dst, true);
        dst = log(dst * src);
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint Instr>
void ARM::Thumb_HighRegisterOperations(u16 instr)
{
    enum Opcode
    {
        kOpcodeAdd,
        kOpcodeCmp,
        kOpcodeMov,
        kOpcodeBx
    };

    static constexpr uint kHs     = bits::seq<6, 1>(Instr);
    static constexpr uint kHd     = bits::seq<7, 1>(Instr);
    static constexpr uint kOpcode = bits::seq<8, 2>(Instr);

    uint rd = bits::seq<0, 3>(instr);
    uint rs = bits::seq<3, 3>(instr);

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
        if (cpsr.t = src & 0x1)
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
        UNREACHABLE;
        break;
    }
}

template<uint Instr>
void ARM::Thumb_LoadPcRelative(u16 instr)
{
    static constexpr uint kRd = bits::seq<8, 3>(Instr);

    uint offset = bits::seq<0, 8>(instr);

    regs[kRd] = readWord((pc & ~0x3) + (offset << 2));

    idle();
}

template<uint Instr>
void ARM::Thumb_LoadStoreRegisterOffset(u16 instr)
{
    enum Opcode
    {
        kOpcodeStr,
        kOpcodeStrb,
        kOpcodeLdr,
        kOpcodeLdrb
    };

    static constexpr uint kRo     = bits::seq< 6, 3>(Instr);
    static constexpr uint kOpcode = bits::seq<10, 2>(Instr);

    uint rd = bits::seq<0, 3>(instr);
    uint rb = bits::seq<3, 3>(instr);

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
        dst = readWordRotated(addr);
        idle();
        break;

    case kOpcodeLdrb:
        dst = readByte(addr);
        idle();
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint Instr>
void ARM::Thumb_LoadStoreByteHalf(u16 instr)
{
    enum Opcode
    {
        kOpcodeStrh,
        kOpcodeLdrsb,
        kOpcodeLdrh,
        kOpcodeLdrsh
    };

    static constexpr uint kRo     = bits::seq< 6, 3>(Instr);
    static constexpr uint kOpcode = bits::seq<10, 2>(Instr);

    uint rd = bits::seq<0, 3>(instr);
    uint rb = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + regs[kRo];

    switch (kOpcode)
    {
    case kOpcodeStrh:
        writeHalf(addr, dst);
        break;

    case kOpcodeLdrsb:
        dst = readByte(addr);
        dst = bits::sx<8>(dst);
        idle();
        break;

    case kOpcodeLdrh:
        dst = readHalfRotated(addr);
        idle();
        break;

    case kOpcodeLdrsh:
        dst = readHalfSigned(addr);
        idle();
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint Instr>
void ARM::Thumb_LoadStoreImmediateOffset(u16 instr)
{
    enum Opcode
    {
        kOpcodeStr,
        kOpcodeLdr,
        kOpcodeStrb,
        kOpcodeLdrb
    };

    static constexpr uint kAmount = bits::seq< 6, 5>(Instr);
    static constexpr uint kOpcode = bits::seq<11, 2>(Instr);

    uint rd = bits::seq<0, 3>(instr);
    uint rb = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + (kAmount << (~kOpcode & 0x2));

    switch (kOpcode)
    {
    case kOpcodeStr:
        writeWord(addr, dst);
        break;

    case kOpcodeStrb:
        writeByte(addr, dst);
        break;

    case kOpcodeLdr:
        dst = readWordRotated(addr);
        idle();
        break;

    case kOpcodeLdrb:
        dst = readByte(addr);
        idle();
        break;

    default:
        UNREACHABLE;
        break;
    }
}

template<uint Instr>
void ARM::Thumb_LoadStoreHalf(u16 instr)
{
    static constexpr uint kAmount = bits::seq< 6, 5>(Instr);
    static constexpr uint kLoad   = bits::seq<11, 1>(Instr);

    uint rd = bits::seq<0, 3>(instr);
    uint rb = bits::seq<3, 3>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rb] + (kAmount << 1);

    if (kLoad)
    {
        dst = readHalfRotated(addr);
        idle();
    }
    else
    {
        writeHalf(addr, dst);
    }
}

template<uint Instr>
void ARM::Thumb_LoadStoreSpRelative(u16 instr)
{
    static constexpr uint kRd   = bits::seq< 8, 3>(Instr);
    static constexpr uint kLoad = bits::seq<11, 1>(Instr);

    uint offset = bits::seq<0, 8>(instr);

    u32& dst = regs[kRd];
    u32 addr = sp + (offset << 2);

    if (kLoad)
    {
        dst = readWordRotated(addr);
        idle();
    }
    else
    {
        writeWord(addr, dst);
    }
}

template<uint Instr>
void ARM::Thumb_LoadRelativeAddress(u16 instr)
{
    static constexpr uint kRd = bits::seq< 8, 3>(Instr);
    static constexpr uint kSp = bits::seq<11, 1>(Instr);

    uint offset = bits::seq<0, 8>(instr);

    offset <<= 2;

    regs[kRd] = (kSp ? (sp & ~0x0) : (pc & ~0x2)) + offset;
}

template<uint Instr>
void ARM::Thumb_AddOffsetSp(u16 instr)
{
    static constexpr uint kSign = bits::seq<7, 1>(Instr);

    uint offset = bits::seq<0, 7>(instr);

    offset <<= 2;

    if (kSign)
        sp -= offset;
    else
        sp += offset; 
}

template<uint Instr>
void ARM::Thumb_PushPopRegisters(u16 instr)
{
    static constexpr uint kRbit = bits::seq< 8, 1>(Instr);
    static constexpr uint kPop  = bits::seq<11, 1>(Instr);

    uint rlist = bits::seq<0, 8>(instr);

    rlist |= kRbit << (kPop ? 15 : 14);

    if (kPop)
    {
        for (uint x : bits::iter(rlist))
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
        sp -= 4 * bits::popcnt(rlist);

        u32 addr = sp;

        for (uint x : bits::iter(rlist))
        {
            writeWord(addr, regs[x]);
            addr += 4;
        }
    }
}

template<uint Instr>
void ARM::Thumb_LoadStoreMultiple(u16 instr)
{
    static constexpr uint kRb   = bits::seq< 8, 3>(Instr);
    static constexpr uint kLoad = bits::seq<11, 1>(Instr);

    uint rlist = bits::seq<0, 8>(instr);

    u32 addr = regs[kRb];
    u32 base = regs[kRb];

    bool writeback = true;

    if (rlist != 0)
    {
        if (kLoad)
        {
            if (rlist & (1 << kRb))
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
            for (uint x : bits::iter(rlist))
            {
                u32 value = x != kRb
                    ? regs[x]
                    : x == bits::ctz(rlist)
                        ? base
                        : base + 4 * bits::popcnt(rlist);

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
void ARM::Thumb_ConditionalBranch(u16 instr)
{
    static constexpr uint kCondition = bits::seq<8, 4>(Instr);

    if (cpsr.check(kCondition))
    {
        uint offset = bits::seq<0, 8>(instr);

        offset = bits::sx<8>(offset);
        offset <<= 1;

        pc += offset;
        flushHalf();
    }
}

template<uint Instr>
void ARM::Thumb_SoftwareInterrupt(u16 instr)
{
    interruptSW();
}

template<uint Instr>
void ARM::Thumb_UnconditionalBranch(u16 instr)
{
    uint offset = bits::seq<0, 11>(instr);

    offset = bits::sx<11>(offset);
    offset <<= 1;

    pc += offset;
    flushHalf();
}

template<uint Instr>
void ARM::Thumb_LongBranchLink(u16 instr)
{
    static constexpr uint kSecond = bits::seq<11, 1>(Instr);

    uint offset = bits::seq<0, 11>(instr);

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
        offset = bits::sx<11>(offset);
        offset <<= 12;

        lr = pc + offset;
    }
}

template<uint Instr>
void ARM::Thumb_Undefined(u16 instr)
{
    ASSERT(false, __FUNCTION__);
}

template<uint Hash>
constexpr ARM::Handler16 ARM::Thumb_Decode()
{
    constexpr auto kDehash = dehashThumb(Hash);
    constexpr auto kDecode = decodeThumb(Hash);

    if constexpr (kDecode == InstructionThumb::MoveShiftedRegister)      return &ARM::Thumb_MoveShiftedRegister<kDehash>;
    if constexpr (kDecode == InstructionThumb::AddSubtract)              return &ARM::Thumb_AddSubtract<kDehash>;
    if constexpr (kDecode == InstructionThumb::ImmediateOperations)      return &ARM::Thumb_ImmediateOperations<kDehash>;
    if constexpr (kDecode == InstructionThumb::AluOperations)            return &ARM::Thumb_AluOperations<kDehash>;
    if constexpr (kDecode == InstructionThumb::HighRegisterOperations)   return &ARM::Thumb_HighRegisterOperations<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadPcRelative)           return &ARM::Thumb_LoadPcRelative<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreRegisterOffset)  return &ARM::Thumb_LoadStoreRegisterOffset<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreByteHalf)        return &ARM::Thumb_LoadStoreByteHalf<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreImmediateOffset) return &ARM::Thumb_LoadStoreImmediateOffset<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreHalf)            return &ARM::Thumb_LoadStoreHalf<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreSpRelative)      return &ARM::Thumb_LoadStoreSpRelative<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadRelativeAddress)      return &ARM::Thumb_LoadRelativeAddress<kDehash>;
    if constexpr (kDecode == InstructionThumb::AddOffsetSp)              return &ARM::Thumb_AddOffsetSp<kDehash>;
    if constexpr (kDecode == InstructionThumb::PushPopRegisters)         return &ARM::Thumb_PushPopRegisters<kDehash>;
    if constexpr (kDecode == InstructionThumb::LoadStoreMultiple)        return &ARM::Thumb_LoadStoreMultiple<kDehash>;
    if constexpr (kDecode == InstructionThumb::ConditionalBranch)        return &ARM::Thumb_ConditionalBranch<kDehash>;
    if constexpr (kDecode == InstructionThumb::SoftwareInterrupt)        return &ARM::Thumb_SoftwareInterrupt<kDehash>;
    if constexpr (kDecode == InstructionThumb::UnconditionalBranch)      return &ARM::Thumb_UnconditionalBranch<kDehash>;
    if constexpr (kDecode == InstructionThumb::LongBranchLink)           return &ARM::Thumb_LongBranchLink<kDehash>;
    if constexpr (kDecode == InstructionThumb::Undefined)                return &ARM::Thumb_Undefined<kDehash>;
}

#define DECODE0001(hash) Thumb_Decode<hash>(),
#define DECODE0004(hash) DECODE0001(hash + 0 *   1) DECODE0001(hash + 1 *   1) DECODE0001(hash + 2 *   1) DECODE0001(hash + 3 *   1)
#define DECODE0016(hash) DECODE0004(hash + 0 *   4) DECODE0004(hash + 1 *   4) DECODE0004(hash + 2 *   4) DECODE0004(hash + 3 *   4)
#define DECODE0064(hash) DECODE0016(hash + 0 *  16) DECODE0016(hash + 1 *  16) DECODE0016(hash + 2 *  16) DECODE0016(hash + 3 *  16)
#define DECODE0256(hash) DECODE0064(hash + 0 *  64) DECODE0064(hash + 1 *  64) DECODE0064(hash + 2 *  64) DECODE0064(hash + 3 *  64)
#define DECODE1024(hash) DECODE0256(hash + 0 * 256) DECODE0256(hash + 1 * 256) DECODE0256(hash + 2 * 256) DECODE0256(hash + 3 * 256)

std::array<ARM::Handler16, 1024> ARM::instr_thumb = { DECODE1024(0) };

#undef DECODE0001
#undef DECODE0004
#undef DECODE0016
#undef DECODE0064
#undef DECODE0256
#undef DECODE1024
