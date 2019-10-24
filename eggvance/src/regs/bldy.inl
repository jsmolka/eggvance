template<int index>
inline u8 BldY::readByte()
{
    static_assert(index <= 1, "Invalid index");
    return 0;
}

template<int index>
inline void BldY::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    if (index == 0)
    {
        reg.evy = bits<0, 5>(byte);
        update();
    }
}
