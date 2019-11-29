#pragma once

#include <array>

#include "registers/haltcontrol.h"
#include "registers/intenabled.h"
#include "registers/intmaster.h"
#include "registers/intrequest.h"
#include "registers/waitcontrol.h"
#include "system/dmacontroller.h"
#include "system/timercontroller.h"
#include "registers.h"

enum class Interrupt
{
    VBlank  = 1 <<  0,
    HBlank  = 1 <<  1,
    VMatch  = 1 <<  2,
    Timer0  = 1 <<  3,
    Timer1  = 1 <<  4,
    Timer2  = 1 <<  5,
    Timer3  = 1 <<  6,
    Serial  = 1 <<  7,
    DMA0    = 1 <<  8,
    DMA1    = 1 <<  9,
    DMA2    = 1 << 10,
    DMA3    = 1 << 11,
    Keypad  = 1 << 12,
    GamePak = 1 << 13
};

class ARM : public Registers
{
public:
    void reset();

    void run(int cycles);
    void interrupt(Interrupt flag);

    DMAController   dma;
    TimerController timer;

    struct IO
    {
        IntMaster   int_master;
        IntEnabled  int_enabled;
        IntRequest  int_request;
        WaitControl waitcnt;
        HaltControl haltcnt;
    } io;

    u32 pipe[2];

private:
    enum class Shift
    {
        LSL = 0b00,
        LSR = 0b01,
        ASR = 0b10,
        ROR = 0b11
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

    u32 lsl(u32 value, int amount, bool& carry) const;
    u32 lsr(u32 value, int amount, bool& carry, bool immediate) const;
    u32 asr(u32 value, int amount, bool& carry, bool immediate) const;
    u32 ror(u32 value, int amount, bool& carry, bool immediate) const;
    u32 shift(Shift type, u32 value, int amount, bool& carry, bool immediate) const;

    u32 logical(u32 result, bool flags);
    u32 logical(u32 result, bool carry, bool flags);

    u32 add(u32 op1, u32 op2, bool flags);
    u32 sub(u32 op1, u32 op2, bool flags);
    u32 adc(u64 op1, u64 op2, bool flags);
    u32 sbc(u64 op1, u64 op2, bool flags);

    void execute();
    void disasm();

    void flushPipeHalf();
    void flushPipeWord();

    void idle();
    void booth(u32 multiplier, bool ones);

    void doInterrupt(u32 pc, u32 lr, PSR::Mode mode);
    void interruptHW();
    void interruptSW();

    void Arm_BranchExchange(u32 instr);
    template<int link>
    void Arm_BranchLink(u32 instr);
    template<int flags, int opcode, int imm_op>
    void Arm_DataProcessing(u32 instr);
    template<int write, int use_spsr, int imm_op>
    void Arm_StatusTransfer(u32 instr);
    template<int flags, int accumulate>
    void Arm_Multiply(u32 instr);
    template<int flags, int accumulate, int sign>
    void Arm_MultiplyLong(u32 instr);
    template<int load, int writeback, int byte, int increment, int pre_index, int reg_offset>
    void Arm_SingleDataTransfer(u32 instr);
    template<int opcode, int load, int writeback, int imm_offset, int increment, int pre_index>
    void Arm_HalfSignedDataTransfer(u32 instr);
    template<int load, int writeback, int user_mode, int increment, int pre_index>
    void Arm_BlockDataTransfer(u32 instr);
    template<int byte>
    void Arm_SingleDataSwap(u32 instr);
    void Arm_SoftwareInterrupt(u32 instr);
    void Arm_CoprocessorDataOperations(u32 instr);
    void Arm_CoprocessorDataTransfers(u32 instr);
    void Arm_CoprocessorRegisterTransfers(u32 instr);
    void Arm_Undefined(u32 instr);

    template<int offset, int opcode>
    void Thumb_MoveShiftedRegister(u16 instr);
    template<int rn, int opcode>
    void Thumb_AddSubtract(u16 instr);
    template<int rd, int opcode>
    void Thumb_ImmediateOperations(u16 instr);
    template<int opcode>
    void Thumb_ALUOperations(u16 instr);
    template<int hs, int hd, int opcode>
    void Thumb_HighRegisterOperations(u16 instr);
    template<int rd>
    void Thumb_LoadPCRelative(u16 instr);
    template<int ro, int opcode>
    void Thumb_LoadStoreRegisterOffset(u16 instr);
    template<int ro, int opcode>
    void Thumb_LoadStoreByteHalf(u16 instr);
    template<int offset, int opcode>
    void Thumb_LoadStoreImmediateOffset(u16 instr);
    template<int offset, int load>
    void Thumb_LoadStoreHalf(u16 instr);
    template<int rd, int load>
    void Thumb_LoadStoreSPRelative(u16 instr);
    template<int rd, int use_sp>
    void Thumb_LoadRelativeAddress(u16 instr);
    template<int sign>
    void Thumb_AddOffsetSP(u16 instr);
    template<int special, int pop>
    void Thumb_PushPopRegisters(u16 instr);
    template<int rb, int load>
    void Thumb_LoadStoreMultiple(u16 instr);
    template<int condition>
    void Thumb_ConditionalBranch(u16 instr);
    void Thumb_SoftwareInterrupt(u16 instr);
    void Thumb_UnconditionalBranch(u16 instr);
    template<int second>
    void Thumb_LongBranchLink(u16 instr);
    void Thumb_Undefined(u16 instr);

    int remaining;
    int instr_size;
    u32 last_addr;

    static std::array<void(ARM::*)(u32), 4096> instr_arm;
    static std::array<void(ARM::*)(u16), 1024> instr_thumb;
};

extern ARM arm;
