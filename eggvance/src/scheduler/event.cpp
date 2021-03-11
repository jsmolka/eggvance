#include "event.h"

void Event::operator()(u64 now)
{
    u64 late = now - when;

    when = 0;
    callback(data, late);
}

bool Event::operator<(const Event& other) const
{
    return when < other.when;
}

bool Event::operator>(const Event& other) const
{
    return when > other.when;

}

bool Event::scheduled() const
{
    return when;
}
