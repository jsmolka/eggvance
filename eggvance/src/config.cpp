#include "config.h"

Config& Config::instance()
{
    static Config config;
    return config;
}
