#pragma once

#include <chrono>
#include <functional>
#include <thread>

class FrameRateLimiter
{
public:
    FrameRateLimiter();

    void reset();
    void queueReset();
    void setMultiplier(double multiplier);
    bool isFastForward() const;

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

    void setFps(double fps);

    double multiplier = 1;
    Duration accumulated;
    Duration frame_delta;
    bool queue_reset = false;
};
