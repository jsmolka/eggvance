inline IntMaster::operator bool() const
{
    return enabled;
}

template<int index>
inline u8 IntMaster::readByte()
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: return enabled;
    case 1: return 0;

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

template<int index>
inline void IntMaster::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    if (index == 0)
        enabled = bits<0, 1>(byte);
}
