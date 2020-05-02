#include "fs.h"

#ifdef _WIN32
#include <windows.h>
#endif

fs::path fs::make_path(const std::string& path)
{
    #ifdef _WIN32
    int size = MultiByteToWideChar(
        CP_UTF8, 0,
        path.data(), static_cast<int>(path.size()),
        nullptr, 0
    );

    std::wstring utf8;
    utf8.resize(size);

    MultiByteToWideChar(
        CP_UTF8, 0,
        path.data(), static_cast<int>(path.size()),
        utf8.data(), static_cast<int>(utf8.size())
    );

    return utf8;
    #else
    return path;
    #endif
}
