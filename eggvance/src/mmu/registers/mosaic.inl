template<int index>
inline u8 Mosaic::readByte()
{
    static_assert(index <= 1, "Invalid index");
    return 0;
}

template<int index>
inline void Mosaic::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0:
        bgs.x = bits<0, 4>(byte) + 1;
        bgs.y = bits<4, 4>(byte) + 1;
        break;

    case 1:
        obj.x = bits<0, 4>(byte) + 1;
        obj.y = bits<4, 4>(byte) + 1;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
