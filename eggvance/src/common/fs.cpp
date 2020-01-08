#include "fs.h"

#include <fstream>
#include <stdexcept>

#ifdef _MSC_VER
#include <windows.h>
#endif

static Path executable_dir;

Path::Path(const char* path)
    : Path(std::string(path))
{

}

Path::Path(const std::string& path)
    : std::filesystem::path(path)
{
    #ifdef _MSC_VER
    std::wstring wpath;

    int wlength = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        path.data(),
        static_cast<int>(path.size()),
        nullptr,
        0
    );

    if (wlength == 0)
        throw std::runtime_error("Cannot convert path");

    wpath.resize(wlength);

    int success = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        path.data(),
        static_cast<int>(path.size()),
        wpath.data(),
        static_cast<int>(wpath.size())
    );

    if (success == 0)
        throw std::runtime_error("Cannot convert path");

    *this = wpath;
    #endif
}

Path::Path(const std::wstring& path)
    : std::filesystem::path(path) {}

Path::Path(const std::filesystem::path& path)
    : std::filesystem::path(path) {}

Path& Path::operator=(const char* path)
{
    return *this = Path(path);
}

Path& Path::operator=(const std::string& path)
{
    return *this = Path(path);
}

Path& Path::operator=(const std::wstring& path)
{
    return *this = Path(path);
}

Path& Path::operator=(const std::filesystem::path& path)
{
    return *this = Path(path);
}

void fs::init(const Path& executable)
{
    executable_dir = executable.parent_path();
}

bool fs::read(const Path& file, std::vector<u8>& dst)
{
    auto stream = std::ifstream(file, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.seekg(0, std::ios::end);
    std::streampos size = stream.tellg();
    stream.seekg(0, std::ios::beg);

    dst.resize(size);

    stream.read(reinterpret_cast<char*>(dst.data()), size);

    return true;
}

bool fs::write(const Path& file, std::vector<u8>& src)
{
    auto stream = std::ofstream(file, std::ios::binary);
    if (!stream.is_open())
        return false;

    stream.write(reinterpret_cast<char*>(src.data()), src.size());

    return true;
}

bool fs::isDir(const Path& path)
{
    return std::filesystem::exists(path)
        && std::filesystem::is_directory(path);
}

bool fs::isFile(const Path& file)
{
    return std::filesystem::exists(file)
        && std::filesystem::is_regular_file(file);
}

Path fs::relativeToCwd(const Path& path)
{
    return Path(std::filesystem::current_path()) / path;
}

Path fs::relativeToExe(const Path& path)
{
    return executable_dir / path;
}
