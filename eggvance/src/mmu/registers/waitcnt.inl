template<int index>
inline u8 WaitCnt::readByte()
{
    static_assert(index <= 2, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= sram  << 0;
        byte |= ws0.n << 2;
        byte |= ws0.s << 4;
        byte |= ws1.n << 5;
        byte |= ws1.s << 7;
        break;

    case 1:
        byte |= ws2.n    << 0;
        byte |= ws2.s    << 2;
        byte |= phi      << 3;
        byte |= prefetch << 6;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

template<int index>
inline void WaitCnt::writeByte(u8 byte)
{
    static_assert(index <= 2, "Invalid index");

    switch (index)
    {
    case 0:
        sram  = bits<0, 2>(byte);
        ws0.n = bits<2, 2>(byte);
        ws0.s = bits<4, 1>(byte);
        ws1.n = bits<5, 2>(byte);
        ws1.s = bits<7, 1>(byte);
        break;

    case 1:
        ws2.n    = bits<0, 2>(byte);
        ws2.s    = bits<2, 1>(byte);
        phi      = bits<3, 2>(byte);
        prefetch = bits<6, 1>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    update();
}
