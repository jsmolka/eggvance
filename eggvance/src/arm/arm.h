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
    enum class Access
    {
        Seq    = 0,
        Nonseq = 1,
    };

    enum class Shift
    {
        LSL = 0b00,
        LSR = 0b01,
        ASR = 0b10,
        ROR = 0b11
    };

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    // Todo: remove
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
    void debug();

    template<int width>
    inline void advance();
    inline void advance();

    // Todo: rename once finished
    void logical_old(u32 result);
    void logical_old(u32 result, bool carry);
    
    u32 logical(u32 result, bool flags);
    u32 logical(u32 result, bool carry, bool flags);

    u32 add(u32 op1, u32 op2, bool flags);
    u32 sub(u32 op1, u32 op2, bool flags);

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

    template<Access access>
    void cycle(u32 addr)
    {
        switch (access)
        {
        case Access::Seq:
            cycle(addr, SEQ);
            break;

        case Access::Nonseq:
            cycle(addr, NSEQ);
            break;
        }
    }

    u32 lsl(u32 value, int amount, bool& carry) const;
    u32 lsr(u32 value, int amount, bool& carry, bool immediate = true) const;
    u32 asr(u32 value, int amount, bool& carry, bool immediate = true) const;
    u32 ror(u32 value, int amount, bool& carry, bool immediate = true) const;
    u32 shift(Shift type, u32 value, int amount, bool& carry, bool immediate = true) const;

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

#include "arm.inl"
