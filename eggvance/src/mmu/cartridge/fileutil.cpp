#include "fileutil.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

bool fileutil::read(const std::string& file, std::vector<u8>& dst)
{
    std::ifstream stream(file, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    dst.resize(size);
    stream.read(reinterpret_cast<char*>(dst.data()), size);

    return true;
}

bool fileutil::write(const std::string& file, std::vector<u8>& src)
{
    std::ofstream stream(file, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.write(reinterpret_cast<char*>(src.data()), src.size());

    return true;
}

bool fileutil::exists(const std::string& file)
{
    return fs::is_regular_file(file);
}
