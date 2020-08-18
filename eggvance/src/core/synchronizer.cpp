#include "synchronizer.h"

#include <thread>

Synchronizer::Synchronizer(double fps)
{
    delta_have = Duration(0);
    delta_want = Duration(Duration::rep(Duration::period::den / fps));
}

void Synchronizer::sync(const std::function<void(void)>& frame)
{
    auto beg = Clock::now();
    frame();
    auto end = Clock::now();

    delta_have += end - beg;
    if (delta_have < delta_want)
    {
        std::this_thread::sleep_for(delta_want - delta_have);
        delta_have += Clock::now() - end;
    }
    delta_have -= delta_want;
}
