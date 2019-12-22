#pragma once

#include "common/integer.h"

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

enum class InstructionThumb
{
    Undefined,
    MoveShiftedRegister,
    AddSubtract,
    ImmediateOperations,
    ALUOperations,
    HighRegisterOperations,
    LoadPCRelative,
    LoadStoreRegisterOffset,
    LoadStoreByteHalf,
    LoadStoreImmediateOffset,
    LoadStoreHalf,
    LoadStoreSPRelative,
    LoadRelativeAddress,
    AddOffsetSP,
    PushPopRegisters,
    LoadStoreMultiple,
    ConditionalBranch,
    SoftwareInterrupt,
    UnconditionalBranch,
    LongBranchLink
};

constexpr int armHash(u32 instr)
{
    return ((instr >> 16) & 0xFF0) | ((instr >> 4) & 0xF);
}

constexpr int thumbHash(u16 instr)
{
    return instr >> 8;
}

InstructionArm decodeArm(u32 instr);
InstructionArm decodeArmHash(int hash);
InstructionThumb decodeThumb(u16 instr);
InstructionThumb decodeThumbHash(int hash);
