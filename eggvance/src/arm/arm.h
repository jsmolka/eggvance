#pragma once

#include <array>

#include "registers/haltcontrol.h"
#include "registers/waitcontrol.h"
#include "registers.h"

class ARM : public Registers
{
public:
    friend class DMA;
    friend class IO;
    friend class IRQHandler;

    void reset();

    void run(uint cycles);

    void updateDispatch();

    u32 pipe[2];

private:
    using Dispatch = void(ARM::*)(void);

    Dispatch dispatch;

    enum class Shift
    {
        Lsl = 0b00,
        Lsr = 0b01,
        Asr = 0b10,
        Ror = 0b11
    };

    bool isSequential(u32 addr) const;

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    u32 readWordRotated(u32 addr);
    u32 readHalfRotated(u32 addr);
    u32 readHalfSigned(u32 addr);

    void flushHalf();
    void flushWord();

    template<uint flags>
    void execute();

    void disasm();

    void idle();
    void booth(u32 multiplier, bool ones);

    void interrupt(u32 pc, u32 lr, PSR::Mode mode);
    void interruptHW();
    void interruptSW();

    void Arm_BranchExchange(u32 instr);
    template<uint link>
    void Arm_BranchLink(u32 instr);
    template<uint flags, uint opcode, uint imm_op>
    void Arm_DataProcessing(u32 instr);
    template<uint write, uint use_spsr, uint imm_op>
    void Arm_StatusTransfer(u32 instr);
    template<uint flags, uint accumulate>
    void Arm_Multiply(u32 instr);
    template<uint flags, uint accumulate, uint sign>
    void Arm_MultiplyLong(u32 instr);
    template<uint load, uint writeback, uint byte, uint increment, uint pre_index, uint imm_op>
    void Arm_SingleDataTransfer(u32 instr);
    template<uint opcode, uint load, uint writeback, uint imm_op, uint increment, uint pre_index>
    void Arm_HalfSignedDataTransfer(u32 instr);
    template<uint load, uint writeback, uint user_mode, uint increment, uint pre_index>
    void Arm_BlockDataTransfer(u32 instr);
    template<uint byte>
    void Arm_SingleDataSwap(u32 instr);
    void Arm_SoftwareInterrupt(u32 instr);
    void Arm_CoprocessorDataOperations(u32 instr);
    void Arm_CoprocessorDataTransfers(u32 instr);
    void Arm_CoprocessorRegisterTransfers(u32 instr);
    void Arm_Undefined(u32 instr);

    template<uint amount, uint opcode>
    void Thumb_MoveShiftedRegister(u16 instr);
    template<uint rn, uint opcode>
    void Thumb_AddSubtract(u16 instr);
    template<uint rd, uint opcode>
    void Thumb_ImmediateOperations(u16 instr);
    template<uint opcode>
    void Thumb_AluOperations(u16 instr);
    template<uint hs, uint hd, uint opcode>
    void Thumb_HighRegisterOperations(u16 instr);
    template<uint rd>
    void Thumb_LoadPcRelative(u16 instr);
    template<uint ro, uint opcode>
    void Thumb_LoadStoreRegisterOffset(u16 instr);
    template<uint ro, uint opcode>
    void Thumb_LoadStoreByteHalf(u16 instr);
    template<uint amount, uint opcode>
    void Thumb_LoadStoreImmediateOffset(u16 instr);
    template<uint amount, uint load>
    void Thumb_LoadStoreHalf(u16 instr);
    template<uint rd, uint load>
    void Thumb_LoadStoreSpRelative(u16 instr);
    template<uint rd, uint use_sp>
    void Thumb_LoadRelativeAddress(u16 instr);
    template<uint sign>
    void Thumb_AddOffsetSp(u16 instr);
    template<uint rbit, uint pop>
    void Thumb_PushPopRegisters(u16 instr);
    template<uint rb, uint load>
    void Thumb_LoadStoreMultiple(u16 instr);
    template<uint condition>
    void Thumb_ConditionalBranch(u16 instr);
    void Thumb_SoftwareInterrupt(u16 instr);
    void Thumb_UnconditionalBranch(u16 instr);
    template<uint second>
    void Thumb_LongBranchLink(u16 instr);
    void Thumb_Undefined(u16 instr);

    int cycles;
    u32 last_addr;

    struct IO
    {
        WaitControl waitcnt;
        HaltControl haltcnt;
    } io;

    static std::array<void(ARM::*)(u32), 4096> instr_arm;
    static std::array<void(ARM::*)(u16), 1024> instr_thumb;
};

extern ARM arm;
