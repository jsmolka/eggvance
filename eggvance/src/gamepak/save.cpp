#include "save.h"

#include "fileutil.h"

Save::Save(const std::string& file, Type type)
    : file(file)
    , type(type)
{
    if (file_util::exists(file))
        file_util::read(file, data);
}

Save::~Save()
{
    if (!data.empty())
        file_util::write(file, data);
}
