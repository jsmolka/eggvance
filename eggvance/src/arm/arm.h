#pragma once

#include <array>

#include "io.h"
#include "registers.h"

class Arm : public Registers
{
public:
    Arm();

    void init();
    void run(int cycles);
    void raise(uint irq);

    uint state   = 0;
    uint pipe[2] = {};

    IrqMaster   irqmaster;
    IrqEnable   irqenable;
    IrqRequest  irqrequest;
    WaitControl waitcnt;
    HaltControl haltcnt;

private:
    using Instruction32 = void(Arm::*)(u32);
    using Instruction16 = void(Arm::*)(u16);

    enum Shift
    {
        kShiftLsl,
        kShiftLsr,
        kShiftAsr,
        kShiftRor
    };

    template<bool Immediate> u32 lsl(u32 value, u32 amount, bool flags = true);
    template<bool Immediate> u32 lsr(u32 value, u32 amount, bool flags = true);
    template<bool Immediate> u32 asr(u32 value, u32 amount, bool flags = true);
    template<bool Immediate> u32 ror(u32 value, u32 amount, bool flags = true);

    template<typename Integral>
    Integral log(Integral value, bool flags = true);

    u32 add(u32 op1, u32 op2, bool flags = true);
    u32 sub(u32 op1, u32 op2, bool flags = true);
    u32 adc(u32 op1, u32 op2, bool flags = true);
    u32 sbc(u32 op1, u32 op2, bool flags = true);

    bool isSequential(u32 addr) const;

    u8  readByte(u32 addr);
    u16 readHalf(u32 addr);
    u32 readWord(u32 addr);

    void writeByte(u32 addr, u8  byte);
    void writeHalf(u32 addr, u16 half);
    void writeWord(u32 addr, u32 word);

    u32 readWordRotate(u32 addr);
    u32 readHalfRotate(u32 addr);
    u32 readHalfSignEx(u32 addr);

    void flushHalf();
    void flushWord();

    template<uint State>
    void dispatch();

    void idle();
    template<bool Signed>
    void booth(u32 multiplier);

    void interrupt(u32 pc, u32 lr, Psr::Mode mode);
    void interruptHw();
    void interruptSw();
    void processIrq();

    template<u32 Instr> void Arm_BranchExchange(u32 instr);
    template<u32 Instr> void Arm_BranchLink(u32 instr);
    template<u32 Instr> void Arm_DataProcessing(u32 instr);
    template<u32 Instr> void Arm_StatusTransfer(u32 instr);
    template<u32 Instr> void Arm_Multiply(u32 instr);
    template<u32 Instr> void Arm_MultiplyLong(u32 instr);
    template<u32 Instr> void Arm_SingleDataTransfer(u32 instr);
    template<u32 Instr> void Arm_HalfSignedDataTransfer(u32 instr);
    template<u32 Instr> void Arm_BlockDataTransfer(u32 instr);
    template<u32 Instr> void Arm_SingleDataSwap(u32 instr);
    template<u32 Instr> void Arm_SoftwareInterrupt(u32 instr);
    template<u32 Instr> void Arm_CoprocessorDataOperations(u32 instr);
    template<u32 Instr> void Arm_CoprocessorDataTransfers(u32 instr);
    template<u32 Instr> void Arm_CoprocessorRegisterTransfers(u32 instr);
    template<u32 Instr> void Arm_Undefined(u32 instr);

    template<u16 Instr> void Thumb_MoveShiftedRegister(u16 instr);
    template<u16 Instr> void Thumb_AddSubtract(u16 instr);
    template<u16 Instr> void Thumb_ImmediateOperations(u16 instr);
    template<u16 Instr> void Thumb_AluOperations(u16 instr);
    template<u16 Instr> void Thumb_HighRegisterOperations(u16 instr);
    template<u16 Instr> void Thumb_LoadPcRelative(u16 instr);
    template<u16 Instr> void Thumb_LoadStoreRegisterOffset(u16 instr);
    template<u16 Instr> void Thumb_LoadStoreByteHalf(u16 instr);
    template<u16 Instr> void Thumb_LoadStoreImmediateOffset(u16 instr);
    template<u16 Instr> void Thumb_LoadStoreHalf(u16 instr);
    template<u16 Instr> void Thumb_LoadStoreSpRelative(u16 instr);
    template<u16 Instr> void Thumb_LoadRelativeAddress(u16 instr);
    template<u16 Instr> void Thumb_AddOffsetSp(u16 instr);
    template<u16 Instr> void Thumb_PushPopRegisters(u16 instr);
    template<u16 Instr> void Thumb_LoadStoreMultiple(u16 instr);
    template<u16 Instr> void Thumb_ConditionalBranch(u16 instr);
    template<u16 Instr> void Thumb_SoftwareInterrupt(u16 instr);
    template<u16 Instr> void Thumb_UnconditionalBranch(u16 instr);
    template<u16 Instr> void Thumb_LongBranchLink(u16 instr);
    template<u16 Instr> void Thumb_Undefined(u16 instr);

    template<uint Hash> static constexpr Instruction32 Arm_Decode();
    template<uint Hash> static constexpr Instruction16 Thumb_Decode();

    static const std::array<Instruction32, 4096> instr_arm;
    static const std::array<Instruction16, 1024> instr_thumb;

    int cycles    = 0;
    u32 prev_addr = 0;
};

inline Arm arm;

#include "arm.inl"
