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

    u32& reg(u8 number);
    u32& spsr(u8 number);

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

    u8 flagZ() const;
    u8 flagN() const;
    u8 flagC() const;
    u8 flagV() const;

    void setFlagZ(bool set);
    void setFlagN(bool set);
    void setFlagC(bool set);
    void setFlagV(bool set);

    void updateFlagZ(u32 result);
    void updateFlagN(u32 result);
    void updateFlagC(u8 carry);
    void updateFlagC(u32 value, u32 operand, bool addition);
    void updateFlagV(u32 value, u32 operand, bool addition);

    u32 LSL(u32 value, u8 offset, bool flags = true);
    u32 LSR(u32 value, u8 offset, bool flags = true);
    u32 ASR(u32 value, u8 offset, bool flags = true);
    u32 ROR(u32 value, u8 offset, bool flags = true);

    u32 ADD(u32 value, u32 operand, bool flags = true);
    u32 SUB(u32 value, u32 operand, bool flags = true);
    u32 ADC(u32 value, u32 operand, bool flags = true);
    u32 SBC(u32 value, u32 operand, bool flags = true);
    u32 MUL(u32 value, u32 operand, bool flags = true);
    u32 AND(u32 value, u32 operand, bool flags = true);
    u32 ORR(u32 value, u32 operand, bool flags = true);
    u32 EOR(u32 value, u32 operand, bool flags = true);
    u32 BIC(u32 value, u32 operand, bool flags = true);

    u32 NEG(u32 operand, bool flags = true);
    u32 MOV(u32 operand, bool flags = true);
    u32 MVN(u32 operand, bool flags = true);

    void CMP(u32 value, u32 operand);
    void CMN(u32 value, u32 operand);
    void TST(u32 value, u32 operand);

    void STRW(u32 addr, u32 value);
    void STRH(u32 addr, u32 value);
    void STRB(u32 addr, u32 value);
    u32 LDRW(u32 addr);
    u16 LDRH(u32 addr);
     u8 LDRB(u32 addr);
    u32 LDSH(u32 addr);
    u32 LDSB(u32 addr);

    u32 STMIA(u32 addr, u8 rlist);
    u32 LDMIA(u32 addr, u8 rlist);

    void PUSH(u8 rlist, bool lr);
    void POP(u8 rlist, bool pc);

    void BX(u32 value);

    bool checkCondition(Condition cond) const;
    bool checkBranchCondition(Condition cond) const;

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
