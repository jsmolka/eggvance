#include "microclock.h"

// Based on https://stackoverflow.com/a/41862592

#ifdef _MSC_VER
#  include <windows.h>
#else
#  include <time.h>
#  include <errno.h>
#endif

#ifdef _MSC_VER

class TimerHandle
{
public:
    TimerHandle()
    {
        timer = CreateWaitableTimer(NULL, TRUE, NULL);
    }
    ~TimerHandle()
    {
        CloseHandle(timer);
    }
    inline operator HANDLE() const
    {
        return timer;
    }
private:
    HANDLE timer;
};

static TimerHandle timer;

inline u64 GetPerfFrequency()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
}

inline u64 PerfFrequency()
{
    static u64 freq = GetPerfFrequency();
    return freq;
}

inline u64 PerfCounter()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

u64 micro_clock::now()
{
    return static_cast<u64>(static_cast<double>(PerfCounter()) * 1000000 / PerfFrequency());
}

void micro_clock::sleep(u32 us)
{
    LARGE_INTEGER relative_time;
    relative_time.QuadPart = -static_cast<s64>(us * 10);

    SetWaitableTimer(timer, &relative_time, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
}

#else

u64 micro_clock::now()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return static_cast<u64>(now.tv_sec) * 1000000 + now.tv_nsec / 1000;
}

void micro_clock::sleep(u32 us)
{
    struct timespec ts;
    ts.tv_sec = us / 1000000;
    ts.tv_nsec = us % 1000000 * 1000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

#endif
