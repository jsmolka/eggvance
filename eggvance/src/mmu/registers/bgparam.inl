template<int index>
inline u8 BgParam::readByte()
{
    static_assert(index <= 1, "Invalid index");
    return 0;
}

template<int index>
inline void BgParam::writeByte(u8 byte)
{
    static_assert(index <= 1, "Invalid index");

    byteArray(param)[index] = byte;

}
