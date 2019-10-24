template<int index>
inline u8 WinIn::readByte()
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: return win0.readByte();
    case 1: return win1.readByte();

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

template<int index>
inline void WinIn::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: win0.writeByte(byte); break;
    case 1: win1.writeByte(byte); break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}

template<int index>
inline u8 WinOut::readByte()
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: return winout.readByte();
    case 1: return winobj.readByte();

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

template<int index>
inline void WinOut::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: winout.writeByte(byte); break;
    case 1: winobj.writeByte(byte); break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
