#pragma once

#include <shell/operators.h>

#include "base/bit.h"
#include "base/int.h"

enum class InstructionArm
{
    Undefined,
    BranchExchange,
    BranchLink,
    DataProcessing,
    StatusTransfer,
    Multiply,
    MultiplyLong,
    SingleDataTransfer,
    HalfSignedDataTransfer,
    BlockDataTransfer,
    SingleDataSwap,
    SoftwareInterrupt,
    CoprocessorDataOperations,
    CoprocessorDataTransfers,
    CoprocessorRegisterTransfers
};

constexpr uint hashArm(u32 instr)
{
    return ((instr >> 16) & 0xFF0) | ((instr >> 4) & 0xF);
}

constexpr u32 dehashArm(uint hash)
{
    return ((hash & 0xFF0) << 16) | ((hash & 0xF) << 4);
}

constexpr InstructionArm decodeArm(uint hash)
{
    if ((hash & 0b1111'0000'0000) == 0b1111'0000'0000) return InstructionArm::SoftwareInterrupt;
    if ((hash & 0b1110'0000'0000) == 0b1100'0000'0000) return InstructionArm::CoprocessorDataTransfers;
    if ((hash & 0b1111'0000'0001) == 0b1110'0000'0000) return InstructionArm::CoprocessorDataOperations;
    if ((hash & 0b1111'0000'0001) == 0b1110'0000'0001) return InstructionArm::CoprocessorRegisterTransfers;
    if ((hash & 0b1110'0000'0000) == 0b1010'0000'0000) return InstructionArm::BranchLink;
    if ((hash & 0b1110'0000'0000) == 0b1000'0000'0000) return InstructionArm::BlockDataTransfer;
    if ((hash & 0b1110'0000'0001) == 0b0110'0000'0001) return InstructionArm::Undefined;
    if ((hash & 0b1100'0000'0000) == 0b0100'0000'0000) return InstructionArm::SingleDataTransfer;
    if ((hash & 0b1111'1111'1111) == 0b0001'0010'0001) return InstructionArm::BranchExchange;
    if ((hash & 0b1111'1100'1111) == 0b0000'0000'1001) return InstructionArm::Multiply;
    if ((hash & 0b1111'1000'1111) == 0b0000'1000'1001) return InstructionArm::MultiplyLong;
    if ((hash & 0b1111'1011'1111) == 0b0001'0000'1001) return InstructionArm::SingleDataSwap;
    if ((hash & 0b1110'0000'1001) == 0b0000'0000'1001)
    {
        enum class Opcode { Swap, Ldrh, Ldrsb, Ldrsh };

        uint opcode = bit::seq<1, 2>(hash);

        if (opcode == Opcode::Swap)
            return InstructionArm::Undefined;

        return InstructionArm::HalfSignedDataTransfer;
    }
    if ((hash & 0b1101'1001'0000) == 0b0001'0000'0000) return InstructionArm::StatusTransfer;
    if ((hash & 0b1100'0000'0000) == 0b0000'0000'0000)
    {
        enum class Opcode
        {
            And, Eor, Sub, Rsb,
            Add, Adc, Sbc, Rsc,
            Tst, Teq, Cmp, Cmn,
            Orr, Mov, Bic, Mvn
        }; 

        uint flags  = bit::seq<4, 1>(hash);
        uint opcode = bit::seq<5, 4>(hash);

        if (!flags && (opcode == Opcode::Tst || opcode == Opcode::Teq || opcode == Opcode::Cmp || opcode == Opcode::Cmn))
            return InstructionArm::Undefined;

        return InstructionArm::DataProcessing;
    }
    return InstructionArm::Undefined;
}

enum class InstructionThumb
{
    Undefined,
    MoveShiftedRegister,
    AddSubtract,
    ImmediateOperations,
    AluOperations,
    HighRegisterOperations,
    LoadPcRelative,
    LoadStoreRegisterOffset,
    LoadStoreByteHalf,
    LoadStoreImmediateOffset,
    LoadStoreHalf,
    LoadStoreSpRelative,
    LoadRelativeAddress,
    AddOffsetSp,
    PushPopRegisters,
    LoadStoreMultiple,
    ConditionalBranch,
    SoftwareInterrupt,
    UnconditionalBranch,
    LongBranchLink
};

constexpr uint hashThumb(u16 instr)
{
    return instr >> 6;
}

constexpr u16 dehashThumb(uint hash)
{
    return hash << 6;
}

constexpr InstructionThumb decodeThumb(uint hash)
{
    if ((hash & 0b11'1110'0000) == 0b00'0110'0000) return InstructionThumb::AddSubtract;
    if ((hash & 0b11'1000'0000) == 0b00'0000'0000) return InstructionThumb::MoveShiftedRegister;
    if ((hash & 0b11'1000'0000) == 0b00'1000'0000) return InstructionThumb::ImmediateOperations;
    if ((hash & 0b11'1111'0000) == 0b01'0000'0000) return InstructionThumb::AluOperations;
    if ((hash & 0b11'1111'0000) == 0b01'0001'0000)
    {
        enum class Opcode { Add, Cmp, Mov, Bx };

        uint hs     = bit::seq<0, 1>(hash);
        uint hd     = bit::seq<1, 1>(hash);
        uint opcode = bit::seq<2, 2>(hash);

        if (opcode != Opcode::Bx && hs == 0 && hd == 0)
            return InstructionThumb::Undefined;
        if (opcode == Opcode::Bx && hd == 1)
            return InstructionThumb::Undefined;

        return InstructionThumb::HighRegisterOperations;
    }
    if ((hash & 0b11'1110'0000) == 0b01'0010'0000) return InstructionThumb::LoadPcRelative;
    if ((hash & 0b11'1100'1000) == 0b01'0100'0000) return InstructionThumb::LoadStoreRegisterOffset;
    if ((hash & 0b11'1100'1000) == 0b01'0100'1000) return InstructionThumb::LoadStoreByteHalf;
    if ((hash & 0b11'1000'0000) == 0b01'1000'0000) return InstructionThumb::LoadStoreImmediateOffset;
    if ((hash & 0b11'1100'0000) == 0b10'0000'0000) return InstructionThumb::LoadStoreHalf;
    if ((hash & 0b11'1100'0000) == 0b10'0100'0000) return InstructionThumb::LoadStoreSpRelative;
    if ((hash & 0b11'1100'0000) == 0b10'1000'0000) return InstructionThumb::LoadRelativeAddress;
    if ((hash & 0b11'1111'1100) == 0b10'1100'0000) return InstructionThumb::AddOffsetSp;
    if ((hash & 0b11'1101'1000) == 0b10'1101'0000) return InstructionThumb::PushPopRegisters;
    if ((hash & 0b11'1100'0000) == 0b11'0000'0000) return InstructionThumb::LoadStoreMultiple;
    if ((hash & 0b11'1111'1100) == 0b11'0111'1100) return InstructionThumb::SoftwareInterrupt;
    if ((hash & 0b11'1100'0000) == 0b11'0100'0000)
    {
        enum class Condition
        {
            EQ, NE, CS, CC,
            MI, PL, VS, VC,
            HI, LS, GE, LT,
            GT, LE, AL, NV
        };

        uint condition = bit::seq<2, 4>(hash);

        if (condition == Condition::AL)
            return InstructionThumb::Undefined;
        if (condition == Condition::NV)
            return InstructionThumb::Undefined;

        return InstructionThumb::ConditionalBranch;
    }
    if ((hash & 0b11'1110'0000) == 0b11'1000'0000) return InstructionThumb::UnconditionalBranch;
    if ((hash & 0b11'1100'0000) == 0b11'1100'0000) return InstructionThumb::LongBranchLink;

    return InstructionThumb::Undefined;
}
