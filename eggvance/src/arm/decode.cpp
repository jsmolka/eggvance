#include "decode.h"

#include <array>

InstructionArm decodeHashArm(int hash)
{
    if ((hash & 0b1110'0000'0000) == 0b1010'0000'0000) return InstructionArm::BranchLink;
    if ((hash & 0b1110'0000'0000) == 0b1000'0000'0000) return InstructionArm::BlockDataTransfer;
    if ((hash & 0b1110'0000'0000) == 0b1100'0000'0000) return InstructionArm::CoprocessorDataTransfers;
    if ((hash & 0b1111'0000'0001) == 0b1110'0000'0000) return InstructionArm::CoprocessorDataOperations;
    if ((hash & 0b1111'0000'0001) == 0b1110'0000'0001) return InstructionArm::CoprocessorRegisterTransfers;
    if ((hash & 0b1111'0000'0000) == 0b1111'0000'0000) return InstructionArm::SoftwareInterrupt;
    if ((hash & 0b1110'0000'0001) == 0b0110'0000'0001) return InstructionArm::Undefined;
    if ((hash & 0b1100'0000'0000) == 0b0100'0000'0000) return InstructionArm::SingleDataTransfer;
    if ((hash & 0b1111'1111'1111) == 0b0001'0010'0001) return InstructionArm::BranchExchange;
    if ((hash & 0b1111'1100'1111) == 0b0000'0000'1001) return InstructionArm::Multiply;
    if ((hash & 0b1111'1000'1111) == 0b0000'1000'1001) return InstructionArm::MultiplyLong;
    if ((hash & 0b1111'1011'1111) == 0b0001'0000'1001) return InstructionArm::SingleDataSwap;
    if ((hash & 0b1110'0000'1001) == 0b0000'0000'1001) return InstructionArm::HalfwordSignedDataTransfer;
    if ((hash & 0b1101'1001'0000) == 0b0001'0000'0000) return InstructionArm::PSRTransfer;
    if ((hash & 0b1100'0000'0000) == 0b0000'0000'0000) return InstructionArm::DataProcessing;

    return InstructionArm::Undefined;
}

using LutArm = std::array<InstructionArm, 4096>;

const LutArm makeLutArm()
{
    LutArm lut;
    for (int hash = 0; hash < lut.size(); ++hash)
        lut[hash] = decodeHashArm(hash);

    return lut;
}

inline int hashArm(u32 instr)
{
    return ((instr >> 16) & 0xFF0) | ((instr >> 4) & 0xF);
}

static const LutArm lut_arm = makeLutArm();

InstructionArm decodeArm(u32 instr)
{
    return lut_arm[hashArm(instr)];
}

InstructionThumb decodeHashThumb(int hash)
{
    if ((hash & 0b1111'1000) == 0b0001'1000) return InstructionThumb::AddSubtractImmediate;
    if ((hash & 0b1110'0000) == 0b0000'0000) return InstructionThumb::MoveShiftedRegister;
    if ((hash & 0b1110'0000) == 0b0010'0000) return InstructionThumb::AddSubtractMoveCompareImmediate;
    if ((hash & 0b1111'1100) == 0b0100'0000) return InstructionThumb::ALUOperations;
    if ((hash & 0b1111'1100) == 0b0100'0100) return InstructionThumb::HighRegisterBranchExchange;
    if ((hash & 0b1111'1000) == 0b0100'1000) return InstructionThumb::LoadPCRelative;
    if ((hash & 0b1111'0010) == 0b0101'0000) return InstructionThumb::LoadStoreRegisterOffset;
    if ((hash & 0b1111'0010) == 0b0101'0010) return InstructionThumb::LoadStoreHalfwordSigned;
    if ((hash & 0b1110'0000) == 0b0110'0000) return InstructionThumb::LoadStoreImmediateOffset;
    if ((hash & 0b1111'0000) == 0b1000'0000) return InstructionThumb::LoadStoreHalfword;
    if ((hash & 0b1111'0000) == 0b1001'0000) return InstructionThumb::LoadStoreSPRelative;
    if ((hash & 0b1111'0000) == 0b1010'0000) return InstructionThumb::LoadAddress;
    if ((hash & 0b1111'1111) == 0b1011'0000) return InstructionThumb::AddOffsetSP;
    if ((hash & 0b1111'0110) == 0b1011'0100) return InstructionThumb::PushPopRegisters;
    if ((hash & 0b1111'0000) == 0b1100'0000) return InstructionThumb::LoadStoreMultiple;
    if ((hash & 0b1111'1111) == 0b1101'1111) return InstructionThumb::SoftwareInterrupt;
    if ((hash & 0b1111'0000) == 0b1101'0000) return InstructionThumb::ConditionalBranch;
    if ((hash & 0b1111'1000) == 0b1110'0000) return InstructionThumb::UnconditionalBranch;
    if ((hash & 0b1111'0000) == 0b1111'0000) return InstructionThumb::LongBranchLink;

    return InstructionThumb::Undefined;
}

using LutThumb = std::array<InstructionThumb, 256>;

const LutThumb makeLutThumb()
{
    LutThumb lut;
    for (int hash = 0; hash < lut.size(); ++hash)
        lut[hash] = decodeHashThumb(hash);

    return lut;
}

inline int hashThumb(u16 instr)
{
    return instr >> 8;
}

static const LutThumb lut_thumb = makeLutThumb();

InstructionThumb decodeThumb(u16 instr)
{
    return lut_thumb[hashThumb(instr)];
}
