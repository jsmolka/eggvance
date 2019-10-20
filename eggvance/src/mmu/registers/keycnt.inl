template<int index>
inline u8 KeyCnt::readByte()
{
    static_assert(index <= 1, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= byteArray(keys)[0] << 0;
        break;

    case 1:
        byte |= byteArray(keys)[1] << 0;
        byte |= irq                << 6;
        byte |= irq_logic          << 7;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

template<int index>
inline void KeyCnt::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0:
        byteArray(keys)[0] = byte;
        break;

    case 1:
        byteArray(keys)[1] = bits<0, 2>(byte);
        irq                = bits<6, 1>(byte);
        irq_logic          = bits<7, 1>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
