#include "disassembler.h"

/**
 * Todo
 * - adr in data processing
 */

#include <iomanip>
#include <sstream>

#include "utility.h"

std::string Disassembler::disassemble(u32 instr, Format format, const Registers& regs)
{
    switch (format)
    {
    case THUMB_1:  return moveShiftedRegister(instr);
    case THUMB_2:  return addSubImmediate(instr);
    case THUMB_3:  return addSubCmpMovImmediate(instr);
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
    case THUMB_17: return softwareInterruptThumb(instr);
    case THUMB_18: return unconditionalBranch(instr, regs.pc);
    case THUMB_19: return longBranchLink(instr, regs.lr);
    case ARM_1:    return branchExchange(instr);
    case ARM_2:    return branchLink(instr, regs.pc);
    case ARM_3:    return dataProcessing(instr, regs.pc);
    case ARM_4:    return psrTransfer(instr);
    case ARM_5:    return multiply(instr);
    case ARM_6:    return multiplyLong(instr);
    case ARM_7:    return singleDataTransfer(instr);
    case ARM_8:    return halfSignedDataTransfer(instr);
    case ARM_9:    return blockDataTransfer(instr);
    case ARM_10:   return singleDataSwap(instr);
    }
    return "unimpl";
}

void Disassembler::mnemonicPad(std::string& mnemonic)
{
    if (mnemonic.size() < 8)
        mnemonic.append(8 - mnemonic.size(), ' ');
}

std::string Disassembler::reg(int number, bool comma)
{
    std::string result;
    if (number <= 12)
    {
        result = "r" + std::to_string(number);
    }
    else
    {
        switch (number)
        {
        case 13: result = "sp"; break;
        case 14: result = "lr"; break;
        case 15: result = "pc"; break;

        default:
            result = "??";
        }
    }

    if (comma)
        result.append(",");

    return result;
}

std::string Disassembler::hex(u32 value)
{
    std::stringstream stream;

    stream << std::uppercase << std::hex << value << std::nouppercase << "h";

    return stream.str();
}

std::string Disassembler::rlistString(int rlist)
{
    std::string result = "{";

    for (int x = 0; x < 16; ++x)
    {
        if (rlist & (1 << x))
            result.append(reg(x, true));
    }

    if (rlist == 0)
        result.append("}");
    else
        result.back() = '}';

    return result;
}

std::string Disassembler::conditionString(u32 instr)
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
    case COND_AL: return "";   break;
    case COND_NV: return "nv"; break;
    }
    return "??";
}

std::string Disassembler::moveShiftedRegister(u16 instr)
{
    int opcode = (instr >> 11) & 0x3;
    int offset = (instr >> 6) & 0x1F;
    int rs = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    std::string mnemonic;
    switch (opcode)
    {
    case 0b00: mnemonic = "lsl"; break;
    case 0b01: mnemonic = "lsr"; break;
    case 0b10: mnemonic = "asr"; break;

    default:
        mnemonic = "???";
    }

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append(reg(rs, true));
    mnemonic.append(hex(offset));

    return mnemonic;
}

std::string Disassembler::addSubImmediate(u16 instr)
{
    bool immediate = (instr >> 10) & 0x1;
    bool subtract = (instr >> 9) & 0x1;
    int rn = (instr >> 6) & 0x7;
    int rs = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    bool move = immediate && rn == 0;

    std::string mnemonic;
    if (move)
        mnemonic = "mov";
    else
        mnemonic = subtract ? "sub" : "add";

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append(reg(rs, !move));
    if (!move)
        mnemonic.append(immediate ? hex(rn) : reg(rn, false));

    return mnemonic;
}

std::string Disassembler::addSubCmpMovImmediate(u16 instr)
{
    int opcode = (instr >> 11) & 0x3;
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;

    std::string mnemonic;
    switch (opcode)
    {
    case 0b00: mnemonic = "mov"; break;
    case 0b01: mnemonic = "cmp"; break;
    case 0b10: mnemonic = "add"; break;
    case 0b11: mnemonic = "sub"; break;
    }

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append(hex(offset));

    return mnemonic;
}

std::string Disassembler::aluOperations(u16 instr)
{
    int opcode = (instr >> 6) & 0xF;
    int rs = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

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

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append(reg(rs, false));

    return mnemonic;
}

std::string Disassembler::highRegisterBranchExchange(u16 instr)
{
    int opcode = (instr >> 8) & 0x3;
    int hd = (instr >> 7) & 0x1;
    int hs = (instr >> 6) & 0x1;
    int rs = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

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

    mnemonicPad(mnemonic);
    if (opcode != 0b11)
        mnemonic.append(reg(rd, true));
    mnemonic.append(reg(rs, false));

    return mnemonic;
}

std::string Disassembler::loadPcRelative(u16 instr, u32 pc)
{
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;
    
    offset <<= 2;

    std::string mnenonic = "ldr";

    mnemonicPad(mnenonic);
    mnenonic.append(reg(rd, true));
    mnenonic.append("[");
    mnenonic.append(hex((pc & ~0x2) + offset));
    mnenonic.append("]");

    return mnenonic;
}

std::string Disassembler::loadStoreRegisterOffset(u16 instr)
{
    int load = (instr >> 11) & 0x1;
    int byte = (instr >> 10) & 0x1;
    int ro = (instr >> 6) & 0x7;
    int rb = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    std::string mnemonic;
    switch (load << 1 | byte)
    {
    case 0b00: mnemonic = "str";  break;
    case 0b01: mnemonic = "strb"; break;
    case 0b10: mnemonic = "ldr";  break;
    case 0b11: mnemonic = "ldrb"; break;
    }

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(rb, true));
    mnemonic.append(reg(ro, false));
    mnemonic.append("]");

    return mnemonic;
}

std::string Disassembler::loadStoreHalfSigned(u16 instr)
{
    int half = (instr >> 11) & 0x1;
    int sign = (instr >> 10) & 0x1;
    int ro = (instr >> 6) & 0x7;
    int rb = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    std::string mnemonic;
    switch (sign << 1 | half)
    {
    case 0b00: mnemonic = "strh";  break;
    case 0b01: mnemonic = "ldrh";  break;
    case 0b10: mnemonic = "ldrsb"; break;
    case 0b11: mnemonic = "ldrsh"; break;
    }

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(rb, true));
    mnemonic.append(reg(ro, false));
    mnemonic.append("]");

    return mnemonic;
}

std::string Disassembler::loadStoreImmediateOffset(u16 instr)
{
    int byte = (instr >> 12) & 0x1;
    int load = (instr >> 11) & 0x1;
    int offset = (instr >> 6) & 0x1F;
    int rb = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    if (!byte)
        offset <<= 2;

    std::string mnemonic;
    switch (load << 1 | byte)
    {
    case 0b00: mnemonic = "str";  break;
    case 0b01: mnemonic = "strb"; break;
    case 0b10: mnemonic = "ldr";  break;
    case 0b11: mnemonic = "ldrb"; break;
    }

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(rb, true));
    mnemonic.append(hex(offset));
    mnemonic.append("]");

    return mnemonic;
}

std::string Disassembler::loadStoreHalf(u16 instr)
{
    bool load = (instr >> 11) & 0x1;
    int offset = (instr >> 6) & 0x1F;
    int rb = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    offset <<= 1;

    std::string mnemonic = load ? "ldrh" : "strh";

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(rb, true));
    mnemonic.append(hex(offset));
    mnemonic.append("]");

    return mnemonic;
}

std::string Disassembler::loadStoreSpRelative(u16 instr)
{
    bool load = (instr >> 11) & 0x1;
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;

    offset <<= 2;

    std::string mnemonic = load ? "ldr" : "str";

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[sp,");
    mnemonic.append(hex(offset));
    mnemonic.append("]");

    return mnemonic;
}

std::string Disassembler::loadAddress(u16 instr, u32 pc)
{
    // No ADR, use =newpc

    bool sp = (instr >> 11) & 0x1;
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;

    offset <<= 2;

    std::string mnemonic = sp ? "add" : "adr";

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    if (sp)
    {
        mnemonic.append("sp,");
        mnemonic.append(hex(offset));
    }
    else
    {
        mnemonic.append(hex((pc & ~0x2) + offset));
    }
    return mnemonic;
}

std::string Disassembler::addOffsetSp(u16 instr)
{
    bool sign = (instr >> 7) & 0x1;
    int offset = instr & 0x3F;

    offset <<= 2;

    std::string mnemonic = "add";

    mnemonicPad(mnemonic);
    mnemonic.append("sp,");
    if (sign)
        mnemonic.append("-");
    mnemonic.append(hex(offset));

    return mnemonic;
}

std::string Disassembler::pushPopRegisters(u16 instr)
{
    bool pop = (instr >> 11) & 0x1;
    bool routine = (instr >> 8) & 0x1;
    int rlist = instr & 0xFF;

    if (routine)
        rlist |= 1 << (pop ? 15 : 14);

    std::string mnemonic = pop ? "pop" : "push";

    mnemonicPad(mnemonic);
    mnemonic.append(rlistString(rlist));

    return mnemonic;
}

std::string Disassembler::loadStoreMultiple(u16 instr)
{
    bool load = (instr >> 11) & 0x1;
    int rb = (instr >> 8) & 0x7;
    int rlist = instr & 0xFF;

    std::string mnemonic = load ? "ldmia" : "stmia";

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rb, false));
    mnemonic.append("!,");
    mnemonic.append(rlistString(rlist));

    return mnemonic;
}

std::string Disassembler::conditionalBranch(u16 instr, u32 pc)
{
    int condition = (instr >> 8) & 0xF;
    int offset = instr & 0xFF;
    
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
    case COND_AL: mnemonic = "b";   break;
    case COND_NV: mnemonic = "b??"; break;
    }

    mnemonicPad(mnemonic);

    mnemonic.append(hex(pc + offset));

    return mnemonic;
}

std::string Disassembler::softwareInterruptThumb(u16 instr)
{
    return "unimplemented thumb swi";
}

std::string Disassembler::unconditionalBranch(u16 instr, u32 pc)
{
    int offset = instr & 0x7FF;

    offset = twos<11>(offset);
    offset <<= 1;

    std::string mnemonic = "b";

    mnemonicPad(mnemonic);
    mnemonic.append(hex(pc + offset));

    return mnemonic;
}

std::string Disassembler::longBranchLink(u16 instr, u32 lr)
{
    bool high = (instr >> 11) & 0x1;
    int offset = instr & 0x7FF;

    offset <<= 1;

    std::string mnemonic = "bl";

    mnemonicPad(mnemonic);

    if (high)
        mnemonic.append(hex(lr + offset));
    else
        mnemonic.append("<setup>");

    return mnemonic;
}

std::string Disassembler::shiftedRegister(int data)
{
    int type    = (data >> 5) & 0x3;
    int use_reg = (data >> 4) & 0x1;
    int rm      = (data >> 0) & 0xF;

    std::string result;
    result.reserve(15);
    result.append(reg(rm, true));
    switch (type)
    {
    case 0b00: result.append("lsl "); break;
    case 0b01: result.append("lsr "); break;
    case 0b10: result.append("asr "); break;
    case 0b11: result.append("ror "); break;
    }

    if (use_reg)
    {
		int rs = (data >> 8) & 0xF;
        result.append(reg(rs, false));
    }
    else
    {
        int offset = (data >> 7) & 0x1F;
        if (offset != 0)
            result.append(hex(offset));
        else
            result.pop_back();
    }
    return result;
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

    std::string cond = conditionString(instr);
    std::string addr = reg(rn, false);

    std::string mnemonic;
    mnemonic.reserve(30);
    mnemonic.append("bx");
    mnemonic.append(cond);
    mnemonicPad(mnemonic);
    mnemonic.append(addr);

    return mnemonic;
}

std::string Disassembler::branchLink(u32 instr, u32 pc)
{
    int link   = (instr >> 24) & 0x000001;
    int offset = (instr >>  0) & 0xFFFFFF;

    offset = twos<24>(offset);
    offset <<= 2;

    std::string cond = conditionString(instr);
    std::string addr = hex(pc + offset);

    std::string mnemonic;
    mnemonic.reserve(30);
    mnemonic.append(link ? "bl" : "b");
    mnemonic.append(cond);
    mnemonicPad(mnemonic);
    mnemonic.append(addr);

    return mnemonic;
}

std::string Disassembler::dataProcessing(u32 instr, u32 pc)
{
    int use_imm = (instr >> 25) & 0x001;
    int opcode  = (instr >> 21) & 0x00F;
    int flags   = (instr >> 20) & 0x001;
    int rn      = (instr >> 16) & 0x00F;
    int rd      = (instr >> 12) & 0x00F;
    int data    = (instr >>  0) & 0xFFF;

    std::string dst = reg(rd, true);
    std::string op1 = reg(rn, true);
    std::string op2;

    if (use_imm)
    {
        u32 value = rotatedImmediate(data);

        if (rn == 15)
        {
            switch (opcode)
            {
            case 0b0010:
                op2 = "=" + hex(pc - value);
                break;

            case 0b0100:
                op2 = "=" + hex(pc + value);
                break;

            default:
                op2 = hex(value);
                break;
            }
        }
        else
        {
            op2 = hex(value);
        }
    }
    else
    {
        op2 = shiftedRegister(data);
    }

    std::string mnemonic;
    mnemonic.reserve(30);
    switch (opcode)
    {
    case 0b0000: mnemonic.append("and"); break;
    case 0b0001: mnemonic.append("eor"); break;
    case 0b0010: mnemonic.append("sub"); break;
    case 0b0011: mnemonic.append("rsb"); break;
    case 0b0100: mnemonic.append("add"); break;
    case 0b0101: mnemonic.append("adc"); break;
    case 0b0110: mnemonic.append("sbc"); break;
    case 0b0111: mnemonic.append("rbc"); break;
    case 0b1000: mnemonic.append("tst"); break;
    case 0b1001: mnemonic.append("teq"); break;
    case 0b1010: mnemonic.append("cmp"); break;
    case 0b1011: mnemonic.append("cmn"); break;
    case 0b1100: mnemonic.append("orr"); break;
    case 0b1101: mnemonic.append("mov"); break;
    case 0b1110: mnemonic.append("bic"); break;
    case 0b1111: mnemonic.append("mvn"); break;
    }

    mnemonic.append(conditionString(instr));

    switch (opcode)
    {
    case 0b1000:
    case 0b1001:
    case 0b1010:
    case 0b1011:
        break;

    default:
        if (flags)
            mnemonic.append("s");
        break;
    }

    mnemonicPad(mnemonic);

    switch (opcode)
    {
    case 0b0010:
    case 0b0100:
        mnemonic.append(dst);
        if (rn != 15 || !use_imm)
            mnemonic.append(op1);
        mnemonic.append(op2);
        break;

    case 0b1000:
    case 0b1001:
    case 0b1010:
    case 0b1011:
        mnemonic.append(reg(rn, true));
        mnemonic.append(op2);
        break;

    case 0b1101:
    case 0b1111:
        mnemonic.append(reg(rd, true));
        mnemonic.append(op2);
        break;

    default:
        mnemonic.append(reg(rd, true));
        mnemonic.append(reg(rn, true));
        mnemonic.append(op2);
        break;
    }

    return mnemonic;
}

std::string Disassembler::psrTransfer(u32 instr)
{
    int write    = (instr >> 21) & 0x1;
    int use_spsr = (instr >> 22) & 0x1;

    std::string mnemonic = write ? "msr" : "mrs";

    mnemonic.append(conditionString(instr));
    mnemonicPad(mnemonic);

    if (write)
    {
		int use_imm = (instr >> 25) & 0x1;
		int data    = instr & 0xFFF;

        mnemonic.append(use_spsr ? "spsr_" : "cpsr_");

        if (instr & (1 << 19))
            mnemonic.append("f");
        if (instr & (1 << 18))
            mnemonic.append("s");
        if (instr & (1 << 17))
            mnemonic.append("x");
        if (instr & (1 << 16))
            mnemonic.append("c");

        mnemonic.append(",");

        if (use_imm)
        {
            int data = instr & 0xFFF;
            mnemonic.append(hex(rotatedImmediate(data)));
        }
        else
        {
            int reg_op = instr & 0xF;
            mnemonic.append(reg(reg_op, false));
        }
    }
    else
    {
        int reg_dst = (instr >> 12) & 0xF;
        mnemonic.append(reg(reg_dst, true));
        mnemonic.append(use_spsr ? "spsr" : "cpsr");
    }
    return mnemonic;
}

std::string Disassembler::multiply(u32 instr)
{
	int accumulate = (instr >> 21) & 0x1;
	int set_flags  = (instr >> 20) & 0x1;;
	int reg_dst    = (instr >> 16) & 0xF;
	int reg_acc    = (instr >> 12) & 0xF;
	int reg_op1    = (instr >> 8) & 0xF;;
	int reg_op2    = instr & 0xF;

    std::string mnemonic = accumulate ? "mla" : "mul";
    
    mnemonic.append(conditionString(instr));
    if (set_flags)
        mnemonic.append("s");
    mnemonicPad(mnemonic);
    mnemonic.append(reg(reg_dst, true));
    mnemonic.append(reg(reg_op1, true));
    mnemonic.append(reg(reg_op2, accumulate));

    if (accumulate)
        mnemonic.append(reg(reg_acc, false));

    return mnemonic;
}

std::string Disassembler::multiplyLong(u32 instr)
{
	int sign	   = (instr >> 22) & 0x1;
	int accumulate = (instr >> 21) & 0x1;
	int set_flags  = (instr >> 20) & 0x1;
	int reg_dsthi  = (instr >> 16) & 0xF;
	int reg_dstlo  = (instr >> 12) & 0xF;
	int reg_op1    = (instr >> 8) & 0xF;
	int reg_op2    = instr & 0xF;

    std::string mnemonic;

    switch ((sign << 1) | accumulate)
    {
    case 0b00: mnemonic = "umull"; break;
    case 0b01: mnemonic = "umlal"; break;
    case 0b10: mnemonic = "smull"; break;
    case 0b11: mnemonic = "smlal"; break;
    }

    mnemonic.append(conditionString(instr));
    if (set_flags)
        mnemonic.append("s");
    mnemonicPad(mnemonic);
    mnemonic.append(reg(reg_dstlo, true));
    mnemonic.append(reg(reg_dsthi, true));
    mnemonic.append(reg(reg_op1, true));
    mnemonic.append(reg(reg_op2, false));

    return mnemonic;
}

std::string Disassembler::singleDataTransfer(u32 instr)
{
    bool use_reg = (instr >> 25) & 0x1;
    bool pre_indexing = (instr >> 24) & 0x1;
    bool increment = (instr >> 23) & 0x1;
    int byte = (instr >> 22) & 0x1;
    bool writeback = (instr >> 21) & 0x1;
    int load = (instr >> 20) & 0x1;
    int rn = (instr >> 16) & 0xF;
    int rd = (instr >> 12) & 0xF;

    std::string offset;
    if (use_reg)
        offset = shiftedRegister(instr & 0xFFF);
    else
        offset = hex(instr & 0xFFF);

    std::string mnemonic;

    switch (load << 1 | byte)
    {
    case 0b00: mnemonic = "str";  break;
    case 0b01: mnemonic = "strb"; break;
    case 0b10: mnemonic = "ldr";  break;
    case 0b11: mnemonic = "ldrb"; break;
    }

    mnemonic.append(conditionString(instr));
    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(rn, pre_indexing));

    if (pre_indexing)
    {
        mnemonic.append(offset);
        mnemonic.append("]");
        if (writeback)
            mnemonic.append("!");
    }
    else
    {
        mnemonic.append("],");
        mnemonic.append(offset);
    }
    return mnemonic;
}

std::string Disassembler::halfSignedDataTransfer(u32 instr)
{
    bool pre_indexing = (instr >> 24) & 0x1;
    bool immediate = (instr >> 22) & 0x1;
    bool increment = (instr >> 23) & 0x1;
    bool writeback = (instr >> 21) & 0x1;
    bool load = (instr >> 20) & 0x1;
    int rn = (instr >> 16) & 0xF;
    int rd = (instr >> 12) & 0xF;
    int sign = (instr >> 6) & 0x1;
    int half = (instr >> 5) & 0x1;

    std::string offset;
    if (immediate)
    {
        int upper = (instr >> 8) & 0xF;
        int lower = instr & 0xF;
        offset = hex(upper << 4 | lower);
    }
    else
    {
        int rm = instr & 0xF;
        offset = reg(rm, false);
    }

    std::string mnemonic;

    switch (sign << 1 | half)
    {
    case 0b00: mnemonic = "???"; break;
    case 0b01: mnemonic = load ? "ldrh" : "strh"; break;
    case 0b10: mnemonic = "ldrsb"; break;
    case 0b11: mnemonic = "ldrsh"; break;
    }

    mnemonic.append(conditionString(instr));
    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(rn, pre_indexing));

    if (pre_indexing)
    {
        mnemonic.append(offset);
        mnemonic.append("]");
        if (writeback)
            mnemonic.append("!");
    }
    else
    {
        mnemonic.append("],");
        mnemonic.append(offset);
    }
    return mnemonic;
}

std::string Disassembler::blockDataTransfer(u32 instr)
{
    int full = (instr >> 24) & 0x1;
    int ascending = (instr >> 23) & 0x1;
    bool user_transfer = (instr >> 22) & 0x1;
    bool writeback = (instr >> 21) & 0x1;
    bool load = (instr >> 20) & 0x1;
    int rn = (instr >> 16) & 0xF;
    int rlist = instr & 0xFFFF;

    std::string mnemonic = load ? "ldm" : "stm";
    switch ((full << 1) | ascending)
    {
    // Todo: Fix me :)
    case 0b00: mnemonic += "ed"; break;
    case 0b01: mnemonic += "ea"; break;
    case 0b10: mnemonic += "fd"; break;
    case 0b11: mnemonic += "fa"; break;
    }

    mnemonic.append(conditionString(instr));
    mnemonicPad(mnemonic);
    mnemonic.append(reg(rn, !writeback));
    if (writeback)
        mnemonic.append("!,");
    mnemonic.append(rlistString(rlist));
    if (user_transfer)
        mnemonic.append("^");

    return mnemonic;
}

std::string Disassembler::singleDataSwap(u32 instr)
{
    bool byte = (instr >> 22) & 0x1;
    int rb = (instr >> 16) & 0xF;
    int rd = (instr >> 12) & 0xF;
    int rs = instr & 0xF;

    std::string mnemonic = byte ? "swpb" : "swp";

    mnemonic.append(conditionString(instr));
    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append(reg(rs, true));
    mnemonic.append("[");
    mnemonic.append(reg(rb, false));
    mnemonic.append("]");

    return mnemonic;
}

std::string Disassembler::swiArm(u32 instr)
{
    return "unimplemented thumb arm";
}
