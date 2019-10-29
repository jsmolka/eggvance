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

extern InstructionArm decodeArm(u32 instr);
extern InstructionThumb decodeThumb(u16 instr);
