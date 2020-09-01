#pragma once

#include <array>

#include "arm/io.h"
#include "arm/registers.h"

class Arm : public Registers
{
public:
    friend class DmaChannel;
    friend class MMU;

    enum State
    {
        kStateThumb = 1 << 0,
        kStateHalt  = 1 << 1,
        kStateIrq   = 1 << 2,
        kStateDma   = 1 << 3,
        kStateTimer = 1 << 4
    };

    Arm();

    void run(int cycles);

    uint state = 0;

    ArmIo io;

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

    u32 lsl(u32 value, u32 amount, bool flags = true);
    template<bool immediate>
    u32 lsr(u32 value, u32 amount, bool flags = true);
    template<bool immediate>
    u32 asr(u32 value, u32 amount, bool flags = true);
    template<bool immediate>
    u32 ror(u32 value, u32 amount, bool flags = true);

    template<typename T>
    T log(T value, bool flags = true);

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

    u32 readWordRotated(u32 addr);
    u32 readHalfRotated(u32 addr);
    u32 readHalfSigned(u32 addr);

    void flushHalf();
    void flushWord();

    template<uint state>
    void dispatch();

    void idle();
    void booth(u32 multiplier, bool ones);

    void interrupt(u32 pc, u32 lr, PSR::Mode mode);
    void interruptHW();
    void interruptSW();

    template<uint Instr> void Arm_BranchExchange(u32 instr);
    template<uint Instr> void Arm_BranchLink(u32 instr);
    template<uint Instr> void Arm_DataProcessing(u32 instr);
    template<uint Instr> void Arm_StatusTransfer(u32 instr);
    template<uint Instr> void Arm_Multiply(u32 instr);
    template<uint Instr> void Arm_MultiplyLong(u32 instr);
    template<uint Instr> void Arm_SingleDataTransfer(u32 instr);
    template<uint Instr> void Arm_HalfSignedDataTransfer(u32 instr);
    template<uint Instr> void Arm_BlockDataTransfer(u32 instr);
    template<uint Instr> void Arm_SingleDataSwap(u32 instr);
    template<uint Instr> void Arm_SoftwareInterrupt(u32 instr);
    template<uint Instr> void Arm_CoprocessorDataOperations(u32 instr);
    template<uint Instr> void Arm_CoprocessorDataTransfers(u32 instr);
    template<uint Instr> void Arm_CoprocessorRegisterTransfers(u32 instr);
    template<uint Instr> void Arm_Undefined(u32 instr);

    template<uint Instr> void Thumb_MoveShiftedRegister(u16 instr);
    template<uint Instr> void Thumb_AddSubtract(u16 instr);
    template<uint Instr> void Thumb_ImmediateOperations(u16 instr);
    template<uint Instr> void Thumb_AluOperations(u16 instr);
    template<uint Instr> void Thumb_HighRegisterOperations(u16 instr);
    template<uint Instr> void Thumb_LoadPcRelative(u16 instr);
    template<uint Instr> void Thumb_LoadStoreRegisterOffset(u16 instr);
    template<uint Instr> void Thumb_LoadStoreByteHalf(u16 instr);
    template<uint Instr> void Thumb_LoadStoreImmediateOffset(u16 instr);
    template<uint Instr> void Thumb_LoadStoreHalf(u16 instr);
    template<uint Instr> void Thumb_LoadStoreSpRelative(u16 instr);
    template<uint Instr> void Thumb_LoadRelativeAddress(u16 instr);
    template<uint Instr> void Thumb_AddOffsetSp(u16 instr);
    template<uint Instr> void Thumb_PushPopRegisters(u16 instr);
    template<uint Instr> void Thumb_LoadStoreMultiple(u16 instr);
    template<uint Instr> void Thumb_ConditionalBranch(u16 instr);
    template<uint Instr> void Thumb_SoftwareInterrupt(u16 instr);
    template<uint Instr> void Thumb_UnconditionalBranch(u16 instr);
    template<uint Instr> void Thumb_LongBranchLink(u16 instr);
    template<uint Instr> void Thumb_Undefined(u16 instr);

    template<uint Hash> static constexpr Instruction32 Arm_Decode();
    template<uint Hash> static constexpr Instruction16 Thumb_Decode();

    static const std::array<Instruction32, 4096> instr_arm;
    static const std::array<Instruction16, 1024> instr_thumb;

    int cycles    = 0;
    u32 prev_addr = 0;
    u32 pipe[2]   = { 0 };
};

inline Arm arm;
