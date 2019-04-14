#include "disassembler.h"

#include "common/utility.h"

std::string Disassembler::disassemble(u32 instr, Format format)
{
    switch (format)
    {
    case THUMB_1:  return moveShiftedRegister(instr);
    case THUMB_2:  return addSubImmediate(instr);
    case THUMB_3:  return addSubMovCmpImmediate(instr);
    case THUMB_4:  return aluOperations(instr);
    case THUMB_5:  return highRegisterBranchExchange(instr);
    case THUMB_6:  return loadPcRelative(instr);
    case THUMB_7:  return loadStoreRegisterOffset(instr);
    case THUMB_8:  return loadStoreHalfSignExtended(instr);
    case THUMB_9:  return loadStoreImmediateOffset(instr);
    case THUMB_10: return loadStoreHalf(instr);
    case THUMB_11: return loadStoreSpRelative(instr);
    case THUMB_12: return loadAddress(instr);
    case THUMB_13: return addOffsetSp(instr);
    case THUMB_14: return pushPopRegisters(instr);
    case THUMB_15: return loadStoreMultiple(instr);
    case THUMB_16: return conditionalBranch(instr);
    //case THUMB_17: return softwareInterruptBreakpoint(instr);
    //case THUMB_18: return unconditionalBranch(instr);
    //case THUMB_19: return longBranchLink(instr);
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

void Disassembler::padMnemonic(std::string& mnemonic)
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

    padMnemonic(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append(reg(rs, true));
    mnemonic.append(std::to_string(offset));

    return mnemonic;
}

std::string Disassembler::addSubImmediate(u16 instr)
{
    bool immediate = (instr >> 10) & 0x1;
    bool subtract = (instr >> 9) & 0x1;

    int imm3 = (instr >> 6) & 0x7;

    int rs = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    std::string mnemonic;

    bool move = immediate && imm3 == 0;

    if (move)
        mnemonic = "mov";
    else
        mnemonic = subtract ? "sub" : "add";

    padMnemonic(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append(reg(rs, !move));

    if (!move)
        mnemonic.append(immediate ? std::to_string(imm3) : reg(imm3, false));

    return mnemonic;
}

std::string Disassembler::addSubMovCmpImmediate(u16 instr)
{
    int opcode = (instr >> 11) & 0x3;
    int offset = instr & 0xFF;

    int rd = (instr >> 8) & 0x7;

    std::string mnemonic;
    switch (opcode)
    {
    case 0b00: mnemonic = "mov"; break;
    case 0b01: mnemonic = "cmp"; break;
    case 0b10: mnemonic = "add"; break;
    case 0b11: mnemonic = "sub"; break;
    }

    padMnemonic(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append(std::to_string(offset));

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

    padMnemonic(mnemonic);
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

    padMnemonic(mnemonic);

    if (opcode != 0b11)
        mnemonic.append(reg(rd, true));
    mnemonic.append(reg(rs, false));

    return mnemonic;
}

std::string Disassembler::loadPcRelative(u16 instr)
{
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;

    offset <<= 2;

    std::string mnenonic = "ldr";

    padMnemonic(mnenonic);
    mnenonic.append(reg(rd, true));
    mnenonic.append("[pc,");
    mnenonic.append(std::to_string(offset));
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

    padMnemonic(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(rb, true));
    mnemonic.append(reg(ro, false));
    mnemonic.append("]");

    return mnemonic;
}

std::string Disassembler::loadStoreHalfSignExtended(u16 instr)
{
    int half = (instr >> 11) & 0x1;
    int sign = (instr >> 10) & 0x1;

    int ro = (instr >> 6) & 0x7;
    int rb = (instr >> 3) & 0x7;
    int rd = instr & 0x7;

    std::string mnemonic;
    switch (sign << 1 | half)
    {
    case 0b00: mnemonic = "strh"; break;
    case 0b01: mnemonic = "ldrh"; break;
    case 0b10: mnemonic = "ldsb"; break;
    case 0b11: mnemonic = "ldsh"; break;
    }

    padMnemonic(mnemonic);
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

    padMnemonic(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(rb, true));
    mnemonic.append(std::to_string(offset));
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

    padMnemonic(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(rb, true));
    mnemonic.append(std::to_string(offset));
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

    padMnemonic(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[sp,");
    mnemonic.append(std::to_string(offset));
    mnemonic.append("]");

    return mnemonic;
}

std::string Disassembler::loadAddress(u16 instr)
{
    bool sp = (instr >> 11) & 0x1;
    
    int rd = (instr >> 8) & 0x7;
    int offset = instr & 0xFF;

    offset <<= 2;

    std::string mnemonic = "add";

    padMnemonic(mnemonic);
    mnemonic.append(reg(rd, true));
    mnemonic.append("[");
    mnemonic.append(reg(sp ? 14 : 15, true));
    mnemonic.append(std::to_string(offset));
    mnemonic.append("]");

    return mnemonic;
}

std::string Disassembler::addOffsetSp(u16 instr)
{
    bool sign = instr >> 7 & 0x1;

    int offset = instr & 0x3F;

    offset <<= 2;

    std::string mnemonic = "add";

    padMnemonic(mnemonic);
    mnemonic.append("sp,");
    if (sign)
        mnemonic.append("-");
    mnemonic.append(std::to_string(offset));

    return mnemonic;
}

std::string Disassembler::pushPopRegisters(u16 instr)
{
    bool pop = instr >> 11 & 0x1;
    bool pc_lr = instr >> 8 & 0x1;

    int rlist = instr & 0xFF;

    std::string mnemonic = pop ? "pop" : "push";

    padMnemonic(mnemonic);
    mnemonic.append("{");
    for (int x = 0; x < 8; ++x)
    {
        if (rlist & (1 << x))
            mnemonic.append(reg(x, x != 7));
    }
    if (pc_lr)
        mnemonic.append(pop ? ",pc" : ",sp");
    mnemonic.append("}");

    return mnemonic;
}

std::string Disassembler::loadStoreMultiple(u16 instr)
{
    bool load = (instr >> 11) & 0x1;

    int rb = (instr >> 8) & 0x7;
    int rlist = instr & 0xFF;

    std::string mnemonic = load ? "ldmia" : "stmia";

    padMnemonic(mnemonic);
    mnemonic.append(reg(rb, false));
    mnemonic.append("!,{");
    for (int x = 0; x < 8; ++x)
    {
        if (rlist & (1 << x))
            mnemonic.append(reg(x, x != 7));
    }
    mnemonic.append("}");

    return mnemonic;
}

std::string Disassembler::conditionalBranch(u16 instr)
{
    // Todo: process SWI
    Condition cond = static_cast<Condition>((instr >> 8) & 0xF);
    
    int offset = instr & 0xFF;
    
    offset = twos2<8>(offset);
    offset <<= 1;

    std::string mnemonic;
    switch (cond)
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

    padMnemonic(mnemonic);

    mnemonic.append(std::to_string(offset));

    return mnemonic;
}
