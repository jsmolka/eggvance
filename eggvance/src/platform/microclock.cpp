#include "microclock.h"

#ifdef _MSC_VER
#include <windows.h>
#else
#include <errno.h>
#include <time.h>
#  ifdef __APPLE__
#  include <mach/clock.h>
#  include <mach/mach.h>
#  endif
#endif

#ifdef _MSC_VER

struct WaitableTimer
{
    WaitableTimer()
    {
        handle = CreateWaitableTimer(NULL, TRUE, NULL);
    }

    ~WaitableTimer()
    {
        CloseHandle(handle);
    }

    HANDLE handle;
};

static WaitableTimer timer;

u64 micro_clock::now()
{
    static u64 freq = []() {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return freq.QuadPart;
    }();

    LARGE_INTEGER perf_counter;
    QueryPerformanceCounter(&perf_counter);

    return static_cast<u64>(static_cast<double>(perf_counter.QuadPart) * 1000000 / freq);
}

void micro_clock::sleep(u32 us)
{
    LARGE_INTEGER relative_time;
    relative_time.QuadPart = -static_cast<s64>(10 * us);

    SetWaitableTimer(timer.handle, &relative_time, 0, NULL, NULL, 0);
    WaitForSingleObject(timer.handle, INFINITE);
}

#else

u64 micro_clock::now()
{
    #ifdef __APPLE__
    clock_serv_t cs;
    mach_timespec_t now;

    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cs);
    clock_get_time(cs, &now);
    mach_port_deallocate(mach_task_self(), cs);
    #else
    timespec now;

    clock_gettime(CLOCK_MONOTONIC, &now);
    #endif

    return static_cast<u64>(now.tv_sec) * 1000000 + now.tv_nsec / 1000;
}

void micro_clock::sleep(u32 us)
{
    timespec ts;
    ts.tv_sec = us / 1000000;
    ts.tv_nsec = us % 1000000 * 1000;

    while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

#endif
