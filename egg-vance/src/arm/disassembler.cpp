#include "disassembler.h"

#include <iomanip>
#include <sstream>

#include "utility.h"

std::string Disassembler::disassemble(u32 instr, Format format, u32 pc)
{
    switch (format)
    {
    case THUMB_1:  return moveShiftedRegister(instr);
    case THUMB_2:  return addSubImmediate(instr);
    case THUMB_3:  return addSubCmpMovImmediate(instr);
    case THUMB_4:  return aluOperations(instr);
    case THUMB_5:  return highRegisterBranchExchange(instr);
    case THUMB_6:  return loadPcRelative(instr, pc);
    case THUMB_7:  return loadStoreRegisterOffset(instr);
    case THUMB_8:  return loadStoreHalfSigned(instr);
    case THUMB_9:  return loadStoreImmediateOffset(instr);
    case THUMB_10: return loadStoreHalf(instr);
    case THUMB_11: return loadStoreSpRelative(instr);
    case THUMB_12: return loadAddress(instr, pc);
    case THUMB_13: return addOffsetSp(instr);
    case THUMB_14: return pushPopRegisters(instr);
    case THUMB_15: return loadStoreMultiple(instr);
    case THUMB_16: return conditionalBranch(instr, pc);
    case THUMB_17: return softwareInterruptThumb(instr);
    case THUMB_18: return unconditionalBranch(instr, pc);
    case THUMB_19: return longBranchLink(instr, pc);
    //case ARM_1:    return branchExchange(instr);
    //case ARM_2:    return branchLink(instr);
    //case ARM_3:    return dataProcessing(instr);
    //case ARM_4:    return psrTransfer(instr);
    //case ARM_5:    return multiply(instr);
    //case ARM_6:    return multiplyLong(instr);
    //case ARM_7:    return singleDataTransfer(instr);
    //case ARM_8:    return halfSignedDataTransfer(instr);
    //case ARM_9:    return blockDataTransfer(instr);
    //case ARM_10:   return singleDataSwap(instr);
    }
    return "unimpl";
}

void Disassembler::mnemonicPad(std::string& mnemonic)
{
    if (mnemonic.size() < 8)
        mnemonic.append(8 - mnemonic.size(), ' ');
}

void Disassembler::mnemonicRlist(std::string& mnemonic, int rlist)
{
    mnemonic.append("{");

    for (int x = 0; x < 16; ++x)
    {
        if (rlist & (1 << x))
            mnemonic.append(reg(x, true));
    }

    if (rlist == 0)
        mnemonic.append("}");
    else
        mnemonic.back() = '}';
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

std::string Disassembler::hex(int value)
{
    std::stringstream stream;

    if (value < 0)
    {
        value = abs(value);
        stream << "-";
    }

    stream << std::uppercase << std::hex << value << std::nouppercase << "h";

    return stream.str();
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
    bool sp = (instr >> 11) & 0x1;
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;

    offset <<= 2;

    std::string mnemonic = sp ? "add" : "adr";

    mnemonicPad(mnemonic);
    mnemonic.append(reg(rd, true));
    if (sp)
    {
        mnemonic.append(reg(14, true));
        mnemonic.append(hex(offset));
    }
    else
    {
        mnemonic.append(hex(pc + offset));
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
    bool pc_lr = (instr >> 8) & 0x1;
    int rlist = instr & 0xFF;

    if (pc_lr)
        rlist |= 1 << (pop ? 15 : 14);

    std::string mnemonic = pop ? "pop" : "push";

    mnemonicPad(mnemonic);
    mnemonicRlist(mnemonic, rlist);

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
    mnemonicRlist(mnemonic, rlist);

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
    case COND_AL: mnemonic = "bal"; break;
    case COND_NV: mnemonic = "bnv"; break;
    }

    mnemonicPad(mnemonic);

    mnemonic.append(hex(pc + offset));

    return mnemonic;
}

std::string Disassembler::softwareInterruptThumb(u16 instr)
{
    return std::string();  // Todo: implement
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

std::string Disassembler::longBranchLink(u16 instr, u32 pc)
{
    bool high = (instr >> 11) & 0x1;
    int offset = instr & 0x7FF;

    std::string mnemonic = "bl";

    mnemonicPad(mnemonic);

    if (high)
        mnemonic.append(hex(pc));
    else
        mnemonic.append("<lower>");

    return mnemonic;
}
