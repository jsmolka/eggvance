inline IntEnabled::operator int() const
{
    return value;
}

template<int index>
inline u8 IntEnabled::readByte()
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: return bits<0, 8>(value);
    case 1: return bits<8, 8>(value);

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

template<int index>
inline void IntEnabled::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: byteArray(value)[0] = byte; break;
    case 1: byteArray(value)[1] = byte; break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
