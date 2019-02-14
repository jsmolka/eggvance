#include "log.h"

Log::Log(const char* function)
{
    std::cout << function << " - ";
}

Log::~Log()
{
    std::cout << "\n";
}
