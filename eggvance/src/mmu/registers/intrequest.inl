inline IntRequest::operator int() const
{
    return value;
}

inline IntRequest& IntRequest::operator|=(int value)
{
    this->value |= value;
    return *this;
}

template<int index>
inline u8 IntRequest::readByte()
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
inline void IntRequest::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: byteArray(value)[0] &= ~byte; break;
    case 1: byteArray(value)[1] &= ~byte; break;

    default:
        EGG_UNREACHABLE;
        break;
    }
}
