#pragma once

#include <chrono>
#include <functional>
#include <thread>

class Synchronizer
{
public:
    Synchronizer(double fps)
    {
        delta_have = Duration(0);
        delta_want = Duration(Duration::rep(Duration::period::den / fps));
    }

    template<typename Callback>
    void sync(Callback callback)
    {
        delta_have += measure(callback);

        if (delta_have < delta_want)
        {
            delta_have += measure([&]() {
                std::this_thread::sleep_for(delta_want - delta_have);
            });
        }
        delta_have -= delta_want;
    }

private:
    using Clock    = std::chrono::high_resolution_clock;
    using Duration = std::chrono::high_resolution_clock::duration;

    template<typename Callback>
    Duration measure(Callback callback)
    {
        const auto begin = Clock::now();

        std::invoke(callback);

        return Clock::now() - begin;
    }

    Duration delta_have;
    Duration delta_want;
};
