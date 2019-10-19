#pragma once

#include <string>

class Config
{
public:
    void init(const std::string& dir);

    bool bios_skip;
    std::string bios_file;
};

extern Config config;
