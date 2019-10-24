template<int index>
inline u8 BgCnt::readByte()
{
    static_assert(index <= 1, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= priority   << 0;
        byte |= tile_block << 2;
        byte |= mosaic     << 6;
        byte |= pformat    << 7;
        break;

    case 1:
        byte |= map_block   << 0;
        byte |= wraparound  << 5;
        byte |= screen_size << 6;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

template<int index>
inline void BgCnt::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0:
        priority   = bits<0, 2>(byte);
        tile_block = bits<2, 2>(byte);
        mosaic     = bits<6, 1>(byte);
        pformat    = bits<7, 1>(byte);
        break;

    case 1:
        map_block   = bits<0, 5>(byte);
        wraparound  = bits<5, 1>(byte);
        screen_size = bits<6, 2>(byte);
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
