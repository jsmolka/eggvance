#pragma once

#include "common/integer.h"

enum class InstructionArm : char
{
    Invalid,
    BranchExchange,
    BranchLink,
    DataProcessing,
    PSRTransfer,
    Multiply,
    MultiplyLong,
    SingleDataTransfer,
    HalfwordSignedDataTransfer,
    BlockDataTransfer,
    SingleDataSwap,
    SoftwareInterrupt,
    CoprocessorDataOperations,
    CoprocessorDataTransfers,
    CoprocessorRegisterTransfers,
    Undefined
};

enum class InstructionThumb : char
{
    Invalid,
    MoveShiftedRegister,
    AddSubtractImmediate,
    AddSubtractMoveCompareImmediate,
    ALUOperations,
    HighRegisterBranchExchange,
    LoadPCRelative,
    LoadStoreRegisterOffset,
    LoadStoreHalfwordSigned,
    LoadStoreImmediateOffset,
    LoadStoreHalfword,
    LoadStoreSPRelative,
    LoadAddress,
    AddOffsetSP,
    PushPopRegisters,
    LoadStoreMultiple,
    ConditionalBranch,
    SoftwareInterrupt,
    UnconditionalBranch,
    LongBranchLink
};

InstructionArm decodeArm(u32 instr);
InstructionThumb decodeThumb(u16 instr);
