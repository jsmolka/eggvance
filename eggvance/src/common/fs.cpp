#include "fs.h"

#include <fstream>

#ifdef _MSC_VER
#include <windows.h>
#endif

static Path executable;

Path::Path(const char* path)
    : Path(std::string(path)) {}

Path::Path(const std::string& path)
    : std::filesystem::path(path)
{
    #ifdef _MSC_VER
    int size = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        path.data(), static_cast<int>(path.size()),
        nullptr, 0
    );

    std::wstring data;
    data.resize(size);

    if (!MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        path.data(), static_cast<int>(path.size()),
        data.data(), static_cast<int>(data.size())
    )) return;

    *this = data;
    #endif
}

Path::Path(const std::wstring& path)
    : std::filesystem::path(path) {}

Path::Path(const std::filesystem::path& path)
    : std::filesystem::path(path) {}

void fs::init(const Path& executable)
{
    ::executable = executable;
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

bool fs::isFile(const Path& path)
{
    return std::filesystem::is_regular_file(path);
}

bool fs::isDirectory(const Path& path)
{
    return std::filesystem::is_directory(path);
}

bool fs::makeDirectory(const Path& path)
{
    return std::filesystem::create_directories(path);
}

Path fs::relativeToExe(const Path& path)
{
    return executable.parent_path() / path;
}
