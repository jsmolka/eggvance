#pragma once

#include "common/integer.h"
#include "mmu/mmu.h"
#include "registers.h"

class ARM
{
public:
    ARM(MMU& mmu);

    void reset();

    void interrupt();

    int step();

private:
    enum AccessType
    {
        NSEQ,
        SEQ
    };

    enum ExceptionVector
    {
        EXV_RST = 0x00,
        EXV_SWI = 0x08,
        EXV_IRQ = 0x18,
    };

    MMU& mmu;
    Registers regs;

    void execute();
    void advance();

    void debug();

    void logical(u32 result);
    void logical(u32 result, bool carry);
    void arithmetic(u32 op1, u32 op2, bool addition);

    u32 lsl(u32 value, int offset, bool& carry);
    u32 lsr(u32 value, int offset, bool& carry, bool immediate = true);
    u32 asr(u32 value, int offset, bool& carry, bool immediate = true);
    u32 ror(u32 value, int offset, bool& carry, bool immediate = true);

    u32 ldr(u32 addr);
    u32 ldrh(u32 addr);
    u32 ldrsh(u32 addr);

    int cycles;
    u64 cycles_total;

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
    void softwareInterruptThumb(u16 instr);
    void unconditionalBranch(u16 instr);
    void longBranchLink(u16 instr);

    u32 shiftedRegister(int data, bool& carry);
    u32 rotatedImmediate(int data, bool& carry);

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
    void softwareInterruptArm(u32 instr);
};
