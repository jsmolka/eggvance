#include "decode.h"

#include "common/bits.h"

InstructionArm decodeArm(u32 instr)
{
    return decodeArmHash(armHash(instr));
}

InstructionArm decodeArmHash(int hash)
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
    if ((hash & 0b1110'0000'1001) == 0b0000'0000'1001) return InstructionArm::HalfSignedDataTransfer;
    if ((hash & 0b1101'1001'0000) == 0b0001'0000'0000) return InstructionArm::StatusTransfer;
    if ((hash & 0b1100'0000'0000) == 0b0000'0000'0000)
    {
        int flags  = bits<4, 1>(hash);
        int opcode = bits<5, 4>(hash);

        if ((opcode >> 2) == 0b10 && !flags)
            return InstructionArm::Undefined;

        return InstructionArm::DataProcessing;
    }
    return InstructionArm::Undefined;
}

InstructionThumb decodeThumb(u16 instr)
{
    return decodeThumbHash(thumbHash(instr));
}

InstructionThumb decodeThumbHash(int hash)
{
    if ((hash & 0b1111'1000) == 0b0001'1000) return InstructionThumb::AddSubtract;
    if ((hash & 0b1110'0000) == 0b0000'0000) return InstructionThumb::MoveShiftedRegister;
    if ((hash & 0b1110'0000) == 0b0010'0000) return InstructionThumb::ImmediateOperations;
    if ((hash & 0b1111'1100) == 0b0100'0000) return InstructionThumb::ALUOperations;
    if ((hash & 0b1111'1100) == 0b0100'0100) return InstructionThumb::HighRegisterOperations;
    if ((hash & 0b1111'1000) == 0b0100'1000) return InstructionThumb::LoadPCRelative;
    if ((hash & 0b1111'0010) == 0b0101'0000) return InstructionThumb::LoadStoreRegisterOffset;
    if ((hash & 0b1111'0010) == 0b0101'0010) return InstructionThumb::LoadStoreByteHalf;
    if ((hash & 0b1110'0000) == 0b0110'0000) return InstructionThumb::LoadStoreImmediateOffset;
    if ((hash & 0b1111'0000) == 0b1000'0000) return InstructionThumb::LoadStoreHalf;
    if ((hash & 0b1111'0000) == 0b1001'0000) return InstructionThumb::LoadStoreSPRelative;
    if ((hash & 0b1111'0000) == 0b1010'0000) return InstructionThumb::LoadRelativeAddress;
    if ((hash & 0b1111'1111) == 0b1011'0000) return InstructionThumb::AddOffsetSP;
    if ((hash & 0b1111'0110) == 0b1011'0100) return InstructionThumb::PushPopRegisters;
    if ((hash & 0b1111'0000) == 0b1100'0000) return InstructionThumb::LoadStoreMultiple;
    if ((hash & 0b1111'1111) == 0b1101'1111) return InstructionThumb::SoftwareInterrupt;
    if ((hash & 0b1111'0000) == 0b1101'0000)
    {
        int condition = bits<2, 4>(hash);

        if (condition == 0b1110)
            return InstructionThumb::Undefined;
        if (condition == 0b1111)
            return InstructionThumb::Undefined;

        return InstructionThumb::ConditionalBranch;
    }
    if ((hash & 0b1111'1000) == 0b1110'0000) return InstructionThumb::UnconditionalBranch;
    if ((hash & 0b1111'0000) == 0b1111'0000) return InstructionThumb::LongBranchLink;

    return InstructionThumb::Undefined;
}
