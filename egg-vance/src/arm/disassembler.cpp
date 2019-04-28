#include "disassembler.h"

#include "common/format.h"
#include "utility.h"

std::string Disassembler::disassemble(u32 instr, Format format, const Registers& regs)
{
    switch (format)
    {
    case THUMB_1:  return moveShiftedRegister(instr);
    case THUMB_2:  return addSubImmediate(instr);
    case THUMB_3:  return addSubMovCmpImmediate(instr);
    case THUMB_4:  return aluOperations(instr);
    case THUMB_5:  return highRegisterBranchExchange(instr);
    case THUMB_6:  return loadPcRelative(instr, regs.pc);
    case THUMB_7:  return loadStoreRegisterOffset(instr);
    case THUMB_8:  return loadStoreHalfSigned(instr);
    case THUMB_9:  return loadStoreImmediateOffset(instr);
    case THUMB_10: return loadStoreHalf(instr);
    case THUMB_11: return loadStoreSpRelative(instr);
    case THUMB_12: return loadAddress(instr, regs.pc);
    case THUMB_13: return addOffsetSp(instr);
    case THUMB_14: return pushPopRegisters(instr);
    case THUMB_15: return loadStoreMultiple(instr);
    case THUMB_16: return conditionalBranch(instr, regs.pc);
    case THUMB_17: return swiThumb(instr);
    case THUMB_18: return unconditionalBranch(instr, regs.pc);
    case THUMB_19: return longBranchLink(instr, regs.lr);
    case ARM_1:    return branchExchange(instr);
    case ARM_2:    return branchLink(instr, regs.pc);
    case ARM_3:    return dataProcessing(instr, regs.pc);
    case ARM_4:    return psrTransfer(instr);
    case ARM_5:    return multiply(instr);
    case ARM_6:    return multiplyLong(instr);
    case ARM_7:    return singleTransfer(instr);
    case ARM_8:    return halfSignedTransfer(instr);
    case ARM_9:    return blockTransfer(instr);
    case ARM_10:   return singleDataSwap(instr);
    case ARM_11:   return swiArm(instr);
    }
    return "unimpl";
}

void Disassembler::mnemonicPad(std::string& mnemonic)
{
    if (mnemonic.size() < 8)
        mnemonic.append(8 - mnemonic.size(), ' ');
}

std::string Disassembler::reg(int number)
{
    if (number <= 12)
    {
        return fmt::format("r{}", number);
    }
    else
    {
        switch (number)
        {
        case 13: return "sp";
        case 14: return "lr";
        case 15: return "pc";
        }
    }
    return "r?";
}

std::string Disassembler::hex(u32 value)
{
    return fmt::format("{:X}h", value);
}

std::string Disassembler::list(int rlist)
{
    std::string result = "{";

    for (int x = 0; x < 16; ++x)
    {
        if (rlist & (1 << x))
            result.append(reg(x) + ",");
    }

    if (rlist == 0)
        result.append("}");
    else
        result.back() = '}';

    return result;
}

std::string Disassembler::cond(u32 instr)
{
    switch (static_cast<Condition>(instr >> 28))
    {
    case COND_EQ: return "eq"; break;
    case COND_NE: return "ne"; break;
    case COND_CS: return "cs"; break;
    case COND_CC: return "cc"; break;
    case COND_MI: return "mi"; break;
    case COND_PL: return "pl"; break;
    case COND_VS: return "vs"; break;
    case COND_VC: return "vc"; break;
    case COND_HI: return "hi"; break;
    case COND_LS: return "ls"; break;
    case COND_GE: return "ge"; break;
    case COND_LT: return "lt"; break;
    case COND_GT: return "gt"; break;
    case COND_LE: return "le"; break;
    case COND_AL: return ""  ; break;
    case COND_NV: return "nv"; break;
    }
    return "??";
}

std::string Disassembler::moveShiftedRegister(u16 instr)
{
    int opcode = (instr >> 11) & 0x03;
    int offset = (instr >>  6) & 0x1F;
    int rs     = (instr >>  3) & 0x07;
    int rd     = (instr >>  0) & 0x07;

    std::string mnemonic;
    switch (opcode)
    {
    case 0b00: mnemonic = "lsl"; break;
    case 0b01: mnemonic = "lsr"; break;
    case 0b10: mnemonic = "asr"; break;
    case 0b11: mnemonic = "???"; break;
    }

    return fmt::format("{:<8}{},{},{}",
        mnemonic,
        reg(rd),
        reg(rs),
        hex(offset)
    );
}

std::string Disassembler::addSubImmediate(u16 instr)
{
    int use_imm  = (instr >> 10) & 0x1;
    int subtract = (instr >>  9) & 0x1;
    int rn       = (instr >>  6) & 0x7;
    int rs       = (instr >>  3) & 0x7;
    int rd       = (instr >>  0) & 0x7;

    if (use_imm && rn == 0)
    {
        return fmt::format("{:<8}{},{}",
            "mov",
            reg(rd),
            reg(rs)
        );
    }
    else
    {
        return fmt::format("{:<8}{},{},{}",
            subtract ? "sub" : "add",
            reg(rd),
            reg(rs),
            use_imm ? hex(rn) : reg(rn)
        );
    }
}

std::string Disassembler::addSubMovCmpImmediate(u16 instr)
{
    int opcode = (instr >> 11) & 0x03;
    int rd     = (instr >>  8) & 0x07;
    int offset = (instr >>  0) & 0xFF;

    std::string mnemonic;
    switch (opcode)
    {
    case 0b00: mnemonic = "mov"; break;
    case 0b01: mnemonic = "cmp"; break;
    case 0b10: mnemonic = "add"; break;
    case 0b11: mnemonic = "sub"; break;
    }

    return fmt::format("{:<8}{},{}",
        mnemonic,
        reg(rd),
        hex(offset)
    );
}

std::string Disassembler::aluOperations(u16 instr)
{
    int opcode = (instr >> 6) & 0xF;
    int rs     = (instr >> 3) & 0x7;
    int rd     = (instr >> 0) & 0x7;

    std::string mnemonic;
    switch (opcode)
    {
    case 0b0000: mnemonic = "add"; break;
    case 0b0001: mnemonic = "eor"; break;
    case 0b0010: mnemonic = "lsl"; break;
    case 0b0011: mnemonic = "lsr"; break;
    case 0b0100: mnemonic = "asr"; break;
    case 0b0101: mnemonic = "adc"; break;
    case 0b0110: mnemonic = "sbc"; break;
    case 0b0111: mnemonic = "ror"; break;
    case 0b1000: mnemonic = "tst"; break;
    case 0b1001: mnemonic = "neg"; break;
    case 0b1010: mnemonic = "cmp"; break;
    case 0b1011: mnemonic = "cmn"; break;
    case 0b1100: mnemonic = "orr"; break;
    case 0b1101: mnemonic = "mul"; break;
    case 0b1110: mnemonic = "bic"; break;
    case 0b1111: mnemonic = "mvn"; break;
    }

    return fmt::format("{:<8}{},{}",
        mnemonic,
        reg(rd),
        reg(rs)
    );
}

std::string Disassembler::highRegisterBranchExchange(u16 instr)
{
    int opcode = (instr >> 8) & 0x3;
    int hd     = (instr >> 7) & 0x1;
    int hs     = (instr >> 6) & 0x1;
    int rs     = (instr >> 3) & 0x7;
    int rd     = (instr >> 0) & 0x7;

    rs |= hs << 3;
    rd |= hd << 3;

    std::string mnemonic;
    switch (opcode)
    {
    case 0b00: mnemonic = "add"; break;
    case 0b01: mnemonic = "cmp"; break;
    case 0b10: mnemonic = "mov"; break;
    case 0b11: mnemonic = "bx";  break;
    }

    if (opcode == 0b11)
    {
        return fmt::format("{:<8}{}",
            mnemonic,
            reg(rs)
        );
    }
    else
    {
        return fmt::format("{:<8}{},{}",
            mnemonic,
            reg(rd),
            reg(rs)
        );
    }
}

std::string Disassembler::loadPcRelative(u16 instr, u32 pc)
{
    int rd     = (instr >> 8) & 0x07;
    int offset = (instr >> 0) & 0xFF;
    
    offset <<= 2;

    return fmt::format("{:<8}{},[{}]",
        "ldr",
        reg(rd),
        hex(alignWord(pc) + offset)
    );
}

std::string Disassembler::loadStoreRegisterOffset(u16 instr)
{
    int load = (instr >> 11) & 0x1;
    int byte = (instr >> 10) & 0x1;
    int ro   = (instr >>  6) & 0x7;
    int rb   = (instr >>  3) & 0x7;
    int rd   = (instr >>  0) & 0x7;

    std::string mnemonic;
    switch ((load << 1) | byte)
    {
    case 0b00: mnemonic = "str" ; break;
    case 0b01: mnemonic = "strb"; break;
    case 0b10: mnemonic = "ldr" ; break;
    case 0b11: mnemonic = "ldrb"; break;
    }

    return fmt::format("{:<8}{},[{},{}]",
        mnemonic,
        reg(rd),
        reg(rb),
        reg(ro)
    );
}

std::string Disassembler::loadStoreHalfSigned(u16 instr)
{
    int opcode = (instr >> 10) & 0x3;
    int ro     = (instr >>  6) & 0x7;
    int rb     = (instr >>  3) & 0x7;
    int rd     = (instr >>  0) & 0x7;

    std::string mnemonic;
    switch (opcode)
    {
    case 0b00: mnemonic = "strh" ; break;
    case 0b01: mnemonic = "ldrsb"; break;
    case 0b10: mnemonic = "ldrh" ; break;
    case 0b11: mnemonic = "ldrsh"; break;
    }

    return fmt::format("{:<8}{},[{},{}]",
        mnemonic,
        reg(rd),
        reg(rb),
        reg(ro)
    );
}

std::string Disassembler::loadStoreImmediateOffset(u16 instr)
{
    int byte   = (instr >> 12) & 0x01;
    int load   = (instr >> 11) & 0x01;
    int offset = (instr >>  6) & 0x1F;
    int rb     = (instr >>  3) & 0x07;
    int rd     = (instr >>  0) & 0x07;

    if (!byte)
        offset <<= 2;

    std::string mnemonic;
    switch ((load << 1) | byte)
    {
    case 0b00: mnemonic = "str" ; break;
    case 0b01: mnemonic = "strb"; break;
    case 0b10: mnemonic = "ldr" ; break;
    case 0b11: mnemonic = "ldrb"; break;
    }

    return fmt::format("{:<8}{},[{},{}]",
        mnemonic,
        reg(rd),
        reg(rb),
        hex(offset)
    );
}

std::string Disassembler::loadStoreHalf(u16 instr)
{
    int load   = (instr >> 11) & 0x01;
    int offset = (instr >>  6) & 0x1F;
    int rb     = (instr >>  3) & 0x07;
    int rd     = (instr >>  0) & 0x07;

    offset <<= 1;

    return fmt::format("{:<8}{},[{},{}]",
        load ? "ldrh" : "strh",
        reg(rd),
        reg(rb),
        hex(offset)
    );
}

std::string Disassembler::loadStoreSpRelative(u16 instr)
{
    int load   = (instr >> 11) & 0x01;
    int rd     = (instr >>  8) & 0x07;
    int offset = (instr >>  0) & 0xFF;

    offset <<= 2;

    return fmt::format("{:<8}{},[sp,{}]",
        load ? "ldr" : "str",
        reg(rd),
        hex(offset)
    );
}

std::string Disassembler::loadAddress(u16 instr, u32 pc)
{
    int use_sp = (instr >> 11) & 0x01;
    int rd     = (instr >>  8) & 0x07;
    int offset = (instr >>  0) & 0xFF;

    offset <<= 2;

    if (use_sp)
    {
        return fmt::format("{:<8}{},sp,{}",
            "add",
            reg(rd),
            hex(offset)
        );
    }
    else
    {
        return fmt::format("{:<8}{},={}",
            "add",
            reg(rd),
            hex(alignWord(pc) + offset)
        );
    }
}

std::string Disassembler::addOffsetSp(u16 instr)
{
    int sign   = (instr >> 7) & 0x01;
    int offset = (instr >> 0) & 0x3F;

    offset <<= 2;

    return fmt::format("{:<8}sp,{}{}",
        "add",
        sign ? "-" : "",
        hex(offset)
    );
}

std::string Disassembler::pushPopRegisters(u16 instr)
{
    int pop   = (instr >> 11) & 0x01;
    int pc_lr = (instr >>  8) & 0x01;
    int rlist = (instr >>  0) & 0xFF;

    if (pc_lr)
        rlist |= 1 << (pop ? 15 : 14);

    return fmt::format("{:<8}{}",
        pop ? "pop" : "push",
        list(rlist)
    );
}

std::string Disassembler::loadStoreMultiple(u16 instr)
{
    int load  = (instr >> 11) & 0x01;
    int rb    = (instr >>  8) & 0x07;
    int rlist = (instr >>  0) & 0xFF;

    return fmt::format("{:<8}{}!,{}",
        load ? "ldmia" : "stmia",
        reg(rb),
        list(rlist)
    );
}

std::string Disassembler::conditionalBranch(u16 instr, u32 pc)
{
    int condition = (instr >> 8) & 0x0F;
    int offset    = (instr >> 0) & 0xFF;

    offset = twos<8>(offset);
    offset <<= 1;

    std::string mnemonic;
    switch (static_cast<Condition>(condition))
    {
    case COND_EQ: mnemonic = "beq"; break;
    case COND_NE: mnemonic = "bne"; break;
    case COND_CS: mnemonic = "bcs"; break;
    case COND_CC: mnemonic = "bcc"; break;
    case COND_MI: mnemonic = "bmi"; break;
    case COND_PL: mnemonic = "bpl"; break;
    case COND_VS: mnemonic = "bvs"; break;
    case COND_VC: mnemonic = "bvc"; break;
    case COND_HI: mnemonic = "bhi"; break;
    case COND_LS: mnemonic = "bls"; break;
    case COND_GE: mnemonic = "bge"; break;
    case COND_LT: mnemonic = "blt"; break;
    case COND_GT: mnemonic = "bgt"; break;
    case COND_LE: mnemonic = "ble"; break;
    case COND_AL: mnemonic = "b"; break;
    case COND_NV: mnemonic = "b??"; break;
    }

    return fmt::format("{:<8}{}",
        mnemonic,
        hex(pc + offset)
    );
}

std::string Disassembler::swiThumb(u16 instr)
{
    return "unimplemented thumb swi";
}

std::string Disassembler::unconditionalBranch(u16 instr, u32 pc)
{
    int offset = (instr >> 0) & 0x7FF;

    offset = twos<11>(offset);
    offset <<= 1;

    return fmt::format("{:<8}{}",
        "b",
        hex(pc + offset)
    );
}

std::string Disassembler::longBranchLink(u16 instr, u32 lr)
{
    int second = (instr >> 11) & 0x001;
    int offset = (instr >>  0) & 0x7FF;

    offset <<= 1;

    return fmt::format("{:<8}{}",
        "bl",
        second ? hex(lr + offset) : "<setup>"
    );;
}

std::string Disassembler::shiftedRegister(int data)
{
    int type    = (data >> 5) & 0x3;
    int use_reg = (data >> 4) & 0x1;
    int rm      = (data >> 0) & 0xF;

    std::string shift;
    switch (type)
    {
    case 0b00: shift = "lsl"; break;
    case 0b01: shift = "lsr"; break;
    case 0b10: shift = "asr"; break;
    case 0b11: shift = "ror"; break;
    }
    
    std::string offset;
    if (use_reg)
    {
        int rs = (data >> 8) & 0xF;
        offset = reg(rs);
    }
    else
    {
        int imm = (data >> 7) & 0x1F;
        if (imm == 0)
            return reg(rm);
        offset = hex(imm);
    }
    
    return fmt::format("{},{} {}",
        reg(rm),
        shift,
        offset
    );
}

u32 Disassembler::rotatedImmediate(int data)
{
    int rotation = (data >> 8) & 0x0F;
    u32 value    = (data >> 0) & 0xFF;

    rotation <<= 1;
    rotation %= 32;

    if (rotation != 0)
        value = (value << (32 - rotation)) | (value >> rotation);

    return value;
}

std::string Disassembler::branchExchange(u32 instr)
{
    int rn = (instr >> 0) & 0xF;

    return fmt::format("{:<8}{}",
        "bx" + cond(instr),
        reg(rn)
    );
}

std::string Disassembler::branchLink(u32 instr, u32 pc)
{
    int link   = (instr >> 24) & 0x000001;
    int offset = (instr >>  0) & 0xFFFFFF;

    offset = twos<24>(offset);
    offset <<= 2;

    return fmt::format("{:<8}{}",
        (link ? "bl" : "b") + cond(instr),
        hex(pc + offset)
    );
}

std::string Disassembler::dataProcessing(u32 instr, u32 pc)
{
    int use_imm = (instr >> 25) & 0x001;
    int opcode  = (instr >> 21) & 0x00F;
    int flags   = (instr >> 20) & 0x001;
    int rn      = (instr >> 16) & 0x00F;
    int rd      = (instr >> 12) & 0x00F;
    int data    = (instr >>  0) & 0xFFF;

    std::string op2;
    if (use_imm)
    {
        u32 value = rotatedImmediate(data);

        if (rn == 15 && (opcode == 0b0010 || opcode == 0b0100))
        {
            if (opcode == 0b0010)
                value = pc - value;
            else
                value = pc + value;
        }
        op2 = hex(value);
    }
    else
    {
        op2 = shiftedRegister(data);
    }

    std::string mnemonic;
    mnemonic.reserve(7);
    switch (opcode)
    {
    case 0b0000: mnemonic = "and"; break;
    case 0b0001: mnemonic = "eor"; break;
    case 0b0010: mnemonic = "sub"; break;
    case 0b0011: mnemonic = "rsb"; break;
    case 0b0100: mnemonic = "add"; break;
    case 0b0101: mnemonic = "adc"; break;
    case 0b0110: mnemonic = "sbc"; break;
    case 0b0111: mnemonic = "rbc"; break;
    case 0b1000: mnemonic = "tst"; break;
    case 0b1001: mnemonic = "teq"; break;
    case 0b1010: mnemonic = "cmp"; break;
    case 0b1011: mnemonic = "cmn"; break;
    case 0b1100: mnemonic = "orr"; break;
    case 0b1101: mnemonic = "mov"; break;
    case 0b1110: mnemonic = "bic"; break;
    case 0b1111: mnemonic = "mvn"; break;
    }

    mnemonic.append(cond(instr));

    if (flags)
    {
        switch (opcode)
        {
        case 0b1000:  // TST
        case 0b1001:  // TEQ
        case 0b1010:  // CMP
        case 0b1011:  // CMN
            break;

        default:
            mnemonic.append("s");
            break;
        }
    }

    switch (opcode)
    {
    case 0b0010:  // SUB
    case 0b0100:  // ADD
        if (rn == 15 && use_imm)
        {
            return fmt::format("{:<8}{},={}",
                mnemonic,
                reg(rd),
                op2
            );
        }
        else
        {
            return fmt::format("{:<8}{},{},{}",
                mnemonic,
                reg(rd),
                reg(rn),
                op2
            );
        }

    case 0b1000:  // TST
    case 0b1001:  // TEQ
    case 0b1010:  // CMP
    case 0b1011:  // CMN
        return fmt::format("{:<8}{},{}",
            mnemonic,
            reg(rn),
            op2
        );

    case 0b1101:  // MOV
    case 0b1111:  // MVN
        return fmt::format("{:<8}{},{}",
            mnemonic,
            reg(rd),
            op2
        );

    default:
        return fmt::format("{:<8}{},{},{}",
            mnemonic,
            reg(rd),
            reg(rn),
            op2
        );
    }
}

std::string Disassembler::psrTransfer(u32 instr)
{
    int write    = (instr >> 21) & 0x1;
    int use_spsr = (instr >> 22) & 0x1;

    if (write)
    {
		int use_imm = (instr >> 25) & 0x001;
		int data    = (instr >>  0) & 0xFFF;

        std::string op;
        if (use_imm)
        {
            op = hex(rotatedImmediate(data));
        }
        else
        {
            int rm = (data >> 0) & 0xF;
            op = reg(rm);
        }

        std::string fsxc;
        fsxc.reserve(5);
        if (instr & (1 << 19))
            fsxc.append("f");
        if (instr & (1 << 18))
            fsxc.append("s");
        if (instr & (1 << 17))
            fsxc.append("x");
        if (instr & (1 << 16))
            fsxc.append("c");

        return fmt::format("{:<8}{}_{},{}",
            "msr" + cond(instr),
            use_spsr ? "spsr" : "cpsr",
            fsxc,
            op
        );
    }
    else
    {
        int rd = (instr >> 12) & 0xF;

        return fmt::format("{:<8}{},{}", 
            "mrs" + cond(instr),
            reg(rd),
            use_spsr ? "spsr" : "cpsr"
        );
    }
}

std::string Disassembler::multiply(u32 instr)
{
    int accumulate = (instr >> 21) & 0x1;
    int flags      = (instr >> 20) & 0x1;
    int rd         = (instr >> 16) & 0xF;
    int rn         = (instr >> 12) & 0xF;
    int rs         = (instr >>  8) & 0xF;
    int rm         = (instr >>  0) & 0xF;

    std::string mnemonic = fmt::format("{}{}{}",
        accumulate ? "mla" : "mul",
        cond(instr),
        flags ? "s" : ""
    );

    if (accumulate)
    {
        return fmt::format("{:<8}{},{},{},{}",
            mnemonic,
            reg(rd),
            reg(rn),
            reg(rs),
            reg(rm)
        );
    }
    else
    {
        return fmt::format("{:<8}{},{},{}",
            mnemonic,
            reg(rd),
            reg(rn),
            reg(rs)
        );
    }
}

std::string Disassembler::multiplyLong(u32 instr)
{
    int sign       = (instr >> 22) & 0x1;
    int accumulate = (instr >> 21) & 0x1;
    int flags      = (instr >> 20) & 0x1;
    int rdhi       = (instr >> 16) & 0xF;
    int rdlo       = (instr >> 12) & 0xF;
    int rs         = (instr >>  8) & 0xF;
    int rm         = (instr >>  0) & 0xF;

    std::string mnemonic;
    mnemonic.reserve(9);
    switch ((sign << 1) | accumulate)
    {
    case 0b00: mnemonic = "umull"; break;
    case 0b01: mnemonic = "umlal"; break;
    case 0b10: mnemonic = "smull"; break;
    case 0b11: mnemonic = "smlal"; break;
    }
    mnemonic.append(cond(instr));
    if (flags)
        mnemonic.append("s");

    return fmt::format("{:<8}{},{},{},{}",
        mnemonic,
        reg(rdlo),
        reg(rdhi),
        reg(rs),
        reg(rm)
    );
}

std::string Disassembler::singleTransfer(u32 instr)
{
    int use_reg   = (instr >> 25) & 0x001;
    int pre_index = (instr >> 24) & 0x001;
    int increment = (instr >> 23) & 0x001;
    int byte      = (instr >> 22) & 0x001;
    int writeback = (instr >> 21) & 0x001;
    int load      = (instr >> 20) & 0x001;
    int rn        = (instr >> 16) & 0x00F;
    int rd        = (instr >> 12) & 0x00F;
    int data      = (instr >>  0) & 0xFFF;

    std::string offset;
    if (use_reg)
        offset = hex(data);
    else
        offset = shiftedRegister(data);

    std::string mnemonic = fmt::format("{}{}{}",
        load ? "ldr" : "str",
        cond(instr),
        byte ? "b" : ""
    );

    if (pre_index)
    {
        return fmt::format("{:<8}{},[{},{}]{}",
            mnemonic,
            reg(rd),
            reg(rn),
            offset,
            writeback ? "!" : ""
        );
    }
    else
    {
        return fmt::format("{:<8}{},[{}],{}",
            mnemonic,
            reg(rd),
            reg(rn),
            offset
        );
    }
}

std::string Disassembler::halfSignedTransfer(u32 instr)
{
    int pre_index = (instr >> 24) & 0x1;
    int use_imm   = (instr >> 22) & 0x1;
    int increment = (instr >> 23) & 0x1;
    int writeback = (instr >> 21) & 0x1;
    int load      = (instr >> 20) & 0x1;
    int rn        = (instr >> 16) & 0xF;
    int rd        = (instr >> 12) & 0xF;
    int sign      = (instr >>  6) & 0x1;
    int half      = (instr >>  5) & 0x1;

    std::string offset;
    if (use_imm)
    {
        int upper = (instr >> 8) & 0xF;
        int lower = (instr >> 0) & 0xF;
        offset = hex(upper << 4 | lower);
    }
    else
    {
        int rm = (instr >> 0) & 0xF;
        offset = reg(rm);
    }

    std::string mnemonic = fmt::format("{}{}{}{}",
        load ? "ldr" : "str",
        cond(instr),
        sign ? "s" : "",
        half ? "h" : "b"
    );

    if (pre_index)
    {
        return fmt::format("{:<8}{},[{},{}]{}",
            mnemonic,
            reg(rd),
            reg(rn),
            offset,
            writeback ? "!" : ""
        );
    }
    else
    {
        return fmt::format("{:<8}{},[{}],{}",
            mnemonic,
            reg(rd),
            reg(rn),
            offset
        );
    }
}

std::string Disassembler::blockTransfer(u32 instr)
{
    int full      = (instr >> 24) & 0x0001;
    int ascending = (instr >> 23) & 0x0001;
    int user      = (instr >> 22) & 0x0001;
    int writeback = (instr >> 21) & 0x0001;
    int load      = (instr >> 20) & 0x0001;
    int rn        = (instr >> 16) & 0x000F;
    int rlist     = (instr >>  0) & 0xFFFF;

    std::string type;
    if (load)
    {
        switch ((full << 1) | ascending)
        {
        case 0b00: type = "fa"; break;
        case 0b01: type = "fd"; break;
        case 0b10: type = "ea"; break;
        case 0b11: type = "ed"; break;
        }
    }
    else
    {
        switch ((full << 1) | ascending)
        {
        case 0b00: type = "ed"; break;
        case 0b01: type = "ea"; break;
        case 0b10: type = "fd"; break;
        case 0b11: type = "fa"; break;
        }
    }

    std::string mnemonic = fmt::format("{}{}{}",
        load ? "ldm" : "stm",
        cond(instr),
        type
    );

    return fmt::format("{:<8}{}{},{}{}",
        mnemonic,
        reg(rn),
        writeback ? "!" : "",
        list(rlist),
        user ? "^" : ""
    );
}

std::string Disassembler::singleDataSwap(u32 instr)
{
    int byte = (instr >> 22) & 0x1;
    int rn   = (instr >> 16) & 0xF;
    int rd   = (instr >> 12) & 0xF;
    int rm   = (instr >>  0) & 0xF;

    std::string mnemonic = fmt::format("swp{}{}",
        cond(instr),
        byte ? "b" : ""
    );

    return fmt::format("{:<8}{},{},[{}]",
        mnemonic,
        reg(rd),
        reg(rm),
        reg(rn)
    );
}

std::string Disassembler::swiArm(u32 instr)
{
    return "unimplemented thumb arm";
}
