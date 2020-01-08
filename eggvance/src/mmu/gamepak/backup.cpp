#include "backup.h"

#include "common/fs.h"

Backup::Backup()
    : file()
    , type(Type::None)
{

}

Backup::Backup(const Path& file, Type type)
    : file(file)
    , type(type)
{
    if (fs::isFile(file))
        fs::read(file, data);
}

Backup::~Backup()
{
    if (!data.empty())
        fs::write(file, data);
}

u8 Backup::readByte(u32 addr)
{
    return 0;
}

void Backup::writeByte(u32 addr, u8 byte)
{

}
