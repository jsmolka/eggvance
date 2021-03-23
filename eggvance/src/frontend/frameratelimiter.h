#pragma once

#include <chrono>
#include <functional>
#include <thread>

template<typename Clock = std::chrono::high_resolution_clock>
class FrameRateLimiter
{
public:
    FrameRateLimiter(double fps)
    {
        accumulated = Duration(0);
        frame_delta = Duration(Duration::rep(Duration::period::den / fps));
    }

    template<typename Frame>
    void run(Frame frame)
    {
        accumulated += measure(frame);

        if (accumulated < frame_delta)
        {
            accumulated += measure([this]()
            {
                std::this_thread::sleep_for(frame_delta - accumulated);
            });
        }
        accumulated -= frame_delta;
    }

private:
    using Duration = typename Clock::duration;

    template<typename Callback>
    static Duration measure(Callback callback)
    {
        const auto begin = Clock::now();

        std::invoke(callback);

        return Clock::now() - begin;
    }

    Duration accumulated;
    Duration frame_delta;
};
