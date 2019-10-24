template<int index>
inline u8 DMACnt::readByte()
{
    static_assert(index <= 3, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
    case 1:
        break;

    case 2:
        byte |= dad_delta << 5;
        byte |= sad_delta << 7;
        break;

    case 3:
        byte |= sad_delta >> 1;
        byte |= repeat   << 1;
        byte |= word     << 2;
        byte |= drq      << 3;
        byte |= timing   << 4;
        byte |= irq      << 6;
        byte |= enabled   << 7;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

template<int index>
inline void DMACnt::writeByte(u8 byte)
{
    static_assert(index <= 3, "Invalid index");

    switch (index)
    {
    case 0:
    case 1:
        byteArray(count)[index] = byte;
        count &= count_mask;
        if (count == 0)
            count = count_mask + 1;
        break;

    case 2:
        dad_delta = bits<5, 2>(byte);
        sad_delta = bits<7, 1>(byte) << 0 | (sad_delta & ~0x1);
        break;

    case 3:
        sad_delta = bits<0, 1>(byte) << 1 | (sad_delta & ~0x2);
        repeat   = bits<1, 1>(byte);
        word     = bits<2, 1>(byte);
        drq      = bits<3, 1>(byte);
        timing   = bits<4, 2>(byte);
        irq      = bits<6, 1>(byte);
        enabled   = bits<7, 1>(byte);
        update   = enabled;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
