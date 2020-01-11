#pragma once

#if __has_include(<filesystem>)
#include <filesystem>
namespace std_filesystem = std::filesystem;
#else
#include <experimental/filesystem>
namespace std_filesystem = std::experimental::filesystem;
#endif
#include <vector>

#include "integer.h"

struct Path : public std_filesystem::path
{
    Path() = default;
    Path(const char* path);
    Path(const std::string& path);
    Path(const std::wstring& path);
    Path(const std_filesystem::path& path);

    template<typename T>
    Path& operator=(const T& path)
    {
        return *this = Path(path);
    }
};

namespace fs
{
    void init(const Path& executable);

    bool read(const Path& file, std::vector<u8>& dst);
    bool write(const Path& file, std::vector<u8>& src);

    bool isFile(const Path& path);
    bool isDirectory(const Path& path);

    bool makeDirectory(const Path& path);
    Path relativeToExe(const Path& path);
}
