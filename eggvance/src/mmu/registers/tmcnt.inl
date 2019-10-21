template<int index>
inline u8 TmCnt::readByte()
{
    static_assert(index <= 3, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
    case 1:
        byte = byteArray(data)[index];
        break;

    case 2:
        byte |= prescaler << 0;
        byte |= cascade   << 2;
        byte |= irq       << 6;
        byte |= enabled   << 7;
        break;
    }
    return byte;
}

template<int index>
inline void TmCnt::writeByte(u8 byte)
{
    static_assert(index <= 3, "Invalid index");

    switch (index)
    {
    case 0:
    case 1:
        byteArray(initial)[index] = byte;
        break;

    case 2:
        prescaler = bits<0, 2>(byte);
        cascade   = bits<2, 1>(byte);
        irq       = bits<6, 1>(byte);
        enabled   = bits<7, 1>(byte);
        break;
    }
}
