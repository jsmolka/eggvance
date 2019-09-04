#include "microtimer.h"

// Based on https://stackoverflow.com/a/41862592

#include <windows.h>

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

MicroTimer::MicroTimer()
{
    timer = CreateWaitableTimer(NULL, TRUE, NULL);
}

MicroTimer::~MicroTimer()
{
    CloseHandle(timer);
}

void MicroTimer::sleep(u32 us) const
{
    LARGE_INTEGER ft;
    ft.QuadPart = -static_cast<s64>(us * 10);

    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
}

u64 MicroTimer::now() const
{
    return static_cast<u64>(static_cast<double>(PerfCounter()) * 1000000 / PerfFrequency());
}
