#pragma once

#include <string>
#include <vector>

#include "common/integer.h"

namespace fileutil
{
    bool read(const std::string& file, std::vector<u8>& dst);
    bool write(const std::string& file, std::vector<u8>& src);
    bool exists(const std::string& file);

    std::string concat(const std::string& left, const std::string& right);
    std::string toAbsolute(const std::string& path);
}
