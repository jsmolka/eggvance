template<int index>
inline u8 DispCnt::readByte()
{
    static_assert(index <= 1, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= mode        << 0;
        byte |= frame       << 4;
        byte |= oam_hblank  << 5;
        byte |= mapping_1d  << 6;
        byte |= force_blank << 7;
        break;

    case 1:
        byte |= bg[0]  << 0;
        byte |= bg[1]  << 1;
        byte |= bg[2]  << 2;
        byte |= bg[3]  << 3;
        byte |= obj    << 4;
        byte |= win0   << 5;
        byte |= win1   << 6;
        byte |= winobj << 7;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

template<int index>
inline void DispCnt::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0:
        mode        = bits<0, 3>(byte);
        frame       = bits<4, 1>(byte);
        oam_hblank  = bits<5, 1>(byte);
        mapping_1d  = bits<6, 1>(byte);
        force_blank = bits<7, 1>(byte);
        break;

    case 1:
        bg[0]  = bits<0, 1>(byte);
        bg[1]  = bits<1, 1>(byte);
        bg[2]  = bits<2, 1>(byte);
        bg[3]  = bits<3, 1>(byte);
        obj    = bits<4, 1>(byte);
        win0   = bits<5, 1>(byte);
        win1   = bits<6, 1>(byte);
        winobj = bits<7, 1>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    update();
}
