template<int index>
inline u8 BgOffset::readByte()
{
    static_assert(index <= 1, "Invalid index");
    return 0;
}

template<int index>
inline void BgOffset::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    if (index == 1)
        byte &= 0x1;

    byteArray(offset)[index] = byte;
}
