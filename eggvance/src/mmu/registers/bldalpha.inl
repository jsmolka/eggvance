template<int index>
inline u8 BldAlpha::readByte()
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: return reg.eva;
    case 1: return reg.evb;

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

template<int index>
inline void BldAlpha::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: reg.eva = bits<0, 5>(byte); break;
    case 1: reg.evb = bits<0, 5>(byte); break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    update();
}
