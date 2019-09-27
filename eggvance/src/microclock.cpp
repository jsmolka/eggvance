#include "microclock.h"

// Based on https://stackoverflow.com/a/41862592

#include <windows.h>

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
