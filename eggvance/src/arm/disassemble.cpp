#include "disassemble.h"

#include <shell/fmt/format.h>

#include "arm/decode.h"

#define MNEMONIC "{:<8}"

static constexpr const char* kBiosFunctions[43] = {
    "SoftReset",
    "RegisterRamReset",
    "Halt",
    "Stop/Sleep",
    "IntrWait",
    "VBlankIntrWait",
    "Div",
    "DivArm",
    "Sqrt",
    "ArcTan",
    "ArcTan2",
    "CpuSet",
    "CpuFastSet",
    "GetBiosChecksum",
    "BgAffineSet",
    "ObjAffineSet",
    "BitUnPack",
    "LZ77UnCompReadNormalWrite8bit",
    "LZ77UnCompReadNormalWrite16bit",
    "HuffUnCompReadNormal",
    "RLUnCompReadNormalWrite8bit",
    "RLUnCompReadNormalWrite16bit",
    "Diff8bitUnFilterWrite8bit",
    "Diff8bitUnFilterWrite16bit",
    "Diff16bitUnFilter",
    "SoundBias",
    "SoundDriverInit",
    "SoundDriverMode",
    "SoundDriverMain",
    "SoundDriverVSync",
    "SoundChannelClear",
    "MidiKey2Freq",
    "SoundWhatever0",
    "SoundWhatever1",
    "SoundWhatever2",
    "SoundWhatever3",
    "SoundWhatever4",
    "MultiBoot",
    "HardReset",
    "CustomHalt",
    "SoundDriverVSyncOff",
    "SoundDriverVSyncOn",
    "SoundGetJumpList"
};

const char* reg(uint n)
{
    static constexpr const char* kRegs[16] = {
         "r0", "r1",  "r2",  "r3",
         "r4", "r5",  "r6",  "r7",
         "r8", "r9", "r10", "r11",
        "r12", "sp",  "lr",  "pc"
    };
    return kRegs[n];
}

const char* condition(u32 instr)
{
    static constexpr const char* kConditions[16] = {
        "eq", "ne", "cs", "cc",
        "mi", "pl", "vs", "vc",
        "hi", "ls", "ge", "lt",
        "gt", "le",   "", "nv"
    };
    return kConditions[instr >> 28];
}

std::string hex(u32 value)
{
    return fmt::format("{:X}h", value);
}

std::string rlist(u16 rlist)
{
    if (rlist == 0)
        return "{}";

    std::string list;

    list.reserve(4 * bit::popcnt(rlist) + 4);
    list.append("{");

    for (uint x : bit::iterate(rlist))
    {
        list.append(reg(x));
        list.append(",");
    }

    list.back() = '}';

    return list;
}

std::string shiftedRegister(uint data)
{
    static constexpr const char* kMnemonics[4] = {
        "lsl", "lsr", "asr", "ror"
    };

    uint rm     = bit::seq<0, 4>(data);
    uint reg_op = bit::seq<4, 1>(data);
    uint shift  = bit::seq<5, 2>(data);

    std::string offset;
    if (reg_op)
    {
        uint rs = bit::seq<8, 4>(data);
        offset = reg(rs);
    }
    else
    {
        uint amount = bit::seq<7, 5>(data);
        if (!amount)
            return reg(rm);

        offset = hex(amount);
    }
    return fmt::format("{},{} {}", reg(rm), kMnemonics[shift], offset);
}

u32 rotatedImmediate(uint data)
{
    uint value  = bit::seq<0, 8>(data);
    uint amount = bit::seq<8, 4>(data);

    return bit::ror(value, amount << 1);
}

std::string Arm_BranchExchange(u32 instr)
{
    uint rn = bit::seq<0, 4>(instr);

    const auto mnemonic = fmt::format("bx{}", condition(instr));

    return fmt::format(MNEMONIC"{}", mnemonic, reg(rn));
}

std::string Arm_BranchLink(u32 instr, u32 pc)
{
    uint offset = bit::seq< 0, 24>(instr);
    uint link   = bit::seq<24,  1>(instr);

    offset = bit::signEx<24>(offset);
    offset <<= 2;

    const auto mnemonic = fmt::format("{}{}", link ? "bl" : "b", condition(instr));

    return fmt::format(MNEMONIC"{}", mnemonic, hex(pc + offset));
}

std::string Arm_DataProcessing(u32 instr, u32 pc)
{
    static constexpr const char* kMnemonics[16] = {
        "and", "eor", "sub", "rsb",
        "add", "adc", "sbc", "rsc",
        "tst", "teq", "cmp", "cmn",
        "orr", "mov", "bic", "mvn"
    };

    uint rd     = bit::seq<12, 4>(instr);
    uint rn     = bit::seq<16, 4>(instr);
    uint flags  = bit::seq<20, 1>(instr);
    uint opcode = bit::seq<21, 4>(instr);
    uint imm_op = bit::seq<25, 1>(instr);

    std::string operand;
    if (imm_op)
    {
        u32 value = rotatedImmediate(instr);
        if (rn == 15)
        {
            if (opcode == 0b0010) value = pc - value;
            if (opcode == 0b0100) value = pc + value;
        }
        operand = hex(value);
    }
    else
    {
        operand = shiftedRegister(instr);
    }

    const auto mnemonic = fmt::format(
        "{}{}{}",
        kMnemonics[opcode],
        condition(instr),
        (flags && (opcode >> 2) != 0b10) ? "s" : "");

    switch (opcode)
    {
    case 0b0100:
    case 0b0010:
        if (rn == 15 && imm_op)
        {
            return fmt::format(
                MNEMONIC"{},={}",
                mnemonic,
                reg(rd),
                operand);
        }
        else
        {
            return fmt::format(
                MNEMONIC"{},{},{}",
                mnemonic,
                reg(rd),
                reg(rn),
                operand);
        }

    case 0b1000:
    case 0b1001:
    case 0b1010:
    case 0b1011:
        return fmt::format(
            MNEMONIC"{},{}",
            mnemonic,
            reg(rn),
            operand);

    case 0b1101:
    case 0b1111:
        return fmt::format(
            MNEMONIC"{},{}",
            mnemonic,
            reg(rd),
            operand);

    default:
        return fmt::format(
            MNEMONIC"{},{},{}",
            mnemonic,
            reg(rd),
            reg(rn),
            operand);
    }
}

std::string Arm_StatusTransfer(u32 instr)
{
    uint write = bit::seq<21, 1>(instr);
    uint spsr  = bit::seq<22, 1>(instr);

    const char* psr = spsr
        ? "spsr"
        : "cpsr";

    if (write)
    {
        uint imm_op = bit::seq<25, 1>(instr);

        std::string operand;
        if (imm_op)
        {
            operand = hex(rotatedImmediate(instr));
        }
        else
        {
            uint rm = bit::seq<0, 4>(instr);
            operand = reg(rm);
        }

        std::string fsxc;
        if (instr & (1 << 19)) fsxc.append("f");
        if (instr & (1 << 18)) fsxc.append("s");
        if (instr & (1 << 17)) fsxc.append("x");
        if (instr & (1 << 16)) fsxc.append("c");

        const auto mnemonic = fmt::format("msr{}", condition(instr));

        return fmt::format(
            MNEMONIC"{}_{},{}",
            mnemonic,
            psr,
            fsxc,
            operand);
    }
    else
    {
        uint rd = bit::seq<12, 4>(instr);

        const auto mnemonic = fmt::format("mrs{}", condition(instr));

        return fmt::format(
            MNEMONIC"{},{}",
            mnemonic,
            reg(rd),
            psr);
    }
}

std::string Arm_Multiply(u32 instr)
{
    uint rm         = bit::seq< 0, 4>(instr);
    uint rs         = bit::seq< 8, 4>(instr);
    uint rn         = bit::seq<12, 4>(instr);
    uint rd         = bit::seq<16, 4>(instr);
    uint flags      = bit::seq<20, 1>(instr);
    uint accumulate = bit::seq<21, 1>(instr);

    const auto mnemonic = fmt::format(
        "{}{}{}",
        accumulate ? "mla" : "mul",
        condition(instr),
        flags ? "s" : "");

    if (accumulate)
    {
        return fmt::format(
            MNEMONIC"{},{},{},{}",
            mnemonic,
            reg(rd),
            reg(rn),
            reg(rs),
            reg(rm));
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},{},{}",
            mnemonic,
            reg(rd),
            reg(rn),
            reg(rs));
    }
}

std::string Arm_MultiplyLong(u32 instr)
{
    static constexpr const char* kMnemonics[4] = {
        "umull", "umlal", "smull", "smlal"
    };

    uint rm     = bit::seq< 0, 4>(instr);
    uint rs     = bit::seq< 8, 4>(instr);
    uint rdl    = bit::seq<12, 4>(instr);
    uint rdh    = bit::seq<16, 4>(instr);
    uint flags  = bit::seq<20, 1>(instr);
    uint opcode = bit::seq<21, 2>(instr);

    const auto mnemonic = fmt::format(
        "{}{}{}", 
        kMnemonics[opcode],
        condition(instr),
        flags ? "s" : "");

    return fmt::format(
        MNEMONIC"{},{},{},{}",
        mnemonic,
        reg(rdl),
        reg(rdh),
        reg(rs),
        reg(rm));
}

std::string Arm_SingleDataTransfer(u32 instr)
{
    uint data      = bit::seq< 0, 12>(instr);
    uint rd        = bit::seq<12,  4>(instr);
    uint rn        = bit::seq<16,  4>(instr);
    uint load      = bit::seq<20,  1>(instr);
    uint writeback = bit::seq<21,  1>(instr);
    uint byte      = bit::seq<22,  1>(instr);
    uint increment = bit::seq<23,  1>(instr);
    uint pre_index = bit::seq<24,  1>(instr);
    uint imm_op    = bit::seq<25,  1>(instr);

    std::string offset = imm_op
        ? shiftedRegister(data)
        : hex(data);

    const auto mnemonic = fmt::format(
        "{}{}{}",
        load ? "ldr" : "str",
        condition(instr),
        byte ? "b" : "");

    if (pre_index)
    {
        return fmt::format(
            MNEMONIC"{},[{},{}{}]{}",
            mnemonic,
            reg(rd),
            reg(rn),
            increment ? "" : "-",
            offset,
            writeback ? "!" : "");
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},[{}],{}{}",
            mnemonic,
            reg(rd),
            reg(rn),
            increment ? "" : "-",
            offset);
    }
}

std::string Arm_HalfSignedDataTransfer(u32 instr)
{
    uint half      = bit::seq< 5, 1>(instr);
    uint sign      = bit::seq< 6, 1>(instr);
    uint rd        = bit::seq<12, 4>(instr);
    uint rn        = bit::seq<16, 4>(instr);
    uint load      = bit::seq<20, 1>(instr);
    uint writeback = bit::seq<21, 1>(instr);
    uint imm_op    = bit::seq<22, 1>(instr);
    uint increment = bit::seq<23, 1>(instr);
    uint pre_index = bit::seq<24, 1>(instr);

    std::string offset;
    if (imm_op)
    {
        uint lower = bit::seq<0, 4>(instr);
        uint upper = bit::seq<8, 4>(instr);
        offset = hex((upper << 4) | lower);
    }
    else
    {
        uint rm = bit::seq<0, 4>(instr);
        offset = reg(rm);
    }

    const auto mnemonic = fmt::format(
        "{}{}{}{}",
        load ? "ldr" : "str",
        condition(instr),
        sign ? "s" : "",
        half ? "h" : "b");

    if (pre_index)
    {
        return fmt::format(
            MNEMONIC"{},[{},{}{}]{}",
            mnemonic,
            reg(rd),
            reg(rn),
            increment ? "" : "-",
            offset,
            writeback ? "!" : "");
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},[{}{}],{}",
            mnemonic,
            reg(rd),
            reg(rn),
            increment ? "" : "-",
            offset);
    }
}

std::string Arm_BlockDataTransfer(u32 instr)
{
    static constexpr const char* kSuffixes[2][4] = {
        { "ed", "ea", "fd", "fa" },
        { "fa", "fd", "ea", "ed" }
    };

    uint rlist     = bit::seq< 0, 16>(instr);
    uint rn        = bit::seq<16,  4>(instr);
    uint load      = bit::seq<20,  1>(instr);
    uint writeback = bit::seq<21,  1>(instr);
    uint user_mode = bit::seq<22,  1>(instr);
    uint opcode    = bit::seq<23,  2>(instr);

    const auto mnemonic = fmt::format(
        "{}{}{}",
        load ? "ldm" : "stm",
        condition(instr),
        kSuffixes[load][opcode]);

    return fmt::format(
        MNEMONIC"{}{},{}{}",
        mnemonic,
        reg(rn),
        writeback ? "!" : "",
        ::rlist(rlist),
        user_mode ? "^" : "");
}

std::string Arm_SingleDataSwap(u32 instr)
{
    uint rm   = bit::seq< 0, 4>(instr);
    uint rd   = bit::seq<12, 4>(instr);
    uint rn   = bit::seq<16, 4>(instr);
    uint byte = bit::seq<22, 1>(instr);

    const auto mnemonic = fmt::format(
        "swp{}{}",
        condition(instr),
        byte ? "b" : "");

    return fmt::format(
        MNEMONIC"{},{},[{}]",
        mnemonic,
        reg(rd),
        reg(rm),
        reg(rn));
}

std::string Arm_SoftwareInterrupt(u32 instr)
{
    uint comment = bit::seq<16, 8>(instr);

    const char* func = comment < 43
        ? kBiosFunctions[comment]
        : "unknown";

    return fmt::format(MNEMONIC"{:X} - {}", "swi", comment, func);
}

std::string Thumb_MoveShiftedRegister(u16 instr)
{
    static constexpr const char* kMnemonics[4] = {
        "lsl", "lsr", "asr", "???"
    };

    uint rd     = bit::seq< 0, 3>(instr);
    uint rs     = bit::seq< 3, 3>(instr);
    uint amount = bit::seq< 6, 5>(instr);
    uint opcode = bit::seq<11, 2>(instr);

    return fmt::format(
        MNEMONIC"{},{},{}",
        kMnemonics[opcode],
        reg(rd),
        reg(rs),
        hex(amount));
}

std::string Thumb_AddSubtract(u16 instr)
{
    uint rd     = bit::seq< 0, 3>(instr);
    uint rs     = bit::seq< 3, 3>(instr);
    uint rn     = bit::seq< 6, 3>(instr);
    uint sub    = bit::seq< 9, 1>(instr);
    uint imm_op = bit::seq<10, 1>(instr);

    if (imm_op && rn == 0)
    {
        return fmt::format(
            MNEMONIC"{},{}",
            "mov",
            reg(rd),
            reg(rs));
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},{},{}",
            sub ? "sub" : "add",
            reg(rd),
            reg(rs),
            imm_op ? hex(rn) : reg(rn));
    }
}

std::string Thumb_ImmediateOperations(u16 instr)
{
    static constexpr const char* kMnemonics[4] = {
        "mov", "cmp", "add", "sub"
    };

    uint amount = bit::seq< 0, 8>(instr);
    uint rd     = bit::seq< 8, 3>(instr);
    uint opcode = bit::seq<11, 2>(instr);

    return fmt::format(
        MNEMONIC"{},{}",
        kMnemonics[opcode],
        reg(rd),
        hex(amount));
}

std::string Thumb_AluOperations(u16 instr)
{
    static constexpr const char* kMnemonics[16] = {
        "and", "eor", "lsl", "lsr",
        "asr", "adc", "sbc", "ror",
        "tst", "neg", "cmp", "cmn",
        "orr", "mul", "bic", "mvn"
    };

    uint rd     = bit::seq<0, 3>(instr);
    uint rs     = bit::seq<3, 3>(instr);
    uint opcode = bit::seq<6, 4>(instr);

    return fmt::format(
        MNEMONIC"{},{}",
        kMnemonics[opcode],
        reg(rd),
        reg(rs));
}

std::string Thumb_HighRegisterOperations(u16 instr)
{
    static constexpr const char* kMnemonics[4] = {
        "add", "cmp", "mov", "bx"
    };

    uint rd     = bit::seq<0, 3>(instr);
    uint rs     = bit::seq<3, 3>(instr);
    uint hs     = bit::seq<6, 1>(instr);
    uint hd     = bit::seq<7, 1>(instr);
    uint opcode = bit::seq<8, 2>(instr);

    rs |= hs << 3;
    rd |= hd << 3;

    return opcode == 0b11
        ? fmt::format(MNEMONIC"{}", kMnemonics[opcode], reg(rs))
        : fmt::format(MNEMONIC"{},{}", kMnemonics[opcode], reg(rd), reg(rs));
}

std::string Thumb_LoadPcRelative(u16 instr, u32 pc)
{
    uint offset = bit::seq<0, 8>(instr);
    uint rd     = bit::seq<8, 3>(instr);

    return fmt::format(
        MNEMONIC"{},[{}]",
        "ldr",
        reg(rd),
        hex((pc & ~0x3) + (offset<< 2)));
}

std::string Thumb_LoadStoreRegisterOffset(u16 instr)
{
    static constexpr const char* kMnemonics[4] = {
        "str", "strb", "ldr", "ldrb"
    };

    uint rd     = bit::seq< 0, 3>(instr);
    uint rb     = bit::seq< 3, 3>(instr);
    uint ro     = bit::seq< 6, 3>(instr);
    uint opcode = bit::seq<10, 2>(instr);

    return fmt::format(
        MNEMONIC"{},[{},{}]",
        kMnemonics[opcode],
        reg(rd),
        reg(rb),
        reg(ro));
}

std::string Thumb_LoadStoreByteHalf(u16 instr)
{
    static constexpr const char* kMnemonics[4] = {
        "strh", "ldrsb", "ldrh", "ldrsh"
    };

    uint rd     = bit::seq< 0, 3>(instr);
    uint rb     = bit::seq< 3, 3>(instr);
    uint ro     = bit::seq< 6, 3>(instr);
    uint opcode = bit::seq<10, 2>(instr);

    return fmt::format(
        MNEMONIC"{},[{},{}]",
        kMnemonics[opcode],
        reg(rd),
        reg(rb),
        reg(ro));
}

std::string Thumb_LoadStoreImmediateOffset(u16 instr)
{
    static constexpr const char* kMnemonics[4] = {
        "str", "ldr", "strb", "ldrb"
    };

    uint rd     = bit::seq< 0, 3>(instr);
    uint rb     = bit::seq< 3, 3>(instr);
    uint offset = bit::seq< 6, 5>(instr);
    uint opcode = bit::seq<11, 2>(instr);

    offset <<= ~opcode & 0x2;

    return fmt::format(
        MNEMONIC"{},[{},{}]",
        kMnemonics[opcode],
        reg(rd),
        reg(rb),
        hex(offset));
}

std::string Thumb_LoadStoreHalf(u16 instr)
{
    uint rd     = bit::seq< 0, 3>(instr);
    uint rb     = bit::seq< 3, 3>(instr);
    uint offset = bit::seq< 6, 5>(instr);
    uint load   = bit::seq<11, 1>(instr);

    offset <<= 1;

    return fmt::format(
        MNEMONIC"{},[{},{}]",
        load ? "ldrh" : "strh",
        reg(rd),
        reg(rb),
        hex(offset));
}

std::string Thumb_LoadStoreSpRelative(u16 instr)
{
    uint offset = bit::seq< 0, 8>(instr);
    uint rd     = bit::seq< 8, 3>(instr);
    uint load   = bit::seq<11, 1>(instr);

    offset <<= 2;

    return fmt::format(
        MNEMONIC"{},[sp,{}]",
        load ? "ldr" : "str",
        reg(rd),
        hex(offset));
}

std::string Thumb_LoadRelativeAddress(u16 instr, u32 pc)
{
    uint offset = bit::seq< 0, 8>(instr);
    uint rd     = bit::seq< 8, 3>(instr);
    uint sp     = bit::seq<11, 1>(instr);

    offset <<= 2;

    if (sp)
    {
        return fmt::format(
            MNEMONIC"{},sp,{}",
            "add",
            reg(rd), 
            hex(offset));
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},={}",
            "add",
            reg(rd),
            hex((pc & ~0x3) + offset));
    }
}

std::string Thumb_AddOffsetSp(u16 instr)
{
    uint offset = bit::seq<0, 7>(instr);
    uint sign   = bit::seq<7, 1>(instr);

    offset <<= 2;

    return fmt::format(
        MNEMONIC"sp,{}{}",
        "add",
        sign ? "-" : "",
        hex(offset));
}

std::string Thumb_PushPopRegisters(u16 instr)
{
    uint rlist = bit::seq< 0, 8>(instr);
    uint rbit  = bit::seq< 8, 1>(instr);
    uint pop   = bit::seq<11, 1>(instr);

    rlist |= rbit << (pop ? 15 : 14);

    return fmt::format(
        MNEMONIC"{}",
        pop ? "pop" : "push",
        ::rlist(rlist));
}

std::string Thumb_LoadStoreMultiple(u16 instr)
{
    uint rlist = bit::seq< 0, 8>(instr);
    uint rb    = bit::seq< 8, 3>(instr);
    uint load  = bit::seq<11, 1>(instr);

    return fmt::format(
        MNEMONIC"{}!,{}",
        load ? "ldmia" : "stmia",
        reg(rb),
        ::rlist(rlist));
}

std::string Thumb_ConditionalBranch(u16 instr, u32 pc)
{
    static constexpr const char* kMnemonics[16] = {
        "beq", "bne", "bcs", "bcc",
        "bmi", "bpl", "bvs", "bvc",
        "bhi", "bls", "bge", "blt",
        "bgt", "ble", "b",   "b??"
    };

    uint offset    = bit::seq<0, 8>(instr);
    uint condition = bit::seq<8, 4>(instr);

    offset = bit::signEx<8>(offset);
    offset <<= 1;

    return fmt::format(
        MNEMONIC"{}",
        kMnemonics[condition],
        hex(pc + offset));
}

std::string Thumb_SoftwareInterrupt(u16 instr)
{
    uint comment = bit::seq<0, 8>(instr);

    const char* func = comment < 43
        ? kBiosFunctions[comment]
        : "unknown";

    return fmt::format(MNEMONIC"{:X} - {}", "swi", comment, func);

}

std::string Thumb_UnconditionalBranch(u16 instr, u32 pc)
{
    uint offset = bit::seq<0, 11>(instr);

    offset = bit::signEx<11>(offset);
    offset <<= 1;

    return fmt::format(MNEMONIC"{}", "b", hex(pc + offset));
}

std::string Thumb_LongBranchLink(u16 instr, u32 lr)
{
    uint offset = bit::seq< 0, 11>(instr);
    uint second = bit::seq<11,  1>(instr);

    offset <<= 1;

    return fmt::format(
        MNEMONIC"{}",
        "bl",
        second ? hex(lr + offset) : "<setup>");
}

std::string disassemble(u32 instr, u32 pc)
{
    switch (decodeArm(hashArm(instr)))
    {
    case InstructionArm::BranchExchange:         return Arm_BranchExchange(instr);
    case InstructionArm::BranchLink:             return Arm_BranchLink(instr, pc);
    case InstructionArm::DataProcessing:         return Arm_DataProcessing(instr, pc);
    case InstructionArm::StatusTransfer:         return Arm_StatusTransfer(instr);
    case InstructionArm::Multiply:               return Arm_Multiply(instr);
    case InstructionArm::MultiplyLong:           return Arm_MultiplyLong(instr);
    case InstructionArm::SingleDataTransfer:     return Arm_SingleDataTransfer(instr);
    case InstructionArm::HalfSignedDataTransfer: return Arm_HalfSignedDataTransfer(instr);
    case InstructionArm::BlockDataTransfer:      return Arm_BlockDataTransfer(instr);
    case InstructionArm::SingleDataSwap:         return Arm_SingleDataSwap(instr);
    case InstructionArm::SoftwareInterrupt:      return Arm_SoftwareInterrupt(instr);
    }
    return "undef";
}

std::string disassemble(u16 instr, u32 pc, u32 lr)
{
    switch (decodeThumb(hashThumb(instr)))
    {
    case InstructionThumb::MoveShiftedRegister:      return Thumb_MoveShiftedRegister(instr);
    case InstructionThumb::AddSubtract:              return Thumb_AddSubtract(instr);
    case InstructionThumb::ImmediateOperations:      return Thumb_ImmediateOperations(instr);
    case InstructionThumb::AluOperations:            return Thumb_AluOperations(instr);
    case InstructionThumb::HighRegisterOperations:   return Thumb_HighRegisterOperations(instr);
    case InstructionThumb::LoadPcRelative:           return Thumb_LoadPcRelative(instr, pc);
    case InstructionThumb::LoadStoreRegisterOffset:  return Thumb_LoadStoreRegisterOffset(instr);
    case InstructionThumb::LoadStoreByteHalf:        return Thumb_LoadStoreByteHalf(instr);
    case InstructionThumb::LoadStoreImmediateOffset: return Thumb_LoadStoreImmediateOffset(instr);
    case InstructionThumb::LoadStoreHalf:            return Thumb_LoadStoreHalf(instr);
    case InstructionThumb::LoadStoreSpRelative:      return Thumb_LoadStoreSpRelative(instr);
    case InstructionThumb::LoadRelativeAddress:      return Thumb_LoadRelativeAddress(instr, pc);
    case InstructionThumb::AddOffsetSp:              return Thumb_AddOffsetSp(instr);
    case InstructionThumb::PushPopRegisters:         return Thumb_PushPopRegisters(instr);
    case InstructionThumb::LoadStoreMultiple:        return Thumb_LoadStoreMultiple(instr);
    case InstructionThumb::ConditionalBranch:        return Thumb_ConditionalBranch(instr, pc);
    case InstructionThumb::SoftwareInterrupt:        return Thumb_SoftwareInterrupt(instr);
    case InstructionThumb::UnconditionalBranch:      return Thumb_UnconditionalBranch(instr, pc);
    case InstructionThumb::LongBranchLink:           return Thumb_LongBranchLink(instr, lr);
    }
    return "undef";
}
