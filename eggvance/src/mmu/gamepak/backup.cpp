#include "backup.h"

#include "common/fileutil.h"

Backup::Backup()
    : file("")
    , type(Type::None)
{

}

Backup::Backup(const std::string& file, Type type)
    : file(file)
    , type(type)
{
    if (fileutil::exists(file))
        fileutil::read(file, data);
}

Backup::~Backup()
{
    if (!data.empty())
        fileutil::write(file, data);
}

u8 Backup::readByte(u32 addr)
{
    return 0;
}

void Backup::writeByte(u32 addr, u8 byte)
{

}
