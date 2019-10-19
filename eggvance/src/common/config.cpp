#include "config.h"

#include <filesystem>

namespace fs = std::filesystem;

Config config;

void Config::init(const std::string& dir)
{
    bios_file = fs::path(dir).append("bios.bin").string();
    bios_skip = true;
}
