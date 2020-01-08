#pragma once

#include <filesystem>
#include <vector>

#include "integer.h"

struct Path : public std::filesystem::path
{
    Path() = default;
    Path(const char* path);
    Path(const std::string& path);
    Path(const std::wstring& path);
    Path(const std::filesystem::path& path);

    Path& operator=(const char* path);
    Path& operator=(const std::string& path);
    Path& operator=(const std::wstring& path);
    Path& operator=(const std::filesystem::path& path);
};

namespace fs
{
    void init(const Path& executable);

    bool read(const Path& file, std::vector<u8>& dst);
    bool write(const Path& file, std::vector<u8>& src);

    bool isDir(const Path& path);
    bool isFile(const Path& file);

    Path relativeToCwd(const Path& path);
    Path relativeToExe(const Path& path);
}
