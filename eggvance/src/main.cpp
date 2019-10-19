#include <memory>

#include "emulator.h"

int main(int argc, char* argv[])
{
    auto emulator = std::make_unique<Emulator>();
    if (!emulator->init(argc, argv))
        return 1;

    emulator->run();

    return 0;
}
