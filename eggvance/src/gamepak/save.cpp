#include "save.h"

#include "file.h"

Save::Save(const std::string& file, Type type)
    : file(file)
    , type(type)
{
    if (File::exists(file))
        File::read(file, data);
}

Save::~Save()
{
    if (!data.empty())
        File::write(file, data);
}
