#include "decoder.h"

Format Decoder::decode(u32 instr, bool arm)
{
    if (arm)
        return decodeArm(instr);
    else
        return decodeThumb(static_cast<u16>(instr));
}

Format Decoder::decodeArm(u32 instr)
{
    if (((instr >> 25) & 0x7) == 0b101)
    {
        return ARM_2;
    }
    else if (((instr >> 25) & 0x7) == 0b100)
    {
        return ARM_9;
    }
    else if (((instr >> 26) & 0x3) == 0b11)
    {
        // Could also be coprocessor instruction, but the GBA has none

        return ARM_11;
    }
    else if (((instr >> 26) & 0x3) == 0b01)
    {
        // Could also be the undefined instruction, but seems linked to the coprocessor

        return ARM_7;
    }
    else
    {
        if (((instr >> 4) & 0xFFFFFF) == 0b000100101111111111110001)
        {
            return ARM_1;
        }
        else if (((instr >> 22) & 0xF) == 0b0000 && ((instr >> 4) & 0xF) == 0b1001)
        {
            return ARM_5;
        }
        else if (((instr >> 23) & 0x7) == 0b001 && ((instr >> 4) & 0xF) == 0b1001)
        {
            return ARM_6;
        }
        else if (((instr >> 23) & 0x7) == 0b010 && ((instr >> 20) & 0x3) == 0b00 && ((instr >> 4) & 0xFF) == 0b00001001)
        {
            return ARM_10;
        }
        else if (((instr >> 25) & 0x1) == 0b0 && ((instr >> 7) & 0x1) == 0b1 && ((instr >> 4) & 0x1) == 0b1)
        {
            return ARM_8;
        }
        else
        {
            int opcode = (instr >> 21) & 0xF;
            switch (opcode)
            {
            case 0b1000:  // TST
            case 0b1001:  // TEQ
            case 0b1010:  // CMP
            case 0b1011:  // CMN
            {
                bool flags = (instr >> 20) & 0x1;
                if (flags)
                    return ARM_3;
                else
                    return ARM_4;
                break;
            }

            default:
                return ARM_3;
            }
        }
    }
    return FMT_NONE;
}

Format Decoder::decodeThumb(u16 instr)
{
    if (((instr >> 11) & 0x1F) == 0b00011)
    {
        return THUMB_2;
    }
    else if (((instr >> 13) & 0x7) == 0b000)
    {
        return THUMB_1;
    }
    else if (((instr >> 13) & 0x7) == 0b001)
    {
        return THUMB_3;
    }
    else if (((instr >> 10) & 0x3F) == 0b010000)
    {
        return THUMB_4;
    }
    else if (((instr >> 10) & 0x3F) == 0b010001)
    {
        return THUMB_5;
    }
    else if (((instr >> 11) & 0x1F) == 0b01001)
    {
        return THUMB_6;
    }
    else if (((instr >> 12) & 0xF) == 0b0101)
    {
        return (((instr >> 9) & 0x1) == 0b0) ? THUMB_7 : THUMB_8;
    }
    else if (((instr >> 13) & 0x7) == 0b011)
    {
        return THUMB_9;
    }
    else if (((instr >> 12) & 0xF) == 0b1000)
    {
        return THUMB_10;
    }
    else if (((instr >> 12) & 0xF) == 0b1001)
    {
        return THUMB_11;
    }
    else if (((instr >> 12) & 0xF) == 0b1010)
    {
        return THUMB_12;
    }
    else if (((instr >> 12) & 0xF) == 0b1011)
    {
        return (((instr >> 10) & 0x1) == 0b0) ? THUMB_13 : THUMB_14;
    }
    else if (((instr >> 12) & 0xF) == 0b1100)
    {
        return THUMB_15;
    }
    else if (((instr >> 12) & 0xF) == 0b1101)
    {
        return (((instr >> 8) & 0xF) == 0b1111) ? THUMB_17 : THUMB_16;
    }
    else if (((instr >> 12) & 0xF) == 0b1110)
    {
        return THUMB_18;
    }
    else if (((instr >> 12) & 0xF) == 0b1111)
    {
        return THUMB_19;
    }
    return FMT_NONE;
}
