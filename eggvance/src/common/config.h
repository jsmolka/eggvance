#pragma once

class Config
{
public:
    static Config& instance();

    bool skip_bios = true;

private:
    Config() = default;
};

#define cfg Config::instance()
