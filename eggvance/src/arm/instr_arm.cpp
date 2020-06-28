#include "arm.h"

#include "arm/decode.h"

template<uint Instr>
void ARM::Arm_BranchExchange(u32 instr)
{
    uint rn = bits::seq<0, 4>(instr);

    pc = regs[rn];

    if (cpsr.t = pc & 0x1)
    {
        flushHalf();
        state |= kStateThumb;
    }
    else
    {
        flushWord();
    }
}

template<uint Instr>
void ARM::Arm_BranchLink(u32 instr)
{
    static constexpr uint kLink = bits::seq<24, 1>(Instr);

    uint offset = bits::seq<0, 24>(instr);

    offset = bits::sx<24>(offset);
    offset <<= 2;

    if (kLink) lr = pc - 4;

    pc += offset;
    flushWord();
}

template<uint Instr>
void ARM::Arm_DataProcessing(u32 instr)
{
    enum Opcode
    {
        kOpcodeAnd,
        kOpcodeEor,
        kOpcodeSub,
        kOpcodeRsb,
        kOpcodeAdd,
        kOpcodeAdc,
        kOpcodeSbc,
        kOpcodeRsc,
        kOpcodeTst,
        kOpcodeTeq,
        kOpcodeCmp,
        kOpcodeCmn,
        kOpcodeOrr,
        kOpcodeMov,
        kOpcodeBic,
        kOpcodeMvn
    };

    static constexpr uint kFlags  = bits::seq<20, 1>(Instr);
    static constexpr uint kOpcode = bits::seq<21, 4>(Instr);
    static constexpr uint kImmOp  = bits::seq<25, 1>(Instr);

    uint rd    = bits::seq<12, 4>(instr);
    uint rn    = bits::seq<16, 4>(instr);
    uint flags = kFlags && rd != 15;

    u32& dst = regs[rd];
    u32  op1 = regs[rn];
    u32  op2;

    static constexpr bool kLogical = [&]() {
        switch (kOpcode)
        {
        case kOpcodeAnd:
        case kOpcodeEor:
        case kOpcodeOrr:
        case kOpcodeMov:
        case kOpcodeBic:
        case kOpcodeMvn:
        case kOpcodeTst:
        case kOpcodeTeq:
            return true;
        }
        return false;
    }();

    if (kImmOp)
    {
        uint value  = bits::seq<0, 8>(instr);
        uint amount = bits::seq<8, 4>(instr);
        op2 = ror<false>(value, amount << 1, flags && kLogical);
    }
    else
    {
        uint rm     = bits::seq<0, 4>(instr);
        uint reg_op = bits::seq<4, 1>(instr);
        uint shift  = bits::seq<5, 2>(instr);

        op2 = regs[rm];

        if (reg_op)
        {
            uint rs = bits::seq<8, 4>(instr);

            if (rn == 15) op1 += 4;
            if (rm == 15) op2 += 4;

            uint amount = regs[rs] & 0xFF;

            switch (shift)
            {
            case kShiftLsl: op2 = lsl       (op2, amount, flags && kLogical); break;
            case kShiftLsr: op2 = lsr<false>(op2, amount, flags && kLogical); break;
            case kShiftAsr: op2 = asr<false>(op2, amount, flags && kLogical); break;
            case kShiftRor: op2 = ror<false>(op2, amount, flags && kLogical); break;

            default:
                UNREACHABLE;
                break;
            }
            idle();
        }
        else
        {
            uint amount = bits::seq<7, 5>(instr);

            switch (shift)
            {
            case kShiftLsl: op2 = lsl      (op2, amount, flags && kLogical); break;
            case kShiftLsr: op2 = lsr<true>(op2, amount, flags && kLogical); break;
            case kShiftAsr: op2 = asr<true>(op2, amount, flags && kLogical); break;
            case kShiftRor: op2 = ror<true>(op2, amount, flags && kLogical); break;

            default:
                UNREACHABLE;
                break;
            }
        }
    }

    switch (kOpcode)
    {
    case kOpcodeAnd: dst = log(op1 &  op2, flags); break;
    case kOpcodeEor: dst = log(op1 ^  op2, flags); break;
    case kOpcodeOrr: dst = log(op1 |  op2, flags); break;
    case kOpcodeMov: dst = log(       op2, flags); break;
    case kOpcodeBic: dst = log(op1 & ~op2, flags); break;
    case kOpcodeMvn: dst = log(      ~op2, flags); break;
    case kOpcodeTst:       log(op1 &  op2, true ); break;
    case kOpcodeTeq:       log(op1 ^  op2, true ); break;
    case kOpcodeCmn:       add(op1,   op2, true ); break;
    case kOpcodeCmp:       sub(op1,   op2, true ); break;
    case kOpcodeAdd: dst = add(op1,   op2, flags); break;
    case kOpcodeAdc: dst = adc(op1,   op2, flags); break;
    case kOpcodeSub: dst = sub(op1,   op2, flags); break;
    case kOpcodeSbc: dst = sbc(op1,   op2, flags); break;
    case kOpcodeRsb: dst = sub(op2,   op1, flags); break;
    case kOpcodeRsc: dst = sbc(op2,   op1, flags); break;

    default:
        UNREACHABLE;
        break;
    }

    if (rd == 15)
    {
        if (kFlags)
        {
            PSR spsr = this->spsr;
            switchMode(spsr.m);
            cpsr = spsr;
        }

        if (cpsr.t)
        {
            flushHalf();
            state |= kStateThumb;
        }
        else
        {
            flushWord();
        }
    }
}

template<uint Instr>
void ARM::Arm_StatusTransfer(u32 instr)
{
    static constexpr uint kWrite = bits::seq<21, 1>(Instr);
    static constexpr uint kSpsr  = bits::seq<22, 1>(Instr);
    static constexpr uint kImmOp = bits::seq<25, 1>(Instr);

    if (kWrite)
    {
        u32 op;
        if (kImmOp)
        {
            uint value  = bits::seq<0, 8>(instr);
            uint amount = bits::seq<8, 4>(instr);
            op = bits::ror(value, amount << 1);
        }
        else
        {
            uint rm = bits::seq<0, 4>(instr);
            op = regs[rm];
        }

        u32 mask = 0;
        if (instr & (1 << 19)) mask |= 0xFF00'0000;
        if (instr & (1 << 18)) mask |= 0x00FF'0000;
        if (instr & (1 << 17)) mask |= 0x0000'FF00;
        if (instr & (1 << 16)) mask |= 0x0000'00FF;

        if (kSpsr)
        {
            spsr = (spsr & ~mask) | (op & mask);
        }
        else
        {
            if (instr & (1 << 16))
                switchMode(op & 0x1F);

            cpsr = (cpsr & ~mask) | (op & mask);
        }
    }
    else
    {
        uint rd = bits::seq<12, 4>(instr);
        regs[rd] = kSpsr ? spsr : cpsr;
    }
}

template<uint Instr>
void ARM::Arm_Multiply(u32 instr)
{
    static constexpr uint kFlags      = bits::seq<20, 1>(Instr);
    static constexpr uint kAccumulate = bits::seq<21, 1>(Instr);

    uint rm = bits::seq< 0, 4>(instr);
    uint rs = bits::seq< 8, 4>(instr);
    uint rn = bits::seq<12, 4>(instr);
    uint rd = bits::seq<16, 4>(instr);

    u32  op1 = regs[rm];
    u32  op2 = regs[rs];
    u32  op3 = regs[rn];
    u32& dst = regs[rd];

    dst = op1 * op2;

    if (kAccumulate)
    {
        dst += op3;
        idle();
    }
    log(dst, kFlags);

    booth(op2, true);
}

template<uint Instr>
void ARM::Arm_MultiplyLong(u32 instr)
{
    static constexpr uint kFlags      = bits::seq<20, 1>(Instr);
    static constexpr uint kAccumulate = bits::seq<21, 1>(Instr);
    static constexpr uint kSign       = bits::seq<22, 1>(Instr);

    uint rm  = bits::seq< 0, 4>(instr);
    uint rs  = bits::seq< 8, 4>(instr);
    uint rdl = bits::seq<12, 4>(instr);
    uint rdh = bits::seq<16, 4>(instr);

    u64  op1  = regs[rm];
    u64  op2  = regs[rs];
    u32& dstl = regs[rdl];
    u32& dsth = regs[rdh];

    if (kSign)
    {
        op1 = bits::sx<32>(op1);
        op2 = bits::sx<32>(op2);
    }

    u64 res = op1 * op2;

    if (kAccumulate)
    {
        res += (static_cast<u64>(dsth) << 32) | dstl;
        idle();
    }
    log(res, kFlags);

    dstl = static_cast<u32>(res);
    dsth = static_cast<u32>(res >> 32);

    booth(static_cast<u32>(op2), kSign);
}

template<uint Instr>
void ARM::Arm_SingleDataTransfer(u32 instr)
{
    static constexpr uint kLoad      = bits::seq<20, 1>(Instr);
    static constexpr uint kWriteback = bits::seq<21, 1>(Instr);
    static constexpr uint kByte      = bits::seq<22, 1>(Instr);
    static constexpr uint kIncrement = bits::seq<23, 1>(Instr);
    static constexpr uint kPreIndex  = bits::seq<24, 1>(Instr);
    static constexpr uint kRegOp     = bits::seq<25, 1>(Instr);

    uint rd = bits::seq<12, 4>(instr);
    uint rn = bits::seq<16, 4>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rn];

    u32 offset;
    if (kRegOp)
    {
        uint rm     = bits::seq<0, 4>(instr);
        uint reg_op = bits::seq<4, 1>(instr);
        uint shift  = bits::seq<5, 2>(instr);

        uint amount;
        if (reg_op)
        {
            uint rs = bits::seq<8, 4>(instr);
            amount = regs[rs] & 0xFF;
        }
        else
        {
            amount = bits::seq<7, 5>(instr);
        }

        switch (shift)
        {
        case kShiftLsl: offset = lsl      (regs[rm], amount, false); break;
        case kShiftLsr: offset = lsr<true>(regs[rm], amount, false); break;
        case kShiftAsr: offset = asr<true>(regs[rm], amount, false); break;
        case kShiftRor: offset = ror<true>(regs[rm], amount, false); break;

        default:
            UNREACHABLE;
            break;
        }
    }
    else
    {
        offset = bits::seq<0, 12>(instr);
    }

    if (!kIncrement)
        offset = -static_cast<int>(offset);

    if (kPreIndex)
        addr += offset;

    if (kLoad)
    {
        dst = kByte
            ? readByte(addr)
            : readWordRotated(addr);

        if (rd == 15)
            flushWord();

        idle();
    }
    else
    {
        u32 value = rd == 15
            ? dst + 4
            : dst + 0;

        if (kByte)
            writeByte(addr, value);
        else
            writeWord(addr, value);
    }

    if ((kWriteback || !kPreIndex) && (!kLoad || rd != rn))
    {
        if (!kPreIndex)
            addr += offset;

        regs[rn] = addr;
    }
}

template<uint Instr>
void ARM::Arm_HalfSignedDataTransfer(u32 instr)
{
    enum Opcode
    {
        kOpcodeSwap,
        kOpcodeLdrh,
        kOpcodeLdrsb,
        kOpcodeLdrsh
    };

    static constexpr uint kOpcode    = bits::seq< 5, 2>(Instr);
    static constexpr uint kLoad      = bits::seq<20, 1>(Instr);
    static constexpr uint kWriteback = bits::seq<21, 1>(Instr);
    static constexpr uint kImmOp     = bits::seq<22, 1>(Instr);
    static constexpr uint kIncrement = bits::seq<23, 1>(Instr);
    static constexpr uint kPreIndex  = bits::seq<24, 1>(Instr);

    uint rd = bits::seq<12, 4>(instr);
    uint rn = bits::seq<16, 4>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rn];

    u32 offset;
    if (kImmOp)
    {
        uint lower = bits::seq<0, 4>(instr);
        uint upper = bits::seq<8, 4>(instr);
        offset = (upper << 4) | lower;
    }
    else
    {
        uint rm = bits::seq<0, 4>(instr);
        offset = regs[rm];
    }

    if (!kIncrement)
        offset = -static_cast<int>(offset);

    if (kPreIndex)
        addr += offset;

    if (kLoad)
    {
        static_assert(kOpcode != kOpcodeSwap);

        switch (kOpcode)
        {
        case kOpcodeLdrh:
            dst = readHalfRotated(addr);
            break;

        case kOpcodeLdrsb:
            dst = readByte(addr);
            dst = bits::sx<8>(dst);
            break;

        case kOpcodeLdrsh:
            dst = readHalfSigned(addr);
            break;

        default:
            UNREACHABLE;
            break;
        }

        if (rd == 15)
            flushWord();

        idle();
    }
    else
    {
        u32 value = (rd == 15)
            ? dst + 4
            : dst + 0;

        writeHalf(addr, value);
    }

    if ((kWriteback || !kPreIndex) && (!kLoad || rd != rn))
    {
        if (!kPreIndex)
            addr += offset;

        regs[rn] = addr;
    }
}

template<uint Instr>
void ARM::Arm_BlockDataTransfer(u32 instr)
{
    static constexpr uint kLoad      = bits::seq<20, 1>(Instr);
    static constexpr uint kUserMode  = bits::seq<22, 1>(Instr);
    static constexpr uint kIncrement = bits::seq<23, 1>(Instr);

    uint rlist     = bits::seq< 0, 16>(instr);
    uint rn        = bits::seq<16,  4>(instr);
    uint writeback = bits::seq<21,  1>(Instr);
    uint pre_index = bits::seq<24,  1>(Instr);

    u32 addr = regs[rn];
    u32 base = regs[rn];

    uint mode = cpsr.m;
    if (kUserMode)
        switchMode(PSR::kModeUsr);

    if (rlist != 0)
    {
        if (!kIncrement)
        {
            addr -= 4 * bits::popcnt(rlist);
            pre_index ^= 0x1;

            if (writeback)
            {
                regs[rn] = addr;
                writeback = false;
            }
        }

        if (kLoad)
        {
            if (rlist & (1 << rn))
                writeback = false;

            for (uint x : bits::iter(rlist))
            {
                addr += 4 * pre_index;
                regs[x] = readWord(addr);
                addr += 4 * pre_index ^ 0x4;
            }

            if (rlist & (1 << 15))
                flushWord();

            idle();
        }
        else
        {
            for (uint x : bits::iter(rlist))
            {
                u32 value = x != rn
                    ? x != 15
                        ? regs[x] + 0
                        : regs[x] + 4
                    : x == bits::ctz(rlist)
                        ? base
                        : base + (kIncrement ? 4 : -4) * bits::popcnt(rlist);

                addr += 4 * pre_index;
                writeWord(addr, value);
                addr += 4 * pre_index ^ 0x4;
            }
        }
    }
    else
    {
        if (kLoad)
        {
            pc = readWord(addr);
            flushWord();
        }
        else
        {
            enum Suffix
            {
                kSuffixDA,
                kSuffixDB,
                kSuffixIA,
                kSuffixIB
            };

            switch ((kIncrement << 1) | pre_index)
            {
            case kSuffixDA: writeWord(addr - 0x3C, pc + 4); break;
            case kSuffixDB: writeWord(addr - 0x40, pc + 4); break;
            case kSuffixIA: writeWord(addr + 0x00, pc + 4); break;
            case kSuffixIB: writeWord(addr + 0x04, pc + 4); break;

            default:
                UNREACHABLE;
                break;
            }
        }

        addr = kIncrement
            ? addr + 0x40
            : addr - 0x40;
    }

    if (writeback)
        regs[rn] = addr;

    if (kUserMode)
        switchMode(mode);
}

template<uint Instr>
void ARM::Arm_SingleDataSwap(u32 instr)
{
    static constexpr uint kByte = bits::seq<22, 1>(Instr);

    uint rm = bits::seq< 0, 4>(instr);
    uint rd = bits::seq<12, 4>(instr);
    uint rn = bits::seq<16, 4>(instr);

    u32  src = regs[rm];
    u32& dst = regs[rd];
    u32 addr = regs[rn];

    if (kByte)
    {
        dst = readByte(addr);
        writeByte(addr, src);
    }
    else
    {
        dst = readWordRotated(addr);
        writeWord(addr, src);
    }
    idle();
}

template<uint Instr>
void ARM::Arm_SoftwareInterrupt(u32 instr)
{
    interruptSW();
}

template<uint Instr>
void ARM::Arm_CoprocessorDataOperations(u32 instr)
{
    ASSERT(false, __FUNCTION__);
}

template<uint Instr>
void ARM::Arm_CoprocessorDataTransfers(u32 instr)
{
    ASSERT(false, __FUNCTION__);
}

template<uint Instr>
void ARM::Arm_CoprocessorRegisterTransfers(u32 instr)
{
    ASSERT(false, __FUNCTION__);
}

template<uint Instr>
void ARM::Arm_Undefined(u32 instr)
{
    ASSERT(false, __FUNCTION__);
}

template<uint Hash>
constexpr ARM::Handler32 ARM::Arm_Decode()
{
    constexpr auto kDehash = ((Hash & 0xFF0) << 16) | ((Hash & 0xF) << 4);
    constexpr auto kOpcode = decodeArmHash(Hash);

    if constexpr (kOpcode == InstructionArm::BranchExchange)               return &ARM::Arm_BranchExchange<kDehash>;
    if constexpr (kOpcode == InstructionArm::BranchLink)                   return &ARM::Arm_BranchLink<kDehash>;
    if constexpr (kOpcode == InstructionArm::DataProcessing)               return &ARM::Arm_DataProcessing<kDehash>;
    if constexpr (kOpcode == InstructionArm::StatusTransfer)               return &ARM::Arm_StatusTransfer<kDehash>;
    if constexpr (kOpcode == InstructionArm::Multiply)                     return &ARM::Arm_Multiply<kDehash>;
    if constexpr (kOpcode == InstructionArm::MultiplyLong)                 return &ARM::Arm_MultiplyLong<kDehash>;
    if constexpr (kOpcode == InstructionArm::SingleDataTransfer)           return &ARM::Arm_SingleDataTransfer<kDehash>;
    if constexpr (kOpcode == InstructionArm::HalfSignedDataTransfer)       return &ARM::Arm_HalfSignedDataTransfer<kDehash>;
    if constexpr (kOpcode == InstructionArm::BlockDataTransfer)            return &ARM::Arm_BlockDataTransfer<kDehash>;
    if constexpr (kOpcode == InstructionArm::SingleDataSwap)               return &ARM::Arm_SingleDataSwap<kDehash>;
    if constexpr (kOpcode == InstructionArm::SoftwareInterrupt)            return &ARM::Arm_SoftwareInterrupt<kDehash>;
    if constexpr (kOpcode == InstructionArm::CoprocessorDataOperations)    return &ARM::Arm_CoprocessorDataOperations<kDehash>;
    if constexpr (kOpcode == InstructionArm::CoprocessorDataTransfers)     return &ARM::Arm_CoprocessorDataTransfers<kDehash>;
    if constexpr (kOpcode == InstructionArm::CoprocessorRegisterTransfers) return &ARM::Arm_CoprocessorRegisterTransfers<kDehash>;
    if constexpr (kOpcode == InstructionArm::Undefined)                    return &ARM::Arm_Undefined<kDehash>;
}

#define DECODE0001(hash) Arm_Decode<hash>(),
#define DECODE0004(hash) DECODE0001(hash + 0 *    1) DECODE0001(hash + 1 *    1) DECODE0001(hash + 2 *    1) DECODE0001(hash + 3 *    1)
#define DECODE0016(hash) DECODE0004(hash + 0 *    4) DECODE0004(hash + 1 *    4) DECODE0004(hash + 2 *    4) DECODE0004(hash + 3 *    4)
#define DECODE0064(hash) DECODE0016(hash + 0 *   16) DECODE0016(hash + 1 *   16) DECODE0016(hash + 2 *   16) DECODE0016(hash + 3 *   16)
#define DECODE0256(hash) DECODE0064(hash + 0 *   64) DECODE0064(hash + 1 *   64) DECODE0064(hash + 2 *   64) DECODE0064(hash + 3 *   64)
#define DECODE1024(hash) DECODE0256(hash + 0 *  256) DECODE0256(hash + 1 *  256) DECODE0256(hash + 2 *  256) DECODE0256(hash + 3 *  256)
#define DECODE4096(hash) DECODE1024(hash + 0 * 1024) DECODE1024(hash + 1 * 1024) DECODE1024(hash + 2 * 1024) DECODE1024(hash + 3 * 1024)

std::array<void(ARM::*)(u32), 4096> ARM::instr_arm = { DECODE4096(0) };

#undef DECODE0001
#undef DECODE0004
#undef DECODE0016
#undef DECODE0064
#undef DECODE0256
#undef DECODE1024
#undef DECODE4096
