#include "arm.h"

#include "arm/decode.h"

template<uint Instr>
void Arm::Arm_BranchExchange(u32 instr)
{
    uint rn = bit::seq<0, 4>(instr);

    pc = regs[rn];

    if ((cpsr.t = pc & 0x1))
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
void Arm::Arm_BranchLink(u32 instr)
{
    constexpr uint kLink = bit::seq<24, 1>(Instr);

    uint offset = bit::seq<0, 24>(instr);

    offset = bit::signEx<24>(offset);
    offset <<= 2;

    if (kLink) lr = pc - 4;

    pc += offset;
    flushWord();
}

template<uint Instr>
void Arm::Arm_DataProcessing(u32 instr)
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

    constexpr uint kFlags  = bit::seq<20, 1>(Instr);
    constexpr uint kOpcode = bit::seq<21, 4>(Instr);
    constexpr uint kImmOp  = bit::seq<25, 1>(Instr);

    uint rd    = bit::seq<12, 4>(instr);
    uint rn    = bit::seq<16, 4>(instr);
    uint flags = kFlags && rd != 15;

    u32& dst = regs[rd];
    u32  op1 = regs[rn];
    u32  op2;

    constexpr bool kLogical = 
           kOpcode == kOpcodeAdd 
        || kOpcode == kOpcodeEor
        || kOpcode == kOpcodeOrr
        || kOpcode == kOpcodeMov
        || kOpcode == kOpcodeBic
        || kOpcode == kOpcodeMvn
        || kOpcode == kOpcodeTst
        || kOpcode == kOpcodeTeq;

    if (kImmOp)
    {
        uint value  = bit::seq<0, 8>(instr);
        uint amount = bit::seq<8, 4>(instr);
        op2 = ror<false>(value, amount << 1, flags && kLogical);
    }
    else
    {
        uint rm     = bit::seq<0, 4>(instr);
        uint reg_op = bit::seq<4, 1>(instr);
        uint shift  = bit::seq<5, 2>(instr);

        op2 = regs[rm];

        if (reg_op)
        {
            uint rs = bit::seq<8, 4>(instr);

            if (rn == 15) op1 += 4;
            if (rm == 15) op2 += 4;

            uint amount = regs[rs] & 0xFF;

            switch (shift)
            {
            case kShiftLsl: op2 = lsl<false>(op2, amount, flags && kLogical); break;
            case kShiftLsr: op2 = lsr<false>(op2, amount, flags && kLogical); break;
            case kShiftAsr: op2 = asr<false>(op2, amount, flags && kLogical); break;
            case kShiftRor: op2 = ror<false>(op2, amount, flags && kLogical); break;

            default:
                SHELL_UNREACHABLE;
                break;
            }
            idle();
        }
        else
        {
            uint amount = bit::seq<7, 5>(instr);

            switch (shift)
            {
            case kShiftLsl: op2 = lsl<true>(op2, amount, flags && kLogical); break;
            case kShiftLsr: op2 = lsr<true>(op2, amount, flags && kLogical); break;
            case kShiftAsr: op2 = asr<true>(op2, amount, flags && kLogical); break;
            case kShiftRor: op2 = ror<true>(op2, amount, flags && kLogical); break;

            default:
                SHELL_UNREACHABLE;
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
        SHELL_UNREACHABLE;
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
void Arm::Arm_StatusTransfer(u32 instr)
{
    constexpr uint kWrite = bit::seq<21, 1>(Instr);
    constexpr uint kSpsr  = bit::seq<22, 1>(Instr);
    constexpr uint kImmOp = bit::seq<25, 1>(Instr);

    if (kWrite)
    {
        u32 op;
        if (kImmOp)
        {
            uint value  = bit::seq<0, 8>(instr);
            uint amount = bit::seq<8, 4>(instr);
            op = bit::ror(value, amount << 1);
        }
        else
        {
            uint rm = bit::seq<0, 4>(instr);
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
        uint rd = bit::seq<12, 4>(instr);
        regs[rd] = kSpsr ? spsr : cpsr;
    }
}

template<uint Instr>
void Arm::Arm_Multiply(u32 instr)
{
    constexpr uint kFlags      = bit::seq<20, 1>(Instr);
    constexpr uint kAccumulate = bit::seq<21, 1>(Instr);

    uint rm = bit::seq< 0, 4>(instr);
    uint rs = bit::seq< 8, 4>(instr);
    uint rn = bit::seq<12, 4>(instr);
    uint rd = bit::seq<16, 4>(instr);

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

    booth<true>(op2);
}

template<uint Instr>
void Arm::Arm_MultiplyLong(u32 instr)
{
    constexpr uint kFlags      = bit::seq<20, 1>(Instr);
    constexpr uint kAccumulate = bit::seq<21, 1>(Instr);
    constexpr uint kSign       = bit::seq<22, 1>(Instr);

    uint rm  = bit::seq< 0, 4>(instr);
    uint rs  = bit::seq< 8, 4>(instr);
    uint rdl = bit::seq<12, 4>(instr);
    uint rdh = bit::seq<16, 4>(instr);

    u64  op1  = regs[rm];
    u64  op2  = regs[rs];
    u32& dstl = regs[rdl];
    u32& dsth = regs[rdh];

    if (kSign)
    {
        op1 = bit::signEx<32>(op1);
        op2 = bit::signEx<32>(op2);
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

    booth<kSign>(static_cast<u32>(op2));
}

template<uint Instr>
void Arm::Arm_SingleDataTransfer(u32 instr)
{
    constexpr uint kLoad      = bit::seq<20, 1>(Instr);
    constexpr uint kWriteback = bit::seq<21, 1>(Instr);
    constexpr uint kByte      = bit::seq<22, 1>(Instr);
    constexpr uint kIncrement = bit::seq<23, 1>(Instr);
    constexpr uint kPreIndex  = bit::seq<24, 1>(Instr);
    constexpr uint kRegOp     = bit::seq<25, 1>(Instr);

    uint rd = bit::seq<12, 4>(instr);
    uint rn = bit::seq<16, 4>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rn];

    u32 offset;
    if (kRegOp)
    {
        uint rm     = bit::seq<0, 4>(instr);
        uint reg_op = bit::seq<4, 1>(instr);
        uint shift  = bit::seq<5, 2>(instr);

        uint amount;
        if (reg_op)
        {
            uint rs = bit::seq<8, 4>(instr);
            amount = regs[rs] & 0xFF;
        }
        else
        {
            amount = bit::seq<7, 5>(instr);
        }

        switch (shift)
        {
        case kShiftLsl: offset = lsl<true>(regs[rm], amount, false); break;
        case kShiftLsr: offset = lsr<true>(regs[rm], amount, false); break;
        case kShiftAsr: offset = asr<true>(regs[rm], amount, false); break;
        case kShiftRor: offset = ror<true>(regs[rm], amount, false); break;

        default:
            SHELL_UNREACHABLE;
            break;
        }
    }
    else
    {
        offset = bit::seq<0, 12>(instr);
    }

    if (!kIncrement)
        offset = -static_cast<int>(offset);

    if (kPreIndex)
        addr += offset;

    if (kLoad)
    {
        dst = kByte
            ? readByte(addr)
            : readWordRotate(addr);

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
void Arm::Arm_HalfSignedDataTransfer(u32 instr)
{
    enum Opcode
    {
        kOpcodeSwap,
        kOpcodeLdrh,
        kOpcodeLdrsb,
        kOpcodeLdrsh
    };

    constexpr uint kOpcode    = bit::seq< 5, 2>(Instr);
    constexpr uint kLoad      = bit::seq<20, 1>(Instr);
    constexpr uint kWriteback = bit::seq<21, 1>(Instr);
    constexpr uint kImmOp     = bit::seq<22, 1>(Instr);
    constexpr uint kIncrement = bit::seq<23, 1>(Instr);
    constexpr uint kPreIndex  = bit::seq<24, 1>(Instr);

    uint rd = bit::seq<12, 4>(instr);
    uint rn = bit::seq<16, 4>(instr);

    u32& dst = regs[rd];
    u32 addr = regs[rn];

    u32 offset;
    if (kImmOp)
    {
        uint lower = bit::seq<0, 4>(instr);
        uint upper = bit::seq<8, 4>(instr);
        offset = (upper << 4) | lower;
    }
    else
    {
        uint rm = bit::seq<0, 4>(instr);
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
            dst = readHalfRotate(addr);
            break;

        case kOpcodeLdrsb:
            dst = readByte(addr);
            dst = bit::signEx<8>(dst);
            break;

        case kOpcodeLdrsh:
            dst = readHalfSignEx(addr);
            break;

        default:
            SHELL_UNREACHABLE;
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
void Arm::Arm_BlockDataTransfer(u32 instr)
{
    constexpr uint kLoad      = bit::seq<20, 1>(Instr);
    constexpr uint kUserMode  = bit::seq<22, 1>(Instr);
    constexpr uint kIncrement = bit::seq<23, 1>(Instr);

    uint rlist     = bit::seq< 0, 16>(instr);
    uint rn        = bit::seq<16,  4>(instr);
    uint writeback = bit::seq<21,  1>(Instr);
    uint pre_index = bit::seq<24,  1>(Instr);

    u32 addr = regs[rn];
    u32 base = regs[rn];

    uint mode = cpsr.m;
    if (kUserMode)
        switchMode(PSR::kModeUsr);

    if (rlist != 0)
    {
        if (!kIncrement)
        {
            addr -= 4 * bit::popcnt(rlist);
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

            for (uint x : bit::iterateBits(rlist))
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
            for (uint x : bit::iterateBits(rlist))
            {
                u32 value = x != rn
                    ? x != 15
                        ? regs[x] + 0
                        : regs[x] + 4
                    : x == bit::ctz(rlist)
                        ? base
                        : base + (kIncrement ? 4 : -4) * bit::popcnt(rlist);

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
                SHELL_UNREACHABLE;
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
void Arm::Arm_SingleDataSwap(u32 instr)
{
    constexpr uint kByte = bit::seq<22, 1>(Instr);

    uint rm = bit::seq< 0, 4>(instr);
    uint rd = bit::seq<12, 4>(instr);
    uint rn = bit::seq<16, 4>(instr);

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
        dst = readWordRotate(addr);
        writeWord(addr, src);
    }
    idle();
}

template<uint Instr>
void Arm::Arm_SoftwareInterrupt(u32 instr)
{
    interruptSw();
}

template<uint Instr>
void Arm::Arm_CoprocessorDataOperations(u32 instr)
{
    SHELL_ASSERT(false, SHELL_FUNCTION);
}

template<uint Instr>
void Arm::Arm_CoprocessorDataTransfers(u32 instr)
{
    SHELL_ASSERT(false, SHELL_FUNCTION);
}

template<uint Instr>
void Arm::Arm_CoprocessorRegisterTransfers(u32 instr)
{
    SHELL_ASSERT(false, SHELL_FUNCTION);
}

template<uint Instr>
void Arm::Arm_Undefined(u32 instr)
{
    SHELL_ASSERT(false, SHELL_FUNCTION);
}

template<uint Hash>
constexpr Arm::Instruction32 Arm::Arm_Decode()
{
    constexpr auto kDehash = dehashArm(Hash);
    constexpr auto kDecode = decodeArm(Hash);

    if constexpr (kDecode == InstructionArm::BranchExchange)               return &Arm::Arm_BranchExchange<kDehash>;
    if constexpr (kDecode == InstructionArm::BranchLink)                   return &Arm::Arm_BranchLink<kDehash>;
    if constexpr (kDecode == InstructionArm::DataProcessing)               return &Arm::Arm_DataProcessing<kDehash>;
    if constexpr (kDecode == InstructionArm::StatusTransfer)               return &Arm::Arm_StatusTransfer<kDehash>;
    if constexpr (kDecode == InstructionArm::Multiply)                     return &Arm::Arm_Multiply<kDehash>;
    if constexpr (kDecode == InstructionArm::MultiplyLong)                 return &Arm::Arm_MultiplyLong<kDehash>;
    if constexpr (kDecode == InstructionArm::SingleDataTransfer)           return &Arm::Arm_SingleDataTransfer<kDehash>;
    if constexpr (kDecode == InstructionArm::HalfSignedDataTransfer)       return &Arm::Arm_HalfSignedDataTransfer<kDehash>;
    if constexpr (kDecode == InstructionArm::BlockDataTransfer)            return &Arm::Arm_BlockDataTransfer<kDehash>;
    if constexpr (kDecode == InstructionArm::SingleDataSwap)               return &Arm::Arm_SingleDataSwap<kDehash>;
    if constexpr (kDecode == InstructionArm::SoftwareInterrupt)            return &Arm::Arm_SoftwareInterrupt<kDehash>;
    if constexpr (kDecode == InstructionArm::CoprocessorDataOperations)    return &Arm::Arm_CoprocessorDataOperations<kDehash>;
    if constexpr (kDecode == InstructionArm::CoprocessorDataTransfers)     return &Arm::Arm_CoprocessorDataTransfers<kDehash>;
    if constexpr (kDecode == InstructionArm::CoprocessorRegisterTransfers) return &Arm::Arm_CoprocessorRegisterTransfers<kDehash>;
    if constexpr (kDecode == InstructionArm::Undefined)                    return &Arm::Arm_Undefined<kDehash>;
}

#define DECODE0001(hash) Arm_Decode<hash>(),
#define DECODE0004(hash) DECODE0001(hash + 0 *    1) DECODE0001(hash + 1 *    1) DECODE0001(hash + 2 *    1) DECODE0001(hash + 3 *    1)
#define DECODE0016(hash) DECODE0004(hash + 0 *    4) DECODE0004(hash + 1 *    4) DECODE0004(hash + 2 *    4) DECODE0004(hash + 3 *    4)
#define DECODE0064(hash) DECODE0016(hash + 0 *   16) DECODE0016(hash + 1 *   16) DECODE0016(hash + 2 *   16) DECODE0016(hash + 3 *   16)
#define DECODE0256(hash) DECODE0064(hash + 0 *   64) DECODE0064(hash + 1 *   64) DECODE0064(hash + 2 *   64) DECODE0064(hash + 3 *   64)
#define DECODE1024(hash) DECODE0256(hash + 0 *  256) DECODE0256(hash + 1 *  256) DECODE0256(hash + 2 *  256) DECODE0256(hash + 3 *  256)
#define DECODE4096(hash) DECODE1024(hash + 0 * 1024) DECODE1024(hash + 1 * 1024) DECODE1024(hash + 2 * 1024) DECODE1024(hash + 3 * 1024)

const std::array<Arm::Instruction32, 4096> Arm::instr_arm = { DECODE4096(0) };

#undef DECODE0001
#undef DECODE0004
#undef DECODE0016
#undef DECODE0064
#undef DECODE0256
#undef DECODE1024
#undef DECODE4096
