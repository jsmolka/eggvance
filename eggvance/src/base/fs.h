#pragma once

#include <array>
#include <fstream>
#include <vector>

#ifdef __cpp_lib_filesystem
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

namespace fs
{
    #ifdef __cpp_lib_filesystem
    using namespace std::filesystem;
    constexpr bool experimental = false;
    #else
    using namespace std::experimental::filesystem;
    constexpr bool experimental = true;
    #endif

    path make_path(const std::string& path);

    template<typename T>
    bool read(const path& file, std::vector<T>& dst)
    {
        auto stream = std::ifstream(file, std::ios::binary);
        if (!stream.is_open())
            return false;

        stream.seekg(0, std::ios::end);
        std::size_t size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        dst.resize((size + sizeof(T) - 1) / sizeof(T));

        stream.read(reinterpret_cast<char*>(dst.data()), size);

        return true;
    }

    template<typename T, std::size_t N>
    bool read(const path& file, std::array<T, N>& dst)
    {
        auto stream = std::ifstream(file, std::ios::binary);
        if (!stream.is_open())
            return false;

        stream.seekg(0, std::ios::end);
        std::size_t size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        if (size != (N * sizeof(T)))
            return false;

        stream.read(reinterpret_cast<char*>(dst.data()), size);

        return true;
    }

    template<typename T>
    bool write(const path& file, const T& src)
    {
        auto stream = std::ofstream(file, std::ios::binary);
        if (!stream.is_open())
            return false;

        stream.write(reinterpret_cast<const char*>(src.data()), src.size() * sizeof(typename T::value_type));

        return true;
    }
}
