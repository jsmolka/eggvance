#pragma once

#include <optional>
#include <nfd/nfd.h>

#include "base/filesystem.h"

std::optional<fs::path> openFileDialog(const char* filter = nullptr)
{
    nfdchar_t* file = nullptr;
    if (NFD_OpenDialog(filter, nullptr, &file) != NFD_OKAY)
        return std::nullopt;

    const auto result = fs::u8path(file);
    free(file);

    return result;
}

std::optional<fs::path> openPathDialog()
{
    nfdchar_t* path = nullptr;
    if (NFD_PickFolder(NULL, &path) != NFD_OKAY)
        return std::nullopt;

    const auto result = fs::u8path(path);
    free(path);

    return result;
}
