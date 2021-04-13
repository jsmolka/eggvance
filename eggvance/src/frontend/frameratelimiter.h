#pragma once

#include <chrono>
#include <functional>
#include <thread>

class FrameRateLimiter
{
public:
    FrameRateLimiter(double fps);

    void reset();
    void queueReset();

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

        if (queue_reset)
            reset();
    }

private:
    using Clock    = std::chrono::high_resolution_clock;
    using Duration = std::chrono::high_resolution_clock::duration;

    template<typename Callback>
    static Duration measure(Callback callback)
    {
        const auto begin = Clock::now();

        std::invoke(callback);

        return Clock::now() - begin;
    }

    Duration accumulated;
    Duration frame_delta;
    bool queue_reset = false;
};
