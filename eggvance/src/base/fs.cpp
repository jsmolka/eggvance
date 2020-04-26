#include "fs.h"

#ifdef _WIN32
#include <windows.h>
#endif

static fs::path exe;

std::wstring encode(const std::string& string)
{
    #ifdef _WIN32
    int size = MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        string.data(), static_cast<int>(string.size()),
        nullptr, 0
    );

    std::wstring result;
    result.resize(size);

    MultiByteToWideChar(
        CP_UTF8, MB_ERR_INVALID_CHARS,
        string.data(), static_cast<int>(string.size()),
        result.data(), static_cast<int>(result.size())
    );

    return result;
    #else
    return std::wstring(string.begin(), string.end());
    #endif
}

void fs::init(int argc, char* argv[])
{
    if (argc > 0)
        exe = canonical(encode(argv[0]));
}

fs::path fs::exe_relative(const fs::path& path)
{
    return exe.parent_path() / path;
}

template<>
fs::path fs::make_path(const char* const& path)
{
    return fs::path(encode(path));
}

template<>
fs::path fs::make_path(const std::string& path)
{
    return fs::path(encode(path));
}
