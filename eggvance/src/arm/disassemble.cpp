#include "disassemble.h"

#include <fmt/format.h>

#include "common/utility.h"
#include "decode.h"

#define MNEMONIC "{:<8}"

static constexpr const char* bios_funcs[43] = {
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

const char* reg(int id)
{
    static constexpr const char* regs[16] = {
        "r0",  "r1",  "r2",  "r3",
        "r4",  "r5",  "r6",  "r7",
        "r8",  "r9",  "r10", "r11",
        "r12", "sp",  "lr",  "pc"
    };
    return regs[id];
}

const std::string hex(u32 value)
{
    return fmt::format("{:X}h", value);
}

const std::string list(u16 rlist)
{
    std::string result;

    result.reserve(4 * bitCount(rlist) + 4);

    int beg = bitScanForward(rlist);
    int end = bitScanReverse(rlist);

    result.append("{");
    for (int x = beg; x <= end; ++x)
    {
        if (rlist & (1 << x))
        {
            result.append(reg(x));
            result.append((x != end) ? "," : "");
        }
    }
    result.append("}");

    return result;
}

const char* condition(u32 instr)
{
    static constexpr const char* conditions[16] = {
        "eq", "ne", "cs", "cc",
        "mi", "pl", "vs", "vc",
        "hi", "ls", "ge", "lt",
        "gt", "le", "",   "nv"
    };
    return conditions[instr >> 28];
}

const std::string shiftedRegister(int data)
{
    int rm      = bits<0, 4>(data);
    int use_reg = bits<4, 1>(data);
    int type    = bits<5, 2>(data);

    static constexpr const char* types[4] = {
        "lsl", "lsr", "asr", "ror"
    };

    std::string offset;
    if (use_reg)
    {
        int rs = bits<8, 4>(data);
        offset = reg(rs);
    }
    else
    {
        int amount = bits<7, 5>(data);
        if (amount == 0)
            return reg(rm);
        offset = hex(amount);
    }
    return fmt::format("{},{} {}", reg(rm), types[type], offset);
}

u32 rotatedImmediate(int data)
{
    u32 value  = bits<0, 8>(data);
    int amount = bits<8, 4>(data);

    return rotateRight(value, amount << 1);
}

const std::string Arm_BranchExchange(u32 instr)
{
    int rn = bits<0, 4>(instr);

    auto mnemonic = fmt::format("bx{}", condition(instr));

    return fmt::format(MNEMONIC"{}", mnemonic, reg(rn));
}

const std::string Arm_BranchLink(u32 instr, u32 pc)
{
    int offset = bits< 0, 24>(instr);
    int link   = bits<24,  1>(instr);

    offset = signExtend<24>(offset);
    offset <<= 2;

    auto mnemonic = fmt::format("{}{}", link ? "bl" : "b", condition(instr));

    return fmt::format(MNEMONIC"{}", mnemonic, hex(pc + offset));
}

const std::string Arm_DataProcessing(u32 instr, u32 pc)
{
    int rd     = bits<12, 4>(instr);
    int rn     = bits<16, 4>(instr);
    int flags  = bits<20, 1>(instr);
    int opcode = bits<21, 4>(instr);
    int imm_op = bits<25, 1>(instr);

    static constexpr const char* mnemonics[16] = {
        "and", "eor", "sub", "rsb",
        "add", "adc", "sbc", "rsc",
        "tst", "teq", "cmp", "cmn",
        "orr", "mov", "bic", "mvn"
    };

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

    auto mnemonic = fmt::format(
        "{}{}{}",
        mnemonics[opcode],
        condition(instr),
        (flags && (opcode >> 2) != 0b10) ? "s" : ""
    );

    switch (opcode)
    {
    case 0b0010:
    case 0b0100:
        if (rn == 15 && imm_op)
        {
            return fmt::format(
                MNEMONIC"{},={}",
                mnemonic,
                reg(rd),
                operand
            );
        }
        else
        {
            return fmt::format(
                MNEMONIC"{},{},{}",
                mnemonic,
                reg(rd),
                reg(rn),
                operand
            );
        }

    case 0b1000:
    case 0b1001:
    case 0b1010:
    case 0b1011:
        return fmt::format(
            MNEMONIC"{},{}",
            mnemonic,
            reg(rn),
            operand
        );

    case 0b1101:
    case 0b1111:
        return fmt::format(
            MNEMONIC"{},{}",
            mnemonic,
            reg(rd),
            operand
        );

    default:
        return fmt::format(
            MNEMONIC"{},{},{}",
            mnemonic,
            reg(rd),
            reg(rn),
            operand
        );
    }
}

const std::string Arm_StatusTransfer(u32 instr)
{
    int write    = bits<21, 1>(instr);
    int use_spsr = bits<22, 1>(instr);

    const char* psr = use_spsr ? "spsr" : "cpsr";

    if (write)
    {
        int imm_op = bits<25,  1>(instr);

        std::string operand;
        if (imm_op)
        {
            operand = hex(rotatedImmediate(instr));
        }
        else
        {
            int rm = bits<0, 4>(instr);
            operand = reg(rm);
        }

        std::string fsxc;
        fsxc.reserve(5);

        if (instr & (1 << 19)) fsxc.append("f");
        if (instr & (1 << 18)) fsxc.append("s");
        if (instr & (1 << 17)) fsxc.append("x");
        if (instr & (1 << 16)) fsxc.append("c");

        auto mnemonic = fmt::format("msr{}", condition(instr));

        return fmt::format(MNEMONIC"{}_{},{}", mnemonic, psr, fsxc, operand);
    }
    else
    {
        int rd = bits<12, 4>(instr);

        auto mnemonic = fmt::format("mrs{}", condition(instr));

        return fmt::format(MNEMONIC"{},{}", mnemonic, reg(rd), psr);
    }
}

const std::string Arm_Multiply(u32 instr)
{
    int rm         = bits< 0, 4>(instr);
    int rs         = bits< 8, 4>(instr);
    int rn         = bits<12, 4>(instr);
    int rd         = bits<16, 4>(instr);
    int flags      = bits<20, 1>(instr);
    int accumulate = bits<21, 1>(instr);

    auto mnemonic = fmt::format(
        "{}{}{}",
        accumulate ? "mla" : "mul",
        condition(instr),
        flags ? "s" : ""
    );

    if (accumulate)
    {
        return fmt::format(
            MNEMONIC"{},{},{},{}",
            mnemonic,
            reg(rd),
            reg(rn),
            reg(rs),
            reg(rm)
        );
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},{},{}",
            mnemonic,
            reg(rd),
            reg(rn),
            reg(rs)
        );
    }
}

const std::string Arm_MultiplyLong(u32 instr)
{
    int rm     = bits< 0, 4>(instr);
    int rs     = bits< 8, 4>(instr);
    int rdl    = bits<12, 4>(instr);
    int rdh    = bits<16, 4>(instr);
    int flags  = bits<20, 1>(instr);
    int opcode = bits<21, 2>(instr);

    static constexpr const char* mnemonics[4] = {
        "umull", "umlal", "smull", "smlal"
    };

    auto mnemonic = fmt::format(
        "{}{}{}", 
        mnemonics[opcode],
        condition(instr),
        flags ? "s" : ""
    );

    return fmt::format(
        MNEMONIC"{},{},{},{}",
        mnemonic,
        reg(rdl),
        reg(rdh),
        reg(rs),
        reg(rm)
    );
}

const std::string Arm_SingleDataTransfer(u32 instr)
{
    int data       = bits< 0, 12>(instr);
    int rd         = bits<12,  4>(instr);
    int rn         = bits<16,  4>(instr);
    int load       = bits<20,  1>(instr);
    int writeback  = bits<21,  1>(instr);
    int byte       = bits<22,  1>(instr);
    int increment  = bits<23,  1>(instr);
    int pre_index  = bits<24,  1>(instr);
    int imm_offset = bits<25,  1>(instr);

    std::string offset;
    if (imm_offset)
        offset = shiftedRegister(data);
    else
        offset = hex(data);

    auto mnemonic = fmt::format(
        "{}{}{}",
        load ? "ldr" : "str",
        condition(instr),
        byte ? "b" : ""
    );

    if (pre_index)
    {
        return fmt::format(
            MNEMONIC"{},[{},{}{}]{}",
            mnemonic,
            reg(rd),
            reg(rn),
            increment ? "" : "-",
            offset,
            writeback ? "!" : ""
        );
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},[{}],{}{}",
            mnemonic,
            reg(rd),
            reg(rn),
            increment ? "" : "-",
            offset
        );
    }
}

const std::string Arm_HalfSignedDataTransfer(u32 instr)
{
    int half       = bits< 5, 1>(instr);
    int sign       = bits< 6, 1>(instr);
    int rd         = bits<12, 4>(instr);
    int rn         = bits<16, 4>(instr);
    int load       = bits<20, 1>(instr);
    int writeback  = bits<21, 1>(instr);
    int imm_offset = bits<22, 1>(instr);
    int increment  = bits<23, 1>(instr);
    int pre_index  = bits<24, 1>(instr);

    std::string offset;
    if (imm_offset)
    {
        int lower = bits<0, 4>(instr);
        int upper = bits<8, 4>(instr);
        offset = hex(upper << 4 | lower);
    }
    else
    {
        int rm = bits<0, 4>(instr);
        offset = reg(rm);
    }

    auto mnemonic = fmt::format(
        "{}{}{}{}",
        load ? "ldr" : "str",
        condition(instr),
        sign ? "s" : "",
        half ? "h" : "b"
    );

    if (pre_index)
    {
        return fmt::format(
            MNEMONIC"{},[{},{}{}]{}",
            mnemonic,
            reg(rd),
            reg(rn),
            increment ? "" : "-",
            offset,
            writeback ? "!" : ""
        );
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},[{}{}],{}",
            mnemonic,
            reg(rd),
            reg(rn),
            increment ? "" : "-",
            offset
        );
    }
}

const std::string Arm_BlockDataTransfer(u32 instr)
{
    int rlist     = bits< 0, 16>(instr);
    int rn        = bits<16,  4>(instr);
    int load      = bits<20,  1>(instr);
    int writeback = bits<21,  1>(instr);
    int user_mode = bits<22,  1>(instr);
    int opcode    = bits<23,  2>(instr);

    static constexpr const char* types[2][4] = {
        { "fa", "fd", "ea", "ed" },
        { "ed", "ea", "fd", "fa" }
    };

    auto mnemonic = fmt::format(
        "{}{}{}",
        load ? "ldm" : "stm",
        condition(instr),
        types[load][opcode]
    );

    return fmt::format(
        MNEMONIC"{}{},{}{}",
        mnemonic,
        reg(rn),
        writeback ? "!" : "",
        list(rlist),
        user_mode ? "^" : ""
    );
}

const std::string Arm_SingleDataSwap(u32 instr)
{
    int rm   = bits< 0, 4>(instr);
    int rd   = bits<12, 4>(instr);
    int rn   = bits<16, 4>(instr);
    int byte = bits<22, 1>(instr);

    auto mnemonic = fmt::format(
        "swp{}{}",
        condition(instr),
        byte ? "b" : ""
    );

    return fmt::format(
        MNEMONIC"{},{},[{}]",
        mnemonic,
        reg(rd),
        reg(rm),
        reg(rn)
    );
}

const std::string Arm_SoftwareInterrupt(u32 instr)
{
    int comment = bits<16, 8>(instr);

    const char* func = comment < 43
        ? bios_funcs[comment]
        : "unknown";

    return fmt::format(MNEMONIC"{:X}, {}", "swi", comment, func);
}

const std::string Thumb_MoveShiftedRegister(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rs     = bits< 3, 3>(instr);
    int offset = bits< 6, 5>(instr);
    int opcode = bits<11, 2>(instr);

    static constexpr const char* mnemonics[4] = {
        "lsl", "lsr", "asr", "???"
    };

    return fmt::format(
        MNEMONIC"{},{},{}",
        mnemonics[opcode],
        reg(rd),
        reg(rs),
        hex(offset)
    );
}

const std::string Thumb_AddSubtract(u16 instr)
{
    int rd       = bits< 0, 3>(instr);
    int rs       = bits< 3, 3>(instr);
    int rn       = bits< 6, 3>(instr);
    int subtract = bits< 9, 1>(instr);
    int use_imm  = bits<10, 1>(instr);

    if (use_imm && rn == 0)
    {
        return fmt::format(
            MNEMONIC"{},{}",
            "mov",
            reg(rd),
            reg(rs)
        );
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},{},{}",
            subtract ? "sub" : "add",
            reg(rd),
            reg(rs),
            use_imm ? hex(rn) : reg(rn)
        );
    }
}

const std::string Thumb_ImmediateOperations(u16 instr)
{
    int offset = bits< 0, 8>(instr);
    int rd     = bits< 8, 3>(instr);
    int opcode = bits<11, 2>(instr);

    static constexpr const char* mnemonics[4] = {
        "mov", "cmp", "add", "sub"
    };

    return fmt::format(
        MNEMONIC"{},{}",
        mnemonics[opcode],
        reg(rd),
        hex(offset)
    );
}

const std::string Thumb_ALUOperations(u16 instr)
{
    int rd     = bits<0, 3>(instr);
    int rs     = bits<3, 3>(instr);
    int opcode = bits<6, 4>(instr);

    static constexpr const char* mnemonics[16] = {
        "and", "eor", "lsl", "lsr",
        "asr", "adc", "sbc", "ror",
        "tst", "neg", "cmp", "cmn",
        "orr", "mul", "bic", "mvn"
    };

    return fmt::format(
        MNEMONIC"{},{}",
        mnemonics[opcode],
        reg(rd),
        reg(rs)
    );
}

const std::string Thumb_HighRegisterOperations(u16 instr)
{
    int rd     = bits<0, 3>(instr);
    int rs     = bits<3, 3>(instr);
    int hs     = bits<6, 1>(instr);
    int hd     = bits<7, 1>(instr);
    int opcode = bits<8, 2>(instr);

    rs |= hs << 3;
    rd |= hd << 3;

    static constexpr const char* mnemonics[4] = {
        "add", "cmp", "mov", "bx"
    };

    if (opcode == 0b11)
        return fmt::format(MNEMONIC"{}", mnemonics[opcode], reg(rs));
    else
        return fmt::format(MNEMONIC"{},{}", mnemonics[opcode], reg(rd), reg(rs));
}

const std::string Thumb_LoadPCRelative(u16 instr, u32 pc)
{
    int offset = bits<0, 8>(instr);
    int rd     = bits<8, 3>(instr);

    offset <<= 2;

    return fmt::format(
        MNEMONIC"{},[{}]",
        "ldr",
        reg(rd),
        hex(alignWord(pc + offset))
    );
}

const std::string Thumb_LoadStoreRegisterOffset(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int ro     = bits< 6, 3>(instr);
    int opcode = bits<10, 2>(instr);

    static constexpr const char* mnemonics[4] = {
        "str", "strb", "ldr", "ldrb"
    };

    return fmt::format(
        MNEMONIC"{},[{},{}]",
        mnemonics[opcode],
        reg(rd),
        reg(rb),
        reg(ro)
    );
}

const std::string Thumb_LoadStoreByteHalf(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int ro     = bits< 6, 3>(instr);
    int opcode = bits<10, 2>(instr);

    static constexpr const char* mnemonics[4] = {
        "strh", "ldrsb", "ldrh", "ldrsh"
    };

    return fmt::format(
        MNEMONIC"{},[{},{}]",
        mnemonics[opcode],
        reg(rd),
        reg(rb),
        reg(ro)
    );
}

const std::string Thumb_LoadStoreImmediateOffset(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int offset = bits< 6, 5>(instr);
    int opcode = bits<11, 2>(instr);

    offset <<= ~opcode & 0x2;

    static constexpr const char* mnemonics[4] = {
        "str", "strb", "ldr", "ldrb"
    };

    return fmt::format(
        MNEMONIC"{},[{},{}]",
        mnemonics[opcode],
        reg(rd),
        reg(rb),
        hex(offset)
    );
}

const std::string Thumb_LoadStoreHalf(u16 instr)
{
    int rd     = bits< 0, 3>(instr);
    int rb     = bits< 3, 3>(instr);
    int offset = bits< 6, 5>(instr);
    int load   = bits<11, 1>(instr);

    offset <<= 1;

    return fmt::format(
        MNEMONIC"{},[{},{}]",
        load ? "ldrh" : "strh",
        reg(rd),
        reg(rb),
        hex(offset)
    );
}

const std::string Thumb_LoadStoreSPRelative(u16 instr)
{
    int offset = bits< 0, 8>(instr);
    int rd     = bits< 8, 3>(instr);
    int load   = bits<11, 1>(instr);

    offset <<= 2;

    return fmt::format(
        MNEMONIC"{},[sp,{}]",
        load ? "ldr" : "str",
        reg(rd),
        hex(offset)
    );
}

const std::string Thumb_LoadRelativeAddress(u16 instr, u32 pc)
{
    int offset = bits< 0, 8>(instr);
    int rd     = bits< 8, 3>(instr);
    int use_sp = bits<11, 1>(instr);

    offset <<= 2;

    if (use_sp)
    {
        return fmt::format(
            MNEMONIC"{},sp,{}",
            "add",
            reg(rd), 
            hex(offset)
        );
    }
    else
    {
        return fmt::format(
            MNEMONIC"{},={}",
            "add",
            reg(rd),
            hex(alignWord(pc + offset))
        );
    }
}

const std::string Thumb_AddOffsetSP(u16 instr)
{
    int offset = bits<0, 7>(instr);
    int sign   = bits<7, 1>(instr);

    offset <<= 2;

    return fmt::format(
        MNEMONIC"sp,{}{}",
        "add",
        sign ? "-" : "",
        hex(offset)
    );
}

const std::string Thumb_PushPopRegisters(u16 instr)
{
    int rlist   = bits< 0, 8>(instr);
    int special = bits< 8, 1>(instr);
    int pop     = bits<11, 1>(instr);

    if (special) rlist |= 1 << (pop ? 15 : 14);

    return fmt::format(
        MNEMONIC"{}",
        pop ? "pop" : "push",
        list(rlist)
    );
}

const std::string Thumb_LoadStoreMultiple(u16 instr)
{
    int rlist = bits< 0, 8>(instr);
    int rb    = bits< 8, 3>(instr);
    int load  = bits<11, 1>(instr);

    return fmt::format(
        MNEMONIC"{}!,{}",
        load ? "ldmia" : "stmia",
        reg(rb),
        list(rlist)
    );
}

const std::string Thumb_ConditionalBranch(u16 instr, u32 pc)
{
    int offset    = bits<0, 8>(instr);
    int condition = bits<8, 4>(instr);

    offset = signExtend<8>(offset);
    offset <<= 1;

    static constexpr const char* mnemonics[16] = {
        "beq", "bne", "bcs", "bcc",
        "bmi", "bpl", "bvs", "bvc",
        "bhi", "bls", "bge", "blt",
        "bgt", "ble", "b",   "b??"
    };

    return fmt::format(
        MNEMONIC"{}",
        mnemonics[condition],
        hex(pc + offset)
    );
}

const std::string Thumb_SoftwareInterrupt(u16 instr)
{
    int comment = bits<0, 8>(instr);

    const char* func = comment < 43
        ? bios_funcs[comment]
        : "unknown";

    return fmt::format(MNEMONIC"{:X} {}", "swi", comment, func);

}

const std::string Thumb_UnconditionalBranch(u16 instr, u32 pc)
{
    int offset = bits<0, 11>(instr);

    offset = signExtend<11>(offset);
    offset <<= 1;

    return fmt::format(
        MNEMONIC"{}",
        "b",
        hex(pc + offset)
    );
}

const std::string Thumb_LongBranchLink(u16 instr, u32 lr)
{
    int offset = bits< 0, 11>(instr);
    int second = bits<11,  1>(instr);

    offset <<= 1;

    return fmt::format(
        MNEMONIC"{}",
        "bl",
        second ? hex(lr + offset) : "<setup>"
    );
}

const std::string disassemble(const DisasmData& data)
{
    if (data.thumb)
    {
        switch (decodeThumb(data.instr))
        {
        case InstructionThumb::MoveShiftedRegister: return Thumb_MoveShiftedRegister(data.instr);
        case InstructionThumb::AddSubtract: return Thumb_AddSubtract(data.instr);
        case InstructionThumb::ImmediateOperations: return Thumb_ImmediateOperations(data.instr);
        case InstructionThumb::ALUOperations: return Thumb_ALUOperations(data.instr);
        case InstructionThumb::HighRegisterOperations: return Thumb_HighRegisterOperations(data.instr);
        case InstructionThumb::LoadPCRelative: return Thumb_LoadPCRelative(data.instr, data.pc);
        case InstructionThumb::LoadStoreRegisterOffset: return Thumb_LoadStoreRegisterOffset(data.instr);
        case InstructionThumb::LoadStoreByteHalf: return Thumb_LoadStoreByteHalf(data.instr);
        case InstructionThumb::LoadStoreImmediateOffset: return Thumb_LoadStoreImmediateOffset(data.instr);
        case InstructionThumb::LoadStoreHalf: return Thumb_LoadStoreHalf(data.instr);
        case InstructionThumb::LoadStoreSPRelative: return Thumb_LoadStoreSPRelative(data.instr);
        case InstructionThumb::LoadRelativeAddress: return Thumb_LoadRelativeAddress(data.instr, data.pc);
        case InstructionThumb::AddOffsetSP: return Thumb_AddOffsetSP(data.instr);
        case InstructionThumb::PushPopRegisters: return Thumb_PushPopRegisters(data.instr);
        case InstructionThumb::LoadStoreMultiple: return Thumb_LoadStoreMultiple(data.instr);
        case InstructionThumb::ConditionalBranch: return Thumb_ConditionalBranch(data.instr, data.pc);
        case InstructionThumb::SoftwareInterrupt: return Thumb_SoftwareInterrupt(data.instr);
        case InstructionThumb::UnconditionalBranch: return Thumb_UnconditionalBranch(data.instr, data.pc);
        case InstructionThumb::LongBranchLink: return Thumb_LongBranchLink(data.instr, data.lr);
        }
    }
    else
    {
        switch (decodeArm(data.instr))
        {
        case InstructionArm::BranchExchange: return Arm_BranchExchange(data.instr);
        case InstructionArm::BranchLink: return Arm_BranchLink(data.instr, data.pc);
        case InstructionArm::DataProcessing: return Arm_DataProcessing(data.instr, data.pc);
        case InstructionArm::StatusTransfer: return Arm_StatusTransfer(data.instr);
        case InstructionArm::Multiply: return Arm_Multiply(data.instr);
        case InstructionArm::MultiplyLong: return Arm_MultiplyLong(data.instr);
        case InstructionArm::SingleDataTransfer: return Arm_SingleDataTransfer(data.instr);
        case InstructionArm::HalfSignedDataTransfer: return Arm_HalfSignedDataTransfer(data.instr);
        case InstructionArm::BlockDataTransfer: return Arm_BlockDataTransfer(data.instr);
        case InstructionArm::SingleDataSwap: return Arm_SingleDataSwap(data.instr);
        case InstructionArm::SoftwareInterrupt: return Arm_SoftwareInterrupt(data.instr);
        }
    }
    return "undef";
}
