#pragma once

#include <string>

#include "common/integer.h"
#include "enums.h"
#include "registers.h"

class Disassembler
{
public:
    static std::string disassemble(u32 data, const Registers& regs);

private:
    static void mnemonicPad(std::string& mnemonic);

    static std::string reg(int number);
    static std::string hex(u32 value);
    static std::string list(int rlist);
    static std::string cond(u32 instr);

    static std::string moveShiftedRegister(u16 instr);
    static std::string addSubImmediate(u16 instr);
    static std::string addSubMovCmpImmediate(u16 instr);
    static std::string aluOperations(u16 instr);
    static std::string highRegisterBranchExchange(u16 instr);
    static std::string loadPcRelative(u16 instr, u32 pc);
    static std::string loadStoreRegisterOffset(u16 instr);
    static std::string loadStoreHalfSigned(u16 instr);
    static std::string loadStoreImmediateOffset(u16 instr);
    static std::string loadStoreHalf(u16 instr);
    static std::string loadStoreSpRelative(u16 instr);
    static std::string loadAddress(u16 instr, u32 pc);
    static std::string addOffsetSp(u16 instr);
    static std::string pushPopRegisters(u16 instr);
    static std::string loadStoreMultiple(u16 instr);
    static std::string conditionalBranch(u16 instr, u32 pc);
    static std::string swiThumb(u16 instr);
    static std::string unconditionalBranch(u16 instr, u32 pc);
    static std::string longBranchLink(u16 instr, u32 lr);

    static std::string shiftedRegister(int data);
    static u32 rotatedImmediate(int data);

    static std::string branchExchange(u32 instr);
    static std::string branchLink(u32 instr, u32 pc);
    static std::string dataProcessing(u32 instr, u32 pc);
    static std::string psrTransfer(u32 instr);
    static std::string multiply(u32 instr);
    static std::string multiplyLong(u32 instr);
    static std::string singleTransfer(u32 instr);
    static std::string halfSignedTransfer(u32 instr);
    static std::string blockTransfer(u32 instr);
    static std::string singleSwap(u32 instr);
    static std::string swiArm(u32 instr);
};
