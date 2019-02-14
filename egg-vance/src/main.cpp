#include <iostream>
#include <memory>

#include "core.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Invalid number of arguments\n";
        return 0;
    }

    std::make_shared<Core>()->run(argv[1]);

    return 0;
}