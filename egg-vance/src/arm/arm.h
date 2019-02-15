#pragma once

#include "common/integer.h"
#include "common/internal.h"
#include "mmu/mmu.h"
#include "enums.h"
#include "registers.h"

class ARM : public Internal
{
public:
    ARM();

    void reset() final;

    void step();

    bool running;

    MMU* mmu;

private:
    Registers regs;

    u32& reg(int number);
    u32& spsr(int number);

    void fetch();
    void decode();
    void execute();
    void advance();

    struct PipeItem
    {
        u32 instr;
        Instruction decoded;
    } pipe[3];

    bool needs_flush;

    void flushPipe();

    Mode currentMode() const;

    bool isArm() const;
    bool isThumb() const;

    int flagZ() const;
    int flagN() const;
    int flagC() const;
    int flagV() const;

    void setFlag(CPSR flag, bool set);
    void setFlagZ(bool set);
    void setFlagN(bool set);
    void setFlagC(bool set);
    void setFlagV(bool set);

    void updateFlagZ(u32 result);
    void updateFlagN(u32 result);
    void updateFlagC(int carry);
    void updateFlagC(u32 input, u32 operand, bool addition);
    void updateFlagV(u32 input, u32 operand, bool addition);

    u32 LSL(u32 value, int offset);
    u32 LSR(u32 value, int offset);
    u32 ASR(u32 value, int offset);
    u32 ROR(u32 value, int offset);

    bool checkCondition(Condition condition) const;

    void moveShiftedRegister(u16 instr);
    void addSubImmediate(u16 instr);
    void moveCmpAddSubImmediate(u16 instr);
    void aluOperations(u16 instr);
    void highRegisterBranchExchange(u16 instr);
    void loadPcRelative(u16 instr);
    void loadStoreRegisterOffset(u16 instr);
    void loadStoreSignExtended(u16 instr);
    void loadStoreImmediateOffset(u16 instr);
    void loadStoreHalfword(u16 instr);
    void loadStoreSpRelative(u16 instr);
    void loadAddress(u16 instr);
    void addOffsetSp(u16 instr);
    void pushPopRegisters(u16 instr);
    void multipleLoadStore(u16 instr);
    void conditionalBranch(u16 instr);
    void softwareInterruptThumb(u16 instr);
    void unconditionalBranch(u16 instr);
    void longBranchLink(u16 instr);

    void branchExchange(u32 instr);
    void branchLink(u32 instr);
};
