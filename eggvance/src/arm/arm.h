#pragma once

#include "registers.h"

class Memory;

class ARM : public Registers
{
public:
    void reset();

    int emulate();

    Memory* mem;

private:
    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    enum AccessType
    {
        NSEQ,
        SEQ
    };

    void hardwareInterrupt();
    void softwareInterrupt();
    void interrupt(u32 pc, u32 lr, PSR::Mode mode);

    int length() const;

    void execute();
    void advance();
    void debug();

    void logical(u32 result);
    void logical(u32 result, bool carry);
    void arithmetic(u32 op1, u32 op2, bool addition);

    enum Arithmetic
    {
        ADD,
        SUB
    };

    // Temporary
    template<Arithmetic arith>
    void arithmetic(u32 op1, u32 op2)
    {
        arithmetic(op1, op2, arith == ADD);
    }

    // Temporary
    template<AccessType type>
    void cycle(u32 addr)
    {
        cycle(addr, type);
    }

    u32 lsl(u32 value, int amount, bool& carry) const;
    u32 lsr(u32 value, int amount, bool& carry, bool immediate = true) const;
    u32 asr(u32 value, int amount, bool& carry, bool immediate = true) const;
    u32 ror(u32 value, int amount, bool& carry, bool immediate = true) const;

    u32 ldr(u32 addr);
    u32 ldrh(u32 addr);
    u32 ldrsh(u32 addr);

    void cycle();
    void cycle(u32 addr, AccessType access);
    void cycleBooth(u32 multiplier, bool allow_ones);

    void moveShiftedRegister(u16 instr);
    void addSubtractImmediate(u16 instr);
    void addSubtractMoveCompareImmediate(u16 instr);
    void aluOperations(u16 instr);
    void highRegisterBranchExchange(u16 instr);
    void loadPCRelative(u16 instr);
    void loadStoreRegisterOffset(u16 instr);
    void loadStoreHalfwordSigned(u16 instr);
    void loadStoreImmediateOffset(u16 instr);
    void loadStoreHalfword(u16 instr);
    void loadStoreSPRelative(u16 instr);
    void loadAddress(u16 instr);
    void addOffsetSP(u16 instr);
    void pushPopRegisters(u16 instr);
    void loadStoreMultiple(u16 instr);
    void conditionalBranch(u16 instr);
    void unconditionalBranch(u16 instr);
    void longBranchLink(u16 instr);

    void branchExchange(u32 instr);
    void branchLink(u32 instr);
    void dataProcessing(u32 instr);
    void psrTransfer(u32 instr);
    void multiply(u32 instr);
    void multiplyLong(u32 instr);
    void singleDataTransfer(u32 instr);
    void halfwordSignedDataTransfer(u32 instr);
    void blockDataTransfer(u32 instr);
    void singleDataSwap(u32 instr);

    u64 cycles;
};
