inline VCount::operator int() const
{
    return line;
}

inline VCount& VCount::operator=(int value)
{
    line = value;
    return *this;
}

template<int index>
inline u8 VCount::readByte()
{
    static_assert(index <= 1, "Invalid index");

    switch (index)
    {
    case 0: return line;
    case 1: return 0;

    default:
        EGG_UNREACHABLE;
        return 0;
    }
}

template<int index>
inline void VCount::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");
}
