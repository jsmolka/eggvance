template<int index>
inline u8 BldCnt::readByte()
{
    static_assert(index <= 1, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= upper << 0;
        byte |= mode  << 6;
        break;

    case 1:
        byte |= lower << 0;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

template<int index>
inline void BldCnt::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0:
        upper = bits<0, 6>(byte);
        mode  = bits<6, 2>(byte);
        break;

    case 1:
        lower = bits<0, 6>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
